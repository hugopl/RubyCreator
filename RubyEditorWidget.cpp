#include "RubyEditorWidget.h"

#include "RubyEditor.h"

namespace RubyEditor {

RubyEditorWidget::RubyEditorWidget(QWidget* parent)
    : TextEditor::BaseTextEditorWidget(parent)
{
}

TextEditor::BaseTextEditor* RubyEditorWidget::createEditor()
{
    return new RubyEditor(this);
}

}

#include "RubyEditorWidget.moc"
