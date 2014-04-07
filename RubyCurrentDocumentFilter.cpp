#include "RubyCurrentDocumentFilter.h"

#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/editormanager/ieditor.h>
#include <coreplugin/idocument.h>

#include "RubyCodeModel.h"

namespace RubyEditor {

RubyCurrentDocumentFilter::RubyCurrentDocumentFilter()
    : m_icon(":/codemodel/images/func.png")
    , m_enabled(false)
{
    setId("Ruby methods in current Document");
    setDisplayName(tr("Ruby Methods in Current Document"));
    setShortcutString(QString(QLatin1Char('.')));
    setIncludedByDefault(false);

    connect(Core::EditorManager::instance(), SIGNAL(currentEditorChanged(Core::IEditor*)),
            this, SLOT(onCurrentEditorChanged(Core::IEditor*)));

}

QList<Locator::FilterEntry> RubyCurrentDocumentFilter::matchesFor(QFutureInterface<Locator::FilterEntry>&, const QString& entry)
{
    QList<Locator::FilterEntry> list;
    if (!m_enabled || m_fileName.isEmpty())
        return list;

    QStringMatcher matcher(entry, Qt::CaseInsensitive);
    RubyCodeModel* codeModel = RubyCodeModel::instance();

    for (RubySymbol symbol : codeModel->methodsIn(m_fileName)) {
        if (matcher.indexIn(symbol.name) != -1)
            list << Locator::FilterEntry(this, symbol.name, qVariantFromValue(symbol), m_icon);
    }
    return list;
}

void RubyCurrentDocumentFilter::accept(Locator::FilterEntry selection) const
{
    RubySymbol symbol = selection.internalData.value<RubySymbol>();
    Core::EditorManager::openEditorAt(m_fileName, symbol.line, symbol.column);
}

void RubyCurrentDocumentFilter::refresh(QFutureInterface<void>&)
{
}

void RubyCurrentDocumentFilter::onCurrentEditorChanged(Core::IEditor* editor)
{
    if (!editor) {
        m_enabled = false;
        return;
    }

    m_fileName = editor->document()->filePath();
    m_enabled = m_fileName.endsWith(".rb");

}

}

#include "RubyCurrentDocumentFilter.moc"
