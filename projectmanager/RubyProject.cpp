#include "RubyProject.h"

#include "editor/RubyCodeModel.h"
#include "RubyConstants.h"
#include "RubyDocument.h"
#include "RubyProjectManager.h"
#include "RubyProjectNode.h"

#include <QDebug>
#include <QFileInfo>
#include <QThread>

namespace Ruby {

Project::Project(ProjectManager* projectManager, const QString& fileName)
    : m_projectManager(projectManager)
    , m_document(new Document)

{
    m_projectDir = QFileInfo(fileName).dir();
    m_rootNode = new ProjectNode(m_projectDir.dirName());

    populateProject(m_projectDir, m_rootNode);
    CodeModel::instance()->updateModels(m_files);
}


QString Project::displayName() const
{
    return m_projectDir.dirName();
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

QStringList Project::files(FilesMode) const
{
    return m_files;
}

void Project::populateProject(const QDir& dir, ProjectExplorer::FolderNode* parent)
{
    using namespace ProjectExplorer;

    static QStringList nameFilter = QStringList() << "*.rb" << "*.erb" << "config.ru" << "Gemfile" << "Rakefile";
    QList<FolderNode*> folders;
    QList<FileNode*> files;

    for (const QFileInfo& info : dir.entryInfoList(nameFilter, QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::CaseSensitive)) {
        if (info.isDir()) {
            folders << new FolderNode(info.fileName());
            parent->addFolderNodes(folders);
            populateProject(QDir(info.filePath()), folders.first());
            folders.clear();
        } else {
            files << new FileNode(info.filePath(), SourceType, false);
            m_files << info.filePath();
        }
    }
    parent->addFileNodes(files);

    emit fileListChanged();
}

}
