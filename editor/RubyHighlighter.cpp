#include "RubyHighlighter.h"
#include "RubyScanner.h"

#include <texteditor/basetextdocument.h>
#include <texteditor/basetextdocumentlayout.h>
#include <texteditor/texteditorconstants.h>
#include <texteditor/texteditorsettings.h>
#include <texteditor/fontsettings.h>
#include <QDebug>

namespace Ruby {

QVector<QTextCharFormat> Highlighter::m_formats;

static void initFormats(QVector<QTextCharFormat> &formats)
{
    formats.resize(Token::EndOfBlock);

    QTextCharFormat &keywordFormat = formats[Token::Keyword];
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
        formats[keywordTokens[tk]] = keywordFormat;


    formats[Token::KeywordVisibility].setForeground(QColor(0, 0, 255));
    formats[Token::KeywordVisibility].setFontWeight(75);
    formats[Token::KeywordSelf].setForeground(QColor(68, 85, 136));
    formats[Token::KeywordSelf].setFontWeight(75);
    formats[Token::String].setForeground(QColor(208, 16, 64));
    formats[Token::InStringCode].setForeground(QColor(0, 110, 40));
    formats[Token::Backtick] = formats[Token::String];
    formats[Token::Comment].setForeground(QColor(153, 153, 136));
    formats[Token::Constant].setForeground(QColor(0, 128, 128));
    formats[Token::Global].setForeground(QColor(0, 128, 128));
    formats[Token::Regexp].setForeground(QColor(0, 153, 38));
    formats[Token::ClassField].setForeground(QColor(0, 128, 128));
    formats[Token::Number].setForeground(QColor(0, 153, 153));
    formats[Token::Symbol].setForeground(QColor(153, 0, 115));
    formats[Token::Method].setForeground(QColor(153, 0, 0));
    formats[Token::Method].setFontWeight(75);
    formats[Token::Parameter].setFontItalic(true);
    formats[Token::Parameter].setForeground(QColor(0, 134, 179));
}

Highlighter::Highlighter(QTextDocument *parent)
    : TextEditor::SyntaxHighlighter(parent)
{
    if (m_formats.empty())
        initFormats(m_formats);
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
    int nextIndentLevel = indentLevel + scanner.indentVariation();
    if (scanner.didBlockInterrupt())
        indentLevel--;

    if (nextIndentLevel < 0)
        nextIndentLevel = 0;

    TextEditor::BaseTextDocumentLayout::setFoldingIndent(currentBlock(), indentLevel);
    TextEditor::BaseTextDocumentLayout::setParentheses(currentBlock(), m_currentBlockParentheses);
    return (nextIndentLevel << 8) | scanner.state();
}

QTextCharFormat Highlighter::formatForToken(const Token &token)
{
    Q_ASSERT(token.kind < m_formats.size());
    return m_formats[token.kind];
}

}
