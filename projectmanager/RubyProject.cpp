#include "RubyProject.h"

#include "RubyConstants.h"
#include "RubyDocument.h"
#include "RubyProjectManager.h"
#include "RubyProjectNode.h"

#include <QFileInfo>

namespace Ruby {

Project::Project(ProjectManager* projectManager, const QString& fileName)
    : m_projectManager(projectManager)
    , m_document(new Document)

{
    m_projectName = QFileInfo(fileName).dir().dirName();
    m_rootNode = new ProjectNode(m_projectName);

    // just to have something to open on editor :-)
    m_rootNode->addFileNodes(QList<ProjectExplorer::FileNode*>() << new ProjectExplorer::FileNode(fileName, ProjectExplorer::SourceType, false), m_rootNode);
}


QString Project::displayName() const
{
    return m_projectName;
}

Core::Id Project::id() const
{
    return Constants::ProjectId;
}

Core::IDocument* Project::document() const
{
    return m_document;
}

ProjectExplorer::IProjectManager* Project::projectManager() const
{
    return m_projectManager;
}

ProjectExplorer::ProjectNode* Project::rootProjectNode() const
{
    return m_rootNode;
}

QStringList Project::files(FilesMode fileMode) const
{
    return QStringList();
}

}
