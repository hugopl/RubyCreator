#ifndef Ruby_ProjectManager_h
#define Ruby_ProjectManager_h

#include <projectexplorer/iprojectmanager.h>

namespace Ruby {

class ProjectManager : public ProjectExplorer::IProjectManager
{
    Q_OBJECT

public:
    ProjectManager();

    virtual QString mimeType() const Q_DECL_OVERRIDE;
    virtual ProjectExplorer::Project *openProject(const QString &fileName, QString *errorString) Q_DECL_OVERRIDE;
};

}

#endif
