#ifndef Ruby_CompletionAssist_h
#define Ruby_CompletionAssist_h

#include <texteditor/codeassist/completionassistprovider.h>
#include <texteditor/codeassist/iassistprocessor.h>

namespace Ruby {

class CompletionAssistProvider : public TextEditor::CompletionAssistProvider
{
    Q_OBJECT
public:
    bool supportsEditor(const Core::Id& editorId) const override;
    TextEditor::IAssistProcessor* createProcessor() const override;

    int activationCharSequenceLength() const override { return 1; }
    bool isActivationCharSequence(const QString& sequence) const override;

};

class CompletionAssistProcessor : public TextEditor::IAssistProcessor
{
public:
    TextEditor::IAssistProposal* perform(const TextEditor::IAssistInterface* interface) override;
};

}

#endif
