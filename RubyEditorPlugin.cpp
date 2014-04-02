#include "RubyEditorPlugin.h"

#include <coreplugin/icore.h>
#include <coreplugin/mimedatabase.h>
#include <QtPlugin>

RubyEditorPlugin::RubyEditorPlugin()
{
}

bool RubyEditorPlugin::initialize(const QStringList&, QString* errorString)
{
    if (!Core::MimeDatabase::addMimeTypes(QLatin1String(":rubyeditor/RubyEditor.mimetypes.xml"), errorString))
        return false;

    return true;
}

void RubyEditorPlugin::extensionsInitialized()
{
}

Q_EXPORT_PLUGIN(RubyEditorPlugin)

#include "RubyEditorPlugin.moc"
