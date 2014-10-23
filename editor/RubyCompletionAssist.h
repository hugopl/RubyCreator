#ifndef Ruby_CompletionAssist_h
#define Ruby_CompletionAssist_h

#include <texteditor/codeassist/completionassistprovider.h>
#include <texteditor/codeassist/iassistprocessor.h>

#include <QIcon>

namespace Ruby {

class CompletionAssistProvider : public TextEditor::CompletionAssistProvider
{
    Q_OBJECT
public:
    TextEditor::IAssistProcessor *createProcessor() const Q_DECL_OVERRIDE;

    int activationCharSequenceLength() const Q_DECL_OVERRIDE { return 1; }
    bool isActivationCharSequence(const QString &sequence) const Q_DECL_OVERRIDE;
    bool supportsEditor(Core::Id editorId) const Q_DECL_OVERRIDE;
};

class CompletionAssistProcessor : public TextEditor::IAssistProcessor
{
public:
    CompletionAssistProcessor();
    TextEditor::IAssistProposal *perform(const TextEditor::AssistInterface *interface) Q_DECL_OVERRIDE;
private:
    // TODO: Share this icons with all instances
    QIcon m_methodIcon;
    QIcon m_identifierIcon;
    QIcon m_constantIcon;
    QIcon m_symbolIcon;
};

}

#endif
