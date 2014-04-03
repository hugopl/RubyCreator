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
    void scheduleDocumentUpdate();
    void updateDocumentNow();

private:
    QTimer m_updateDocumentTimer;
};

}

#endif
