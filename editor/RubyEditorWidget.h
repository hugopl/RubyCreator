#ifndef RubyEditorWidget_h
#define RubyEditorWidget_h

#include <QTimer>

#include <texteditor/texteditor.h>
#include <utils/uncommentselection.h>

namespace Ruby {

class EditorWidget : public TextEditor::TextEditorWidget
{
    Q_OBJECT

public:
    EditorWidget();

    Link findLinkAt(const QTextCursor &cursor, bool, bool inNextSplit) Q_DECL_OVERRIDE;
    void unCommentSelection() Q_DECL_OVERRIDE;

protected:
    void finalizeInitialization() Q_DECL_OVERRIDE;

private slots:
    void scheduleCodeModelUpdate();
    void updateCodeModel();

private:
    QRegExp m_wordRegex;
    Utils::CommentDefinition m_commentDefinition;
    QTimer m_updateCodeModelTimer;
};

}

#endif
