#include "RubyCurrentDocumentFilter.h"

#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/editormanager/ieditor.h>
#include <coreplugin/idocument.h>

namespace RubyEditor {

RubyCurrentDocumentFilter::RubyCurrentDocumentFilter()
    : m_enabled(false)
{
    setId("Ruby methods in current Document");
    setDisplayName(tr("Ruby Methods in Current Document"));
    setShortcutString(QString(QLatin1Char('.')));
    setIncludedByDefault(false);

    connect(Core::EditorManager::instance(), SIGNAL(currentEditorChanged(Core::IEditor*)),
            this, SLOT(onCurrentEditorChanged(Core::IEditor*)));

}

QList<Locator::FilterEntry> RubyCurrentDocumentFilter::matchesFor(QFutureInterface<Locator::FilterEntry>& future, const QString& entry)
{
    QList<Locator::FilterEntry> list;
    if (!m_enabled)
        return list;

    list << Locator::FilterEntry(this, "rubyeditor ftw!", 0);
    return list;
}

void RubyCurrentDocumentFilter::accept(Locator::FilterEntry selection) const
{
}

void RubyCurrentDocumentFilter::refresh(QFutureInterface<void>& future)
{
    puts(__func__);
}

void RubyCurrentDocumentFilter::onCurrentEditorChanged(Core::IEditor* editor)
{
    if (!editor) {
        m_enabled = false;
        return;
    }

    m_enabled = editor->document()->filePath().endsWith(".rb");

}

}

#include "RubyCurrentDocumentFilter.moc"
