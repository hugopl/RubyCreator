#ifndef Ruby_ProjectNode_h
#define Ruby_ProjectNode_h

#include <projectexplorer/projectnodes.h>

namespace Utils { class FileName; }

namespace Ruby {

class ProjectNode : public ProjectExplorer::ProjectNode
{
public:
    ProjectNode(const QString &projectFilePath)
        : ProjectExplorer::ProjectNode(projectFilePath)
    {
    }

    QList<ProjectExplorer::ProjectAction> supportedActions(Node *) const Q_DECL_OVERRIDE { return QList<ProjectExplorer::ProjectAction>(); }

    bool canAddSubProject(const QString &) const Q_DECL_OVERRIDE { return false; }

    bool addSubProjects(const QStringList &) Q_DECL_OVERRIDE { return false; }

    bool removeSubProjects(const QStringList &) Q_DECL_OVERRIDE { return false; }

    bool addFiles(const QStringList &, QStringList*) Q_DECL_OVERRIDE { return false; }
    bool removeFiles(const QStringList &, QStringList*) Q_DECL_OVERRIDE { return false; }
    bool deleteFiles(const QStringList &) Q_DECL_OVERRIDE { return false; }
    bool renameFile(const QString &, const QString &) Q_DECL_OVERRIDE { return false; }
    QList<ProjectExplorer::RunConfiguration*> runConfigurationsFor(Node *)  Q_DECL_OVERRIDE { return QList<ProjectExplorer::RunConfiguration*>(); }
};

}

#endif
