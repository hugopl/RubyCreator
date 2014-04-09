#include "RubyProjectManager.h"

#include "RubyConstants.h"
#include "RubyProject.h"

namespace Ruby {

ProjectManager::ProjectManager()
{
}

QString ProjectManager::mimeType() const
{
    return Constants::MimeType;
}

ProjectExplorer::Project* ProjectManager::openProject(const QString& fileName, QString* errorString)
{
    if (!QFileInfo(fileName).isFile()) {
        *errorString = tr("Failed opening project '%1': Project is not a file").arg(fileName);
        return nullptr;
    }

    return new Project(this, fileName);
}

}

#include "RubyProjectManager.moc"
