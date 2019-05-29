#ifndef Ruby_ProjectNode_h
#define Ruby_ProjectNode_h

#include <projectexplorer/projectnodes.h>

namespace Ruby {

class ProjectNode : public ProjectExplorer::ProjectNode
{
public:
    ProjectNode(const Utils::FilePath &projectFilePath)
        : ProjectExplorer::ProjectNode(projectFilePath)
    {
    }

    bool supportsAction(ProjectExplorer::ProjectAction action, const Node *node) const override;

    bool canAddSubProject(const QString &) const override { return false; }
    bool canRenameFile(const QString &, const QString &) override { return true; }

    bool addSubProject(const QString &) override { return false; }
    bool removeSubProject(const QString &) override { return false; }

    bool addFiles(const QStringList &, QStringList*) override { return true; }
    bool removeFiles(const QStringList &, QStringList*) override { return true; }
    bool deleteFiles(const QStringList &) override { return true; }
    bool renameFile(const QString &, const QString &) override { return true; }
};

}

#endif
