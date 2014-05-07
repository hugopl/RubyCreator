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
{
    const TextEditor::FontSettings fontSettings = TextEditor::TextEditorSettings::fontSettings();
    m_formats = fontSettings.toTextCharFormats({
                                                   TextEditor::C_NUMBER,
                                                   TextEditor::C_STRING,
                                                   TextEditor::C_KEYWORD,
                                                   TextEditor::C_TYPE,
                                                   TextEditor::C_FIELD,
                                                   TextEditor::C_JS_SCOPE_VAR,
                                                   TextEditor::C_OPERATOR,
                                                   TextEditor::C_COMMENT,
                                                   TextEditor::C_DOXYGEN_COMMENT,
                                                   TextEditor::C_TEXT,
                                                   TextEditor::C_VISUAL_WHITESPACE
                                               });
    QTextCharFormat constantFormat = m_formats[Token::Type];
    constantFormat.setFontWeight(100);
    m_formats << constantFormat;
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
    Scanner scanner(text.constData(), text.size());
    scanner.setState(state);

    Token token;
    while ((token = scanner.read()).kind != Token::EndOfBlock)
        setFormat(token.position, token.length, formatForToken(token));

    return scanner.state();
}

QTextCharFormat Highlighter::formatForToken(const Token& token)
{
    Q_ASSERT(token.kind < m_formats.size());
    return m_formats[token.kind];
}




}
