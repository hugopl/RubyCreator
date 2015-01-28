#include "RubyAmbiguousMethodAssistProvider.h"
#include "../RubyConstants.h"

#include <QDebug>
#include <QEvent>
#include <QKeyEvent>

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

namespace Ruby {

// Override just to set fragile = true.
class AmbigousMethodProposal : public TextEditor::GenericProposal
{
public:
    AmbigousMethodProposal(int cursorPos, const QList<TextEditor::AssistProposalItem *> &items)
        : GenericProposal(cursorPos, items)
    {}

    bool isFragile() const Q_DECL_OVERRIDE { return true; }
};

class AmbigousMethodProposalItem : public TextEditor::AssistProposalItem
{
public:
    AmbigousMethodProposalItem(const Symbol &symbol, bool inNextSplit)
        : m_symbol(symbol)
        , m_inNextSplit(inNextSplit)
    {
        QString text = symbol.context;
        if (!text.isEmpty())
            text += QStringLiteral("::");
        text += symbol.name + QString::fromLatin1(" [line %1]").arg(symbol.line);
        setText(text);
        setDetail(*symbol.file);
    }

    void apply(TextEditor::TextEditorWidget*, int) const Q_DECL_OVERRIDE
    {
        Core::EditorManager::OpenEditorFlags flags = Core::EditorManager::NoFlags;
        if (m_inNextSplit)
            flags |= Core::EditorManager::OpenInOtherSplit;
        Core::EditorManager::openEditorAt(*m_symbol.file,
                                          m_symbol.line,
                                          m_symbol.column,
                                          Core::Id(Constants::EditorId),
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

    TextEditor::IAssistProposal *perform(const TextEditor::AssistInterface *interface) Q_DECL_OVERRIDE
    {
        delete interface;

        QList<TextEditor::AssistProposalItem*> proposals;
        foreach (const Symbol &symbol, m_symbols)
            proposals << new AmbigousMethodProposalItem(symbol, m_inNextSplit);
        return new AmbigousMethodProposal(m_cursorPosition, proposals);
    }

    const QList<Symbol> m_symbols;
    int m_cursorPosition;
    bool m_inNextSplit;
};

bool AmbigousMethodAssistProvider::isAsynchronous() const
{
    return true;
}

bool AmbigousMethodAssistProvider::supportsEditor(Core::Id editorId) const
{
    return editorId == Constants::EditorId;
}

TextEditor::IAssistProcessor *AmbigousMethodAssistProvider::createProcessor() const
{
    return new AmbigousMethodAssistProcessor(m_symbols, m_cursorPosition, m_inNexSplit);
}

}
