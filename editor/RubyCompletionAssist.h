#ifndef Ruby_CompletionAssist_h
#define Ruby_CompletionAssist_h

#include <texteditor/codeassist/completionassistprovider.h>
#include <texteditor/codeassist/iassistprocessor.h>
#include <texteditor/snippets/snippetassistcollector.h>

#include <QIcon>

namespace Ruby {

class CompletionAssistProvider : public TextEditor::CompletionAssistProvider
{
public:
    TextEditor::IAssistProcessor *createProcessor() const override;

    int activationCharSequenceLength() const override { return 1; }
    bool isActivationCharSequence(const QString &sequence) const override;
};

class CompletionAssistProcessor : public TextEditor::IAssistProcessor
{
public:
    CompletionAssistProcessor();
    TextEditor::IAssistProposal *perform(const TextEditor::AssistInterface *interface) override;
private:
    // TODO: Share this icons with all instances
    QIcon m_methodIcon;
    QIcon m_identifierIcon;
    QIcon m_constantIcon;
    QIcon m_symbolIcon;
    TextEditor::SnippetAssistCollector m_snippetCollector;
};

}

#endif
