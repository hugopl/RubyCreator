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

    virtual bool hasBuildTargets() const override { return false; }

    virtual QList<ProjectAction> supportedActions(Node* node) const override { return QList<ProjectAction>(); }

    virtual bool canAddSubProject(const QString&) const override { return false; }

    virtual bool addSubProjects(const QStringList&) override { return false; }

    virtual bool removeSubProjects(const QStringList&) override { return false; }

    virtual bool addFiles(const QStringList&, QStringList*) override { return false; }
    virtual bool removeFiles(const QStringList&, QStringList*) override { return false; }
    virtual bool deleteFiles(const QStringList&) override { return false; }
    virtual bool renameFile(const QString&, const QString&) override { return false; }

    virtual QList<ProjectExplorer::RunConfiguration*> runConfigurationsFor(Node*) override { return QList<ProjectExplorer::RunConfiguration*>(); }
};

}

#endif
