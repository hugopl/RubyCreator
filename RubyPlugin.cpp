#include "RubyPlugin.h"

#include "editor/RubyCodeModel.h"
#include "editor/RubyEditorFactory.h"
#include "editor/RubySymbolFilter.h"
#include "projectmanager/RubyProjectManager.h"
#include "projectmanager/RubyProjectWizard.h"

#include <coreplugin/icore.h>
#include <coreplugin/mimedatabase.h>
#include <QtPlugin>

namespace Ruby {

Plugin::Plugin()
{
}

Plugin::~Plugin()
{
    removeObject(m_factory);
}

bool Plugin::initialize(const QStringList&, QString* errorString)
{
    if (!Core::MimeDatabase::addMimeTypes(QLatin1String(":rubysupport/Ruby.mimetypes.xml"), errorString))
        return false;

    m_factory = new EditorFactory(this);
    addObject(m_factory);
    addAutoReleasedObject(new SymbolFilter([](const QString& file) {
        return CodeModel::instance()->methodsIn(file);
    }, "Ruby Methods in Current Document", '.'));
    addAutoReleasedObject(new SymbolFilter([](const QString&) {
        return CodeModel::instance()->allMethods();
    }, "Ruby methods", 'm'));
    addAutoReleasedObject(new ProjectManager);

    addAutoReleasedObject(new ProjectWizard);
    return true;
}

void Plugin::extensionsInitialized()
{
}

}

Q_EXPORT_PLUGIN(Ruby::Plugin)

#include "RubyPlugin.moc"
