#include "RubyHighlighter.h"
#include "RubyScanner.h"

#include <texteditor/basetextdocument.h>
#include <texteditor/texteditorconstants.h>
#include <texteditor/texteditorsettings.h>
#include <texteditor/fontsettings.h>
#include <QDebug>

namespace Ruby {

Highlighter::Highlighter(TextEditor::BaseTextDocument* parent)
    : TextEditor::SyntaxHighlighter(parent)
    , m_formats(Token::EndOfBlock)
{
    m_formats[Token::Keyword].setFontWeight(100);
    m_formats[Token::KeywordDef] = m_formats[Token::Keyword];
    m_formats[Token::KeywordSelf] = m_formats[Token::Keyword];
    m_formats[Token::String].setForeground(QColor(191, 3, 3));
    m_formats[Token::Comment].setForeground(QColor(137, 136, 135));
    m_formats[Token::Type].setForeground(QColor(0, 87, 174));
    m_formats[Token::Constant].setFontWeight(100);
    m_formats[Token::Constant].setForeground(QColor(187, 17, 136));
    m_formats[Token::Global].setForeground(QColor(192, 0, 0));
    m_formats[Token::Regexp].setForeground(QColor(74, 87, 4));
    m_formats[Token::ClassField].setForeground(QColor(0, 110, 40));
    m_formats[Token::Number].setForeground(QColor(176, 128, 0));
    m_formats[Token::Symbol].setForeground(QColor(212, 0, 0));
    m_formats[Token::Method].setForeground(QColor(0, 87, 174));
    m_formats[Token::Method].setFontWeight(100);
    m_formats[Token::Parameter].setFontItalic(true);
    m_formats[Token::Parameter].setForeground(QColor(74, 87, 4));
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
    scanner.setState(state);

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

    TextEditor::BaseTextDocumentLayout::setParentheses(currentBlock(), m_currentBlockParentheses);
    return scanner.state();
}

QTextCharFormat Highlighter::formatForToken(const Token& token)
{
    Q_ASSERT(token.kind < m_formats.size());
    return m_formats[token.kind];
}




}
