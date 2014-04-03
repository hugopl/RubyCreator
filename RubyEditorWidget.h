#ifndef RubyEditorWidget_h
#define RubyEditorWidget_h

#include <texteditor/basetexteditor.h>

namespace RubyEditor {

class RubyEditorWidget : public TextEditor::BaseTextEditorWidget
{
    Q_OBJECT
public:
    RubyEditorWidget(QWidget* parent);
    TextEditor::BaseTextEditor* createEditor() override;
};

}

#endif
