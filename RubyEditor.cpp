#include "RubyEditor.h"

#include "RubyEditorConstants.h"
#include "RubyEditorWidget.h"

namespace RubyEditor {

RubyEditor::RubyEditor(RubyEditorWidget* parent)
    : TextEditor::BaseTextEditor(parent)
{
}

Core::Id RubyEditor::id() const
{
    return Core::Id(Constants::EditorId);
}

}
