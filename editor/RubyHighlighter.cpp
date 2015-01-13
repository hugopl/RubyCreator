#include "RubyHighlighter.h"
#include "RubyScanner.h"

#include <texteditor/textdocument.h>
#include <texteditor/texteditorconstants.h>
#include <texteditor/texteditorsettings.h>
#include <texteditor/fontsettings.h>
#include <QDebug>

namespace Ruby {

Highlighter::Highlighter(QTextDocument *parent)
    : TextEditor::SyntaxHighlighter(parent)
    , m_formats(Token::EndOfBlock)
{
    QTextCharFormat &keywordFormat = m_formats[Token::Keyword];
    keywordFormat.setFontWeight(75);
    const Token::Kind keywordTokens[] = {
        Token::KeywordDef,
        Token::KeywordClass,
        Token::KeywordModule,
        Token::KeywordFlowControl,
        Token::KeywordLoop,
        Token::KeywordBlockStarter,
        Token::KeywordEnd,
        Token::KeywordElseElsIfRescueEnsure
    };
    for (unsigned tk = 0; tk < sizeof(keywordTokens) / sizeof(*keywordTokens); ++tk)
        m_formats[keywordTokens[tk]] = keywordFormat;


    m_formats[Token::KeywordVisibility].setForeground(QColor(0, 0, 255));
    m_formats[Token::KeywordVisibility].setFontWeight(75);
    m_formats[Token::KeywordSelf].setForeground(QColor(68, 85, 136));
    m_formats[Token::KeywordSelf].setFontWeight(75);
    m_formats[Token::String].setForeground(QColor(208, 16, 64));
    m_formats[Token::InStringCode].setForeground(QColor(0, 110, 40));
    m_formats[Token::Backtick] = m_formats[Token::String];
    m_formats[Token::Comment].setForeground(QColor(153, 153, 136));
    m_formats[Token::Constant].setForeground(QColor(0, 128, 128));
    m_formats[Token::Global].setForeground(QColor(0, 128, 128));
    m_formats[Token::Regexp].setForeground(QColor(0, 153, 38));
    m_formats[Token::ClassField].setForeground(QColor(0, 128, 128));
    m_formats[Token::Number].setForeground(QColor(0, 153, 153));
    m_formats[Token::Symbol].setForeground(QColor(153, 0, 115));
    m_formats[Token::Method].setForeground(QColor(153, 0, 0));
    m_formats[Token::Method].setFontWeight(75);
    m_formats[Token::Parameter].setFontItalic(true);
    m_formats[Token::Parameter].setForeground(QColor(0, 134, 179));
}

void Highlighter::highlightBlock(const QString &text)
{
    int initialState = previousBlockState();
    if (initialState == -1)
        initialState = 0;
    setCurrentBlockState(highlightLine(text, initialState));
}

int Highlighter::highlightLine(const QString &text, int state)
{
    m_currentBlockParentheses.clear();

    Scanner scanner(&text);
    scanner.setState(state & 0xff);

    Token token;
    while ((token = scanner.read()).kind != Token::EndOfBlock) {
        setFormat(token.position, token.length, formatForToken(token));
        const bool isOpenBrace = token.kind == Token::ParenOpen;
        if (isOpenBrace || token.kind == Token::ParenClose) {
            m_currentBlockParentheses << Parenthesis(isOpenBrace ? Parenthesis::Opened : Parenthesis::Closed,
                                                     text[token.position], token.position);
        }
    }

    int indentLevel = state >> 8;
    int nextIndentLevel = indentLevel;
    if (scanner.didBlockStart())
        nextIndentLevel++;
    if (scanner.didBlockEnd())
        nextIndentLevel--;
    if (scanner.didBlockInterrupt())
        indentLevel--;

    if (nextIndentLevel < 0)
        nextIndentLevel = 0;

    TextEditor::TextDocumentLayout::setFoldingIndent(currentBlock(), indentLevel);
    TextEditor::TextDocumentLayout::setParentheses(currentBlock(), m_currentBlockParentheses);
    return (nextIndentLevel << 8) | scanner.state();
}

QTextCharFormat Highlighter::formatForToken(const Token &token)
{
    Q_ASSERT(token.kind < m_formats.size());
    return m_formats[token.kind];
}

}
