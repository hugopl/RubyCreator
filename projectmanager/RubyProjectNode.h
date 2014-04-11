#ifndef Ruby_ProjectNode_h
#define Ruby_ProjectNode_h

#include <projectexplorer/projectnodes.h>

namespace Ruby {

class ProjectNode : public ProjectExplorer::ProjectNode
{
public:
    ProjectNode(const QString& projectFilePath)
        : ProjectExplorer::ProjectNode(projectFilePath)
    {
    }

    QList<ProjectExplorer::ProjectAction> supportedActions(Node* node) const override { return QList<ProjectExplorer::ProjectAction>(); }

    bool canAddSubProject(const QString&) const override { return false; }

    bool addSubProjects(const QStringList&) override { return false; }

    bool removeSubProjects(const QStringList&) override { return false; }

    bool addFiles(const QStringList&, QStringList*) override { return false; }
    bool removeFiles(const QStringList&, QStringList*) override { return false; }
    bool deleteFiles(const QStringList&) override { return false; }
    bool renameFile(const QString&, const QString&) override { return false; }

    QList<ProjectExplorer::RunConfiguration*> runConfigurationsFor(Node*) override { return QList<ProjectExplorer::RunConfiguration*>(); }
};

}

#endif
