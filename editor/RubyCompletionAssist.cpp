#include "RubyCompletionAssist.h"
#include "../RubyConstants.h"
#include "RubyCodeModel.h"
#include "RubyScanner.h"

#include <texteditor/codeassist/assistproposalitem.h>
#include <texteditor/codeassist/genericproposal.h>
#include <texteditor/codeassist/genericproposalmodel.h>
#include <texteditor/codeassist/assistinterface.h>

#include <QTextDocument>
#include <QTextBlock>
#include <QDebug>

namespace Ruby {

enum KindOfCompletion {
    MayBeAIdentifier,
    MayBeAMethod,
    MayBeConstant,
    MayBeASymbol,
    MaybeNothing
};

static KindOfCompletion kindOfCompletion(QTextDocument *document, int &startPosition)
{
    QChar ch;
    bool mayBeAConstant = false;
    do {
        ch = document->characterAt(--startPosition);

        if (ch == '.') {
            startPosition++;
            return MayBeAMethod;
        }
        if (ch == ':') {
            QChar lastChar = document->characterAt(startPosition - 1);
            if (lastChar.isLetterOrNumber() || lastChar == ':')
                return MaybeNothing;
            return MayBeASymbol;
        }
        if (ch.isUpper())
            mayBeAConstant = true;
    } while (ch.isLetterOrNumber() || ch == '_');

    startPosition++;
    return mayBeAConstant ? MayBeConstant : MayBeAIdentifier;
}

TextEditor::IAssistProcessor *CompletionAssistProvider::createProcessor() const
{
    return new CompletionAssistProcessor;
}

bool CompletionAssistProvider::isActivationCharSequence(const QString &sequence) const
{
    return sequence.at(0) == '.' || sequence.at(0) == ':';
}

static const QString &nameFor(const QString &s)
{
    return s;
}

static const QString &nameFor(const Symbol &s)
{
    return s.name;
}

template<typename T>
static void addProposalFromSet(QList<TextEditor::AssistProposalItemInterface*> &proposals,
                               const T &container, const QString &myTyping,
                               const QIcon &icon, int order = 0)
{
    for (const auto &item : container) {
        const QString &name = nameFor(item);
        if (myTyping == name)
            continue;

        auto proposal = new TextEditor::AssistProposalItem;

        int indexOfParenthesis = name.indexOf('(');
        if (indexOfParenthesis != -1) {
            proposal->setText(name.mid(0, indexOfParenthesis));
            proposal->setDetail(name);
        } else {
            proposal->setText(name);
        }

        proposal->setIcon(icon);
        proposal->setOrder(order);
        proposals << proposal;
    }
}

CompletionAssistProcessor::CompletionAssistProcessor()
    : m_methodIcon(":/codemodel/images/func.png")
    , m_identifierIcon(":/codemodel/images/var.png")
    , m_constantIcon(":/codemodel/images/macro.png")
    , m_symbolIcon(m_identifierIcon)
    , m_snippetCollector(Constants::SnippetGroupId,
                         QIcon(":/texteditor/images/snippet.png"))
{

}

TextEditor::IAssistProposal *CompletionAssistProcessor::perform(const TextEditor::AssistInterface *interface)
{
    if (interface->reason() == TextEditor::IdleEditor)
        return 0;

    int startPosition = interface->position();

    // FIXME: We should check the block status in case of multi-line tokens
    QTextBlock block = interface->textDocument()->findBlock(startPosition);
    int linePosition = startPosition - block.position();
    const QString line = interface->textDocument()->findBlock(startPosition).text();

    switch(Scanner::tokenAt(&line, linePosition).kind) {
    case Token::Comment:
    case Token::String:
    case Token::Backtick:
    case Token::Regexp:
        return 0;
    default:
        break;
    }

    KindOfCompletion kind = kindOfCompletion(interface->textDocument(), startPosition);
    CodeModel *cm = CodeModel::instance();

    QString myTyping = interface->textAt(startPosition, interface->position() - startPosition);
    const QString fileName = interface->filePath().fileName();

    QList<TextEditor::AssistProposalItemInterface *> proposals;

    switch (kind) {
    case MayBeAMethod:
    case MayBeAIdentifier:
        addProposalFromSet(proposals, cm->methodsIn(fileName), myTyping, m_methodIcon, 1);
        addProposalFromSet(proposals, cm->identifiersIn(fileName), myTyping, m_identifierIcon);
        break;
    case MayBeConstant:
        addProposalFromSet(proposals, cm->constantsIn(fileName), myTyping, m_constantIcon);
        break;
    case MayBeASymbol:
        addProposalFromSet(proposals, cm->symbolsIn(fileName), myTyping, m_symbolIcon);
        break;
    default:
        break;
    }
    proposals += m_snippetCollector.collect();

    if (proposals.empty()) {
        return 0;
    }
    TextEditor::GenericProposalModelPtr model(new TextEditor::GenericProposalModel);
    model->loadContent(proposals);
    TextEditor::IAssistProposal *proposal = new TextEditor::GenericProposal(startPosition, model);
    return proposal;
}

}
