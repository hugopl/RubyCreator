#include "RubyEditorPlugin.h"

#include "RubyCurrentDocumentFilter.h"
#include "RubyEditorFactory.h"
#include <coreplugin/icore.h>
#include <coreplugin/mimedatabase.h>
#include <QtPlugin>

namespace RubyEditor {

RubyEditorPlugin::RubyEditorPlugin()
{
}

RubyEditorPlugin::~RubyEditorPlugin()
{
    removeObject(m_factory);
}

bool RubyEditorPlugin::initialize(const QStringList&, QString* errorString)
{
    if (!Core::MimeDatabase::addMimeTypes(QLatin1String(":rubyeditor/RubyEditor.mimetypes.xml"), errorString))
        return false;

    m_factory = new RubyEditorFactory(this);
    addObject(m_factory);
    addAutoReleasedObject(new RubyCurrentDocumentFilter);

    return true;
}

void RubyEditorPlugin::extensionsInitialized()
{
}

} // namespace RubyEditor

Q_EXPORT_PLUGIN(RubyEditor::RubyEditorPlugin)

#include "RubyEditorPlugin.moc"
