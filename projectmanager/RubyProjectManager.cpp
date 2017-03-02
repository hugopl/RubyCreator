#include "RubyProjectManager.h"

#include "../RubyConstants.h"
#include "RubyProject.h"

namespace Ruby {

ProjectManager::ProjectManager()
{
}

QString ProjectManager::mimeType() const
{
    return QLatin1String(Constants::ProjectMimeType);
}

ProjectExplorer::Project *ProjectManager::openProject(const QString &fileName)
{
    return new Project(fileName);
}

}
