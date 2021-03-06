#ifndef RubyEditorWidget_h
#define RubyEditorWidget_h

#include <QRegularExpression>
#include <QTimer>

#include <texteditor/texteditor.h>
#include <utils/uncommentselection.h>

namespace Ruby {

class AmbigousMethodAssistProvider;

class EditorWidget : public TextEditor::TextEditorWidget
{
    Q_OBJECT

public:
    EditorWidget();
    ~EditorWidget();

    void findLinkAt(const QTextCursor &cursor,
                    Utils::ProcessLinkCallback &&processLinkCallback,
                    bool resolveTarget = true,
                    bool inNextSplit = false) override;
    void unCommentSelection() override;

    void aboutToOpen(const QString &fileName, const QString &realFileName) override;

protected:
    void finalizeInitialization() override;

private:
    void scheduleCodeModelUpdate();

    void scheduleRubocopUpdate();
    void updateCodeModel();
    void updateRubocop();

private:
    QRegularExpression m_wordRegex;
    Utils::CommentDefinition m_commentDefinition;
    QTimer m_updateCodeModelTimer;
    bool m_codeModelUpdatePending = false;

    QTimer m_updateRubocopTimer;
    bool m_rubocopUpdatePending = false;

    QString m_filePathDueToMaybeABug;

    AmbigousMethodAssistProvider *m_ambigousMethodAssistProvider;
};

}

#endif
