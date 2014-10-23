#ifndef Ruby_ProjectManager_h
#define Ruby_ProjectManager_h

#include <projectexplorer/iprojectmanager.h>

namespace Ruby {

class ProjectManager : public ProjectExplorer::IProjectManager
{
    Q_OBJECT

public:
    ProjectManager();

    virtual QString mimeType() const override;
    virtual ProjectExplorer::Project *openProject(const QString &fileName, QString *errorString) override;
};

}

#endif
