#include "RubyQuickFixAssistProvider.h"

#include "RubyQuickFixes.h"
#include "../RubyConstants.h"

#include <coreplugin/id.h>
#include <texteditor/codeassist/iassistprocessor.h>
#include <texteditor/codeassist/genericproposal.h>
#include <texteditor/quickfix.h>
#include <extensionsystem/pluginmanager.h>

namespace Ruby {

QuickFixAssistProvider::QuickFixAssistProvider(QObject *parent)
    : TextEditor::IAssistProvider(parent)
{
}

TextEditor::IAssistProvider::RunType QuickFixAssistProvider::runType() const
{
    return TextEditor::IAssistProvider::Synchronous;
}

class RubyQuickFixAssistProcessor : public TextEditor::IAssistProcessor
{
    TextEditor::IAssistProposal *perform(const TextEditor::AssistInterface *interface) override
    {
        QSharedPointer<const TextEditor::AssistInterface> assistInterface(interface);

        TextEditor::QuickFixOperations quickFixes;

        for (QuickFixFactory *factory : QuickFixFactory::quickFixFactories())
            factory->match(assistInterface, quickFixes);

        return TextEditor::GenericProposal::createProposal(interface, quickFixes);
    }
};

TextEditor::IAssistProcessor *QuickFixAssistProvider::createProcessor() const
{
    return new RubyQuickFixAssistProcessor;
}

}
