#ifndef Ruby_ProjectNode_h
#define Ruby_ProjectNode_h

#include <projectexplorer/projectnodes.h>

namespace Utils { class FileName; }

namespace Ruby {

class ProjectNode : public ProjectExplorer::ProjectNode
{
public:
    ProjectNode(const Utils::FileName &projectFilePath)
        : ProjectExplorer::ProjectNode(projectFilePath)
    {
    }

    QList<ProjectExplorer::ProjectAction> supportedActions(Node *) const override;

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
