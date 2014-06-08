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

const int MIN_TIME_BETWEEN_PROJECT_SCANS = 4500;

Project::Project(ProjectManager* projectManager, const QString& fileName)
    : m_projectManager(projectManager)
    , m_document(new Document)
    , m_nameFilter({"*.rb", "*.erb", "*.rake", "*.yml", "*.haml", "config.ru", "Gemfile", "Rakefile"})
{
    m_document->setFilePath(fileName);
    m_projectDir = QFileInfo(fileName).dir();
    m_rootNode = new ProjectNode(m_projectDir.dirName());

    m_projectScanTimer.setSingleShot(true);
    connect(&m_projectScanTimer, &QTimer::timeout, this, &Project::populateProject);

    populateProject();
    CodeModel::instance()->addFiles(files(AllFiles));

    connect(&m_fsWatcher, &QFileSystemWatcher::directoryChanged, this, &Project::scheduleProjectScan);
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
    return QStringList(m_files.toList());
}

void Project::scheduleProjectScan()
{
    auto elapsedTime = m_lastProjectScan.elapsed();
    if (elapsedTime < MIN_TIME_BETWEEN_PROJECT_SCANS) {
        if (!m_projectScanTimer.isActive()) {
            m_projectScanTimer.setInterval(MIN_TIME_BETWEEN_PROJECT_SCANS - elapsedTime);
            m_projectScanTimer.start();
        }
    } else {
        populateProject();
    }
}

void Project::populateProject()
{
    m_lastProjectScan.start();
    QSet<QString> oldFiles(m_files);
    m_files.clear();
    recursiveScanDirectory(m_projectDir, m_files);

    auto removedFiles = oldFiles - m_files;
    auto addedFiles = m_files - oldFiles;

    removeNodes(removedFiles);
    addNodes(addedFiles);

    for (const QString& file : removedFiles)
        CodeModel::instance()->removeSymbolsFrom(file);
    for (const QString& file : addedFiles)
        CodeModel::instance()->addFile(file);

    if (removedFiles.size() || addedFiles.size())
        emit fileListChanged();
}

void Project::recursiveScanDirectory(const QDir& dir, QSet<QString>& container)
{
    for (const QFileInfo& info : dir.entryInfoList(m_nameFilter, QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::CaseSensitive)) {
        if (info.isDir())
            recursiveScanDirectory(QDir(info.filePath()), container);
        else
            container << info.filePath();
    }
    m_fsWatcher.addPath(dir.absolutePath());
}

void Project::addNodes(const QSet<QString>& nodes)
{
    using namespace ProjectExplorer;

    const QLatin1Char sep('/');
    QStringList path;
    for (const QString& node : nodes) {
        path = m_projectDir.relativeFilePath(node).split(sep);
        path.pop_back();
        FolderNode* folder = findFolderFor(path);
        folder->addFileNodes(QList<FileNode*>() << new FileNode(node, SourceType, false));
    }
}

void Project::removeNodes(const QSet<QString>& nodes)
{
    using namespace ProjectExplorer;

    const QLatin1Char sep('/');
    QStringList path;

    for (const QString& node : nodes) {
        path = m_projectDir.relativeFilePath(node).split(sep);
        path.pop_back();
        FolderNode* folder = findFolderFor(path);

        for (FileNode* file : folder->fileNodes()) {
            if (file->path() == node) {
                folder->removeFileNodes(QList<FileNode*>() << file);
                break;
            }
        }
    }
}

ProjectExplorer::FolderNode* Project::findFolderFor(const QStringList& path)
{
    using namespace ProjectExplorer;
    FolderNode* folder = m_rootNode;

    for (const QString& part : path) {
        bool folderFound = false;
        for (FolderNode* subFolder : folder->subFolderNodes()) {
            if (subFolder->displayName() == part) {
                folder = subFolder;
                folderFound = true;
                break;
            }
        }
        if (!folderFound) {
            FolderNode* newFolder = new FolderNode(part);
            folder->addFolderNodes(QList<FolderNode*>() << newFolder);
            folder = newFolder;
        }
    }
    return folder;
}

}

#include "RubyProject.moc"
