#ifndef RubyEditorWidget_h
#define RubyEditorWidget_h

#include <texteditor/basetexteditor.h>

namespace Ruby {

class EditorWidget : public TextEditor::BaseTextEditorWidget
{
    Q_OBJECT
public:
    EditorWidget(QWidget* parent);
    TextEditor::BaseTextEditor* createEditor() override;
};

}

#endif
