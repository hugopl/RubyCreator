#ifndef Ruby_Editor_h
#define Ruby_Editor_h

#include <texteditor/basetexteditor.h>
#include <QTimer>

namespace Ruby {

class EditorWidget;

class Editor : public TextEditor::BaseTextEditor
{
    Q_OBJECT
public:
    Editor(EditorWidget* parent);

    TextEditor::CompletionAssistProvider* completionAssistProvider() override;
private slots:
    void scheduleCodeModelUpdate();
    void updateCodeModel();

private:
    QTimer m_updateCodeModelTimer;
};

}

#endif
