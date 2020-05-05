#ifndef Ruby_BuildSystem_h
#define Ruby_BuildSystem_h

#include <projectexplorer/buildsystem.h>

namespace Ruby {

class BuildSystem : public ProjectExplorer::BuildSystem
{
public:
    explicit BuildSystem(ProjectExplorer::Target *target)
        : ProjectExplorer::BuildSystem(target)
    {
    }

    bool supportsAction(ProjectExplorer::Node *context,
                        ProjectExplorer::ProjectAction action,
                        const ProjectExplorer::Node *node) const override;

    bool canRenameFile(ProjectExplorer::Node *, const QString &, const QString &) override { return true; }

    ProjectExplorer::RemovedFilesFromProject removeFiles(ProjectExplorer::Node *,
                                                         const QStringList &,
                                                         QStringList*) override
    { return ProjectExplorer::RemovedFilesFromProject::Ok; }
    bool deleteFiles(ProjectExplorer::Node *, const QStringList &) override { return true; }
    bool renameFile(ProjectExplorer::Node *, const QString &, const QString &) override { return true; }
};

}

#endif
