#include "RubySymbolFilter.h"

#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/editormanager/ieditor.h>
#include <coreplugin/idocument.h>

namespace Ruby {

SymbolFilter::SymbolFilter(SymbolProvider provider, const char *description, QChar shortcut)
    : m_icon(":/codemodel/images/func.png")
    , m_symbolProvider(provider)
{
    setId(description);
    setDisplayName(tr(description));
    setShortcutString(shortcut);
    setIncludedByDefault(false);

    connect(Core::EditorManager::instance(), &Core::EditorManager::currentEditorChanged,
            this, &SymbolFilter::onCurrentEditorChanged);

}

QList<Core::LocatorFilterEntry> SymbolFilter::matchesFor(QFutureInterface<Core::LocatorFilterEntry> &, const QString &entry)
{
    QList<Core::LocatorFilterEntry> list;
    QStringMatcher matcher(entry, Qt::CaseInsensitive);

    const auto symbols = m_symbolProvider(m_fileName.toString());
    for (const Symbol &symbol : symbols) {
        if (matcher.indexIn(symbol.name) != -1) {
            list << Core::LocatorFilterEntry(this, symbol.name, qVariantFromValue(symbol), m_icon);
            list.last().extraInfo = symbol.context;
        }
    }
    return list;
}

void SymbolFilter::accept(Core::LocatorFilterEntry selection,
                          QString *newText, int *selectionStart, int *selectionLength) const
{
    Q_UNUSED(newText)
    Q_UNUSED(selectionStart)
    Q_UNUSED(selectionLength)
    Symbol symbol = selection.internalData.value<Symbol>();
    Core::EditorManager::openEditorAt(*symbol.file, symbol.line, symbol.column);
}

void SymbolFilter::refresh(QFutureInterface<void> &)
{
}

void SymbolFilter::onCurrentEditorChanged(Core::IEditor *editor)
{
    if (!editor) {
        setEnabled(false);
        m_fileName.clear();
        return;
    }

    m_fileName = editor->document()->filePath();
    setEnabled(m_fileName.endsWith(".rb"));
}

}
