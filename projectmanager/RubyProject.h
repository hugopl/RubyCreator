#ifndef Ruby_Project
#define Ruby_Project

#include <projectexplorer/project.h>

namespace Ruby {

class Document;
class ProjectManager;
class ProjectNode;

class Project : public ProjectExplorer::Project
{
public:
    Project(ProjectManager* projectManager, const QString& fileName);
    virtual QString displayName() const override;
    virtual Core::Id id() const override;
    virtual Core::IDocument* document() const override;
    virtual ProjectExplorer::IProjectManager* projectManager() const override;
    virtual ProjectExplorer::ProjectNode* rootProjectNode() const override;

    virtual QStringList files(FilesMode fileMode) const override;
private:
    ProjectManager* m_projectManager;
    QString m_projectName;
    Document* m_document;
    ProjectNode* m_rootNode;
};

}

#endif
