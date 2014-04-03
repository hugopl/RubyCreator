#ifndef RubyEditor_h
#define RubyEditor_h

#include <texteditor/basetexteditor.h>
#include <QTimer>

namespace RubyEditor {

class RubyEditorWidget;

class RubyEditor : public TextEditor::BaseTextEditor
{
    Q_OBJECT
public:
    RubyEditor(RubyEditorWidget* parent);

    Core::Id id() const override;

private slots:
    void scheduleCodeModelUpdate();
    void updateCodeModel();

private:
    QTimer m_updateCodeModelTimer;
};

}

#endif
