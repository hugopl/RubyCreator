#include "RubyAmbiguousMethodAssistProvider.h"
#include "../RubyConstants.h"

#include <texteditor/codeassist/assistinterface.h>
#include <texteditor/codeassist/assistproposalitem.h>
#include <texteditor/codeassist/genericproposal.h>
#include <texteditor/codeassist/genericproposalmodel.h>
#include <texteditor/codeassist/genericproposalwidget.h>
#include <texteditor/codeassist/iassistprocessor.h>
#include <texteditor/codeassist/iassistproposal.h>
#include <texteditor/texteditorconstants.h>

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/editormanager/editormanager.h>

#include <QDebug>
#include <QEvent>
#include <QKeyEvent>

namespace Ruby {

class AmbigousMethodProposalItem : public TextEditor::AssistProposalItem
{
public:
    AmbigousMethodProposalItem(const Symbol &symbol, bool inNextSplit)
        : m_symbol(symbol)
        , m_inNextSplit(inNextSplit)
    {
        QString text = symbol.context;
        if (!text.isEmpty())
            text += "::";
        text += symbol.name + QString::fromLatin1(" [line %1]").arg(symbol.line);
        setText(text);
        setDetail(*symbol.file);
    }

    void apply(TextEditor::TextDocumentManipulatorInterface&, int) const override
    {
        Core::EditorManager::OpenEditorFlags flags = Core::EditorManager::NoFlags;
        if (m_inNextSplit)
            flags |= Core::EditorManager::OpenInOtherSplit;
        Core::EditorManager::openEditorAt(*m_symbol.file,
                                          m_symbol.line,
                                          m_symbol.column,
                                          Utils::Id(Constants::EditorId),
                                          flags);
    }
private:
    Symbol m_symbol;
    bool m_inNextSplit;
};

class AmbigousMethodAssistProcessor : public TextEditor::IAssistProcessor {
public:
    AmbigousMethodAssistProcessor(const QList<Symbol> &symbols, int cursorPosition, int inNextSplit)
        : m_symbols(symbols)
        , m_cursorPosition(cursorPosition)
        , m_inNextSplit(inNextSplit)
    {
    }

    TextEditor::IAssistProposal *perform(const TextEditor::AssistInterface *interface) override
    {
        delete interface;

        QList<TextEditor::AssistProposalItemInterface *> proposals;
        for (const Symbol &symbol : m_symbols)
            proposals << new AmbigousMethodProposalItem(symbol, m_inNextSplit);
        auto proposal = new TextEditor::GenericProposal(m_cursorPosition, proposals);
        proposal->setFragile(true);
        return proposal;
    }

    const QList<Symbol> m_symbols;
    int m_cursorPosition;
    bool m_inNextSplit;
};

TextEditor::IAssistProvider::RunType AmbigousMethodAssistProvider::runType() const
{
    return TextEditor::IAssistProvider::AsynchronousWithThread;
}

TextEditor::IAssistProcessor *AmbigousMethodAssistProvider::createProcessor() const
{
    return new AmbigousMethodAssistProcessor(m_symbols, m_cursorPosition, m_inNexSplit);
}

}
