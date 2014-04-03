#include "RubyEditorWidget.h"

#include "RubyEditor.h"
#include "RubyHighlighter.h"

#include <texteditor/basetextdocument.h>

namespace RubyEditor {

RubyEditorWidget::RubyEditorWidget(QWidget* parent)
    : TextEditor::BaseTextEditorWidget(parent)
{
    new RubyHighlighter(baseTextDocument().data());
}

TextEditor::BaseTextEditor* RubyEditorWidget::createEditor()
{
    return new RubyEditor(this);
}

}

#include "RubyEditorWidget.moc"
