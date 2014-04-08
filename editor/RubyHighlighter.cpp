#include "RubyHighlighter.h"
#include "RubyScanner.h"

#include <texteditor/basetextdocument.h>
#include <texteditor/texteditorconstants.h>
#include <QDebug>

namespace Ruby {

Highlighter::Highlighter(TextEditor::BaseTextDocument* parent)
    : TextEditor::SyntaxHighlighter(parent)
{
    static QVector<TextEditor::TextStyle> categories;
    if (categories.isEmpty()) {
        categories << TextEditor::C_NUMBER
                   << TextEditor::C_STRING
                   << TextEditor::C_KEYWORD
                   << TextEditor::C_TYPE
                   << TextEditor::C_FIELD
                   << TextEditor::C_JS_SCOPE_VAR
                   << TextEditor::C_OPERATOR
                   << TextEditor::C_COMMENT
                   << TextEditor::C_DOXYGEN_COMMENT
                   << TextEditor::C_TEXT
                   << TextEditor::C_VISUAL_WHITESPACE
                   << TextEditor::C_STRING;
    }
    setTextFormatCategories(categories);
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
        setFormat(token.position, token.length, formatForCategory(token.kind));

    return scanner.state();
}

}
