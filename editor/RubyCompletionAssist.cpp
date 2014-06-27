#include "RubyCompletionAssist.h"
#include "RubyConstants.h"
#include "RubyCodeModel.h"

#include <texteditor/codeassist/basicproposalitem.h>
#include <texteditor/codeassist/basicproposalitemlistmodel.h>
#include <texteditor/codeassist/genericproposal.h>
#include <texteditor/codeassist/iassistinterface.h>

#include <QTextDocument>
#include <QTextBlock>
#include <QDebug>

namespace Ruby {
bool CompletionAssistProvider::supportsEditor(const Core::Id& editorId) const
{
    return editorId == Constants::EditorId;
}

TextEditor::IAssistProcessor* CompletionAssistProvider::createProcessor() const
{
    return new CompletionAssistProcessor;
}

bool CompletionAssistProvider::isActivationCharSequence(const QString& sequence) const
{
    return sequence.at(0) == QLatin1Char('.');
}

TextEditor::IAssistProposal* CompletionAssistProcessor::perform(const TextEditor::IAssistInterface* interface)
{
    if (interface->reason() == TextEditor::IdleEditor)
        return 0;
return 0;
#if 0
    int pos = interface->position();
    QTextBlock block = interface->textDocument()->findBlock(pos);
    qDebug() << "perform!" << block.text();

    QList<TextEditor::BasicProposalItem*> proposals;
    auto a = new TextEditor::BasicProposalItem;
    a->setText("foooo");
    a->setDetail("Foo is just foo");
    proposals << a;

    TextEditor::BasicProposalItemListModel* model = new TextEditor::BasicProposalItemListModel(proposals);
    TextEditor::IAssistProposal* proposal = new TextEditor::GenericProposal(pos, model);

    return proposal;
#endif
}

}

#include "RubyCompletionAssist.moc"
