#ifndef Ruby_Project
#define Ruby_Project

#include <projectexplorer/project.h>
#include <projectexplorer/projectnodes.h>

namespace Ruby {

class Document;
class ProjectManager;
class ProjectNode;

class Project : public ProjectExplorer::Project
{
public:
    Project(ProjectManager* projectManager, const QString& fileName);
    QString displayName() const override;
    Core::IDocument* document() const override;
    ProjectExplorer::IProjectManager* projectManager() const override;
    ProjectExplorer::ProjectNode* rootProjectNode() const override;

    QStringList files(FilesMode) const override;
private:
    ProjectManager* m_projectManager;
    Document* m_document;
    ProjectNode* m_rootNode;

    QDir m_projectDir;
    QStringList m_files;

    void populateProject(const QDir& dir, ProjectExplorer::FolderNode* parent);
};

}

#endif
