#ifndef RubyEditorWidget_h
#define RubyEditorWidget_h

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

    Link findLinkAt(const QTextCursor &cursor, bool, bool inNextSplit) override;
    void unCommentSelection() override;

    void aboutToOpen(const QString &fileName, const QString &realFileName) override;

protected:
    void finalizeInitialization() override;

private slots:
    void scheduleCodeModelUpdate();
    void maybeUpdateCodeModel();

    void scheduleRubocopUpdate();
    void maybeUpdateRubocop();

private:
    QRegExp m_wordRegex;
    Utils::CommentDefinition m_commentDefinition;
    QTimer m_updateCodeModelTimer;
    bool m_codeModelUpdatePending;

    QTimer m_updateRubocopTimer;
    bool m_rubocopUpdatePending;

    QString m_filePathDueToMaybeABug;

    AmbigousMethodAssistProvider *m_ambigousMethodAssistProvider;

    void updateCodeModel();
    void updateRubocop();
};

}

#endif
