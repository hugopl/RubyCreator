#include "RubyEditorWidget.h"

#include "RubyEditor.h"
#include "RubyHighlighter.h"

#include <texteditor/basetextdocument.h>

namespace Ruby {

EditorWidget::EditorWidget(QWidget* parent)
    : TextEditor::BaseTextEditorWidget(parent)
{
    new Highlighter(baseTextDocument().data());
}

TextEditor::BaseTextEditor* EditorWidget::createEditor()
{
    return new Editor(this);
}

}

#include "RubyEditorWidget.moc"
