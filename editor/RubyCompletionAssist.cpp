#include "RubyCompletionAssist.h"
#include "../RubyConstants.h"
#include "RubyCodeModel.h"

#include <coreplugin/id.h>
#include <texteditor/codeassist/iassistproposalitem.h>
#include <texteditor/codeassist/genericproposal.h>
#include <texteditor/codeassist/igenericproposalmodel.h>
#include <texteditor/codeassist/iassistinterface.h>
#include <texteditor/codeassist/basicproposalitem.h>
#include <texteditor/codeassist/basicproposalitemlistmodel.h>

#include <QTextDocument>
#include <QTextBlock>
#include <QDebug>

namespace Ruby {

enum KindOfCompletion {
    MayBeAIdentifier,
    MayBeAMethod,
    MayBeConstant,
    MayBeASymbol
};

static KindOfCompletion kindOfCompletion(QTextDocument *document, int &startPosition)
{
    QChar ch;
    bool mayBeAConstant = false;
    do {
        ch = document->characterAt(--startPosition);

        if (ch == QLatin1Char('.')) {
            startPosition++;
            return MayBeAMethod;
        }
        if (ch == QLatin1Char(':'))
            return MayBeASymbol;
        if (ch.isUpper())
            mayBeAConstant = true;
    } while (ch.isLetterOrNumber() || ch == QLatin1Char('_'));

    startPosition++;
    return mayBeAConstant ? MayBeConstant : MayBeAIdentifier;
}

bool CompletionAssistProvider::supportsEditor(const Core::Id &editorId) const
{
    return editorId == Constants::EditorId;
}

TextEditor::IAssistProcessor *CompletionAssistProvider::createProcessor() const
{
    return new CompletionAssistProcessor;
}

bool CompletionAssistProvider::isActivationCharSequence(const QString &sequence) const
{
    return sequence.at(0) == QLatin1Char('.') || sequence.at(0) == QLatin1Char(':');
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
static void addProposalFromSet(QList<TextEditor::BasicProposalItem*> &proposals, const T &container, const QString &myTyping, const QIcon &icon, int order = 0)
{
    foreach (const typename T::value_type &item, container) {
        const QString &name = nameFor(item);
        if (myTyping == name)
            continue;

        auto proposal = new TextEditor::BasicProposalItem();
        proposal->setText(name);
        proposal->setIcon(icon);
        proposal->setOrder(order);
        proposals << proposal;
    }
}

CompletionAssistProcessor::CompletionAssistProcessor()
    : m_methodIcon(QLatin1String(":/codemodel/images/func.png"))
    , m_identifierIcon(QLatin1String(":/codemodel/images/var.png"))
    , m_constantIcon(QLatin1String(":/codemodel/images/macro.png"))
    , m_symbolIcon(m_identifierIcon)
{

}

TextEditor::IAssistProposal *CompletionAssistProcessor::perform(const TextEditor::IAssistInterface *interface)
{
    if (interface->reason() == TextEditor::IdleEditor)
        return 0;

    int startPosition = interface->position();
    KindOfCompletion kind = kindOfCompletion(interface->textDocument(), startPosition);
    CodeModel *cm = CodeModel::instance();

    QString myTyping = interface->textAt(startPosition, interface->position() - startPosition);
    const QString fileName = interface->fileName();

    QList<TextEditor::BasicProposalItem *> proposals;

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
    }

    if (proposals.empty()) {
        return 0;
    }

    TextEditor::BasicProposalItemListModel *model = new TextEditor::BasicProposalItemListModel();
    model->loadContent(proposals);
    TextEditor::IAssistProposal *proposal = new TextEditor::GenericProposal(startPosition, model);
    return proposal;
}

}
