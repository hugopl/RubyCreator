#include "RubyEditorWidget.h"

#include "RubyEditor.h"
#include "RubyHighlighter.h"

#include <texteditor/basetextdocument.h>

namespace Ruby {

EditorWidget::EditorWidget()
{
    setParenthesesMatchingEnabled(true);
    setMarksVisible(true);

    new Highlighter(baseTextDocument());
}

TextEditor::BaseTextEditor* EditorWidget::createEditor()
{
    return new Editor(this);
}

}

#include "RubyEditorWidget.moc"
