#include "RubyHighlighter.h"
#include "RubyScanner.h"

#include <texteditor/basetextdocument.h>
#include <texteditor/texteditorconstants.h>
#include <texteditor/texteditorsettings.h>
#include <texteditor/fontsettings.h>
#include <QDebug>

namespace Ruby {

Highlighter::Highlighter(QTextDocument* parent)
    : TextEditor::SyntaxHighlighter(parent)
    , m_formats(Token::EndOfBlock)
{
    auto& keywordFormat = m_formats[Token::Keyword];
    keywordFormat.setFontWeight(100);
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
    for (const auto& tk : keywordTokens)
        m_formats[tk] = keywordFormat;


    m_formats[Token::KeywordSelf].setForeground(QColor(68, 85, 136));
    m_formats[Token::KeywordSelf].setFontWeight(100);
    m_formats[Token::String].setForeground(QColor(208, 16, 64));
    m_formats[Token::Comment].setForeground(QColor(153, 153, 136));
    m_formats[Token::Constant].setForeground(QColor(0, 128, 128));
    m_formats[Token::Global].setForeground(QColor(0, 128, 128));
    m_formats[Token::Regexp].setForeground(QColor(0, 153, 38));
    m_formats[Token::ClassField].setForeground(QColor(0, 128, 128));
    m_formats[Token::Number].setForeground(QColor(0, 153, 153));
    m_formats[Token::Symbol].setForeground(QColor(153, 0, 115));
    m_formats[Token::Method].setForeground(QColor(153, 0, 0));
    m_formats[Token::Method].setFontWeight(100);
    m_formats[Token::Parameter].setFontItalic(true);
    m_formats[Token::Parameter].setForeground(QColor(0, 134, 179));
}

void Highlighter::highlightBlock(const QString& text)
{
    int initialState = previousBlockState();
    if (initialState == -1)
        initialState = 0;
    setCurrentBlockState(highlightLine(text, initialState));
}

int Highlighter::highlightLine(const QString& text, int state)
{
    m_currentBlockParentheses.clear();

    Scanner scanner(&text);
    scanner.setState(state & 0xff);

    static QString openParenthesis = QStringLiteral("([{");
    static QString closeParenthesis = QStringLiteral(")]}");

    Token token;
    while ((token = scanner.read()).kind != Token::EndOfBlock) {
        setFormat(token.position, token.length, formatForToken(token));
        if (token.kind == Token::Operator) {
            QChar ch = text[token.position];
            if (openParenthesis.contains(ch))
                m_currentBlockParentheses << Parenthesis(Parenthesis::Opened, ch, token.position);
            else if (closeParenthesis.contains(ch))
                m_currentBlockParentheses << Parenthesis(Parenthesis::Closed, ch, token.position);
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

    TextEditor::BaseTextDocumentLayout::setFoldingIndent(currentBlock(), indentLevel);
    TextEditor::BaseTextDocumentLayout::setParentheses(currentBlock(), m_currentBlockParentheses);
    return (nextIndentLevel << 8) | scanner.state();
}

QTextCharFormat Highlighter::formatForToken(const Token& token)
{
    Q_ASSERT(token.kind < m_formats.size());
    return m_formats[token.kind];
}




}
