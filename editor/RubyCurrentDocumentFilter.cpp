#include "RubyCurrentDocumentFilter.h"

#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/editormanager/ieditor.h>
#include <coreplugin/idocument.h>

#include "RubyCodeModel.h"

namespace Ruby {

CurrentDocumentFilter::CurrentDocumentFilter()
    : m_icon(":/codemodel/images/func.png")
{
    setId("Ruby methods in current Document");
    setDisplayName(tr("Ruby Methods in Current Document"));
    setShortcutString(QString(QLatin1Char('.')));
    setIncludedByDefault(false);

    connect(Core::EditorManager::instance(), SIGNAL(currentEditorChanged(Core::IEditor*)),
            this, SLOT(onCurrentEditorChanged(Core::IEditor*)));

}

QList<Core::LocatorFilterEntry> CurrentDocumentFilter::matchesFor(QFutureInterface<Core::LocatorFilterEntry>&, const QString& entry)
{
    QList<Core::LocatorFilterEntry> list;
    QStringMatcher matcher(entry, Qt::CaseInsensitive);
    CodeModel* codeModel = CodeModel::instance();

    for (Symbol symbol : codeModel->symbolsIn(m_fileName)) {
        if (matcher.indexIn(symbol.name) != -1) {
            list << Core::LocatorFilterEntry(this, symbol.name, qVariantFromValue(symbol), m_icon);
            list.last().extraInfo = symbol.context;
        }
    }
    return list;
}

void CurrentDocumentFilter::accept(Core::LocatorFilterEntry selection) const
{
    Symbol symbol = selection.internalData.value<Symbol>();
    Core::EditorManager::openEditorAt(m_fileName, symbol.line, symbol.column);
}

void CurrentDocumentFilter::refresh(QFutureInterface<void>&)
{
}

void CurrentDocumentFilter::onCurrentEditorChanged(Core::IEditor* editor)
{
    if (!editor) {
        setEnabled(false);
        return;
    }

    m_fileName = editor->document()->filePath();
    setEnabled(m_fileName.endsWith(".rb"));
}

}

#include "RubyCurrentDocumentFilter.moc"
