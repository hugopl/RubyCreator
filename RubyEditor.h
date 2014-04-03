#ifndef RubyEditor_h
#define RubyEditor_h

#include <texteditor/basetexteditor.h>

namespace RubyEditor {

class RubyEditorWidget;

class RubyEditor : public TextEditor::BaseTextEditor
{
public:
    RubyEditor(RubyEditorWidget* parent);

    Core::Id id() const override;
};

}

#endif
