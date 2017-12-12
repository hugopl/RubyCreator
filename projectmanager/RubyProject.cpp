#include "RubyProject.h"

#include "../editor/RubyCodeModel.h"
#include "../RubyConstants.h"
#include "RubyProjectNode.h"

#include <QDebug>
#include <QtConcurrent>
#include <QFileInfo>
#include <QRegularExpression>
#include <QThread>

#include <texteditor/textdocument.h>
#include <coreplugin/progressmanager/progressmanager.h>

namespace Ruby {

const int MIN_TIME_BETWEEN_PROJECT_SCANS = 4500;

Project::Project(const Utils::FileName &fileName) :
    ProjectExplorer::Project(Constants::MimeType, fileName, [this] { scheduleProjectScan(); })
    , m_populatingProject(false)
{
    m_projectDir = fileName.toFileInfo().dir();
    m_rootNode = new ProjectNode(Utils::FileName::fromString(m_projectDir.dirName()));

    m_projectScanTimer.setSingleShot(true);
    connect(&m_projectScanTimer, &QTimer::timeout, this, &Project::populateProject);

    QFuture<void> future = QtConcurrent::run(this, &Project::populateProject);
    Core::ProgressManager::instance()->addTask(future, tr("Parsing Ruby Files"), Constants::RubyProjectTask);

    connect(&m_fsWatcher, &QFileSystemWatcher::directoryChanged, this, &Project::scheduleProjectScan);

    setDisplayName(m_projectDir.dirName());
}

ProjectExplorer::ProjectNode *Project::rootProjectNode() const
{
    return m_rootNode;
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
    if (m_populatingProject)
        return;
    m_populatingProject = true;

    m_lastProjectScan.start();
    QSet<QString> oldFiles(m_files);
    m_files.clear();
    recursiveScanDirectory(m_projectDir, m_files);

    const auto removedFiles = oldFiles - m_files;
    const auto addedFiles = m_files - oldFiles;

    removeNodes(removedFiles);
    addNodes(addedFiles);

    for (const QString &file : removedFiles)
        CodeModel::instance()->removeSymbolsFrom(file);
    for (const QString &file : addedFiles)
        CodeModel::instance()->addFile(file);
    m_populatingProject = false;

    if (!addedFiles.isEmpty() || !removedFiles.isEmpty())
        emit fileListChanged();
}

void Project::recursiveScanDirectory(const QDir &dir, QSet<QString> &container)
{
    QRegularExpression projectFilePattern(".*\\.rubyproject(?:\\.user)?$");
    const auto files = dir.entryInfoList(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot
                                         | QDir::NoSymLinks | QDir::CaseSensitive);
    for (const QFileInfo &info : files) {
        if (info.isDir())
            recursiveScanDirectory(QDir(info.filePath()), container);
        else if (!projectFilePattern.match(info.fileName()).hasMatch())
            container << info.filePath();
    }
    m_fsWatcher.addPath(dir.absolutePath());
}

void Project::addNodes(const QSet<QString> &nodes)
{
    using namespace ProjectExplorer;

    const QChar sep = '/';
    QStringList path;
    for (const QString &node : nodes) {
        path = m_projectDir.relativeFilePath(node).split(sep);
        path.pop_back();
        FolderNode *folder = findFolderFor(path);
        folder->addNode(new FileNode(Utils::FileName::fromString(node), FileType::Source, false));
    }
}

void Project::tryRemoveEmptyFolder(ProjectExplorer::FolderNode *folder)
{
    using namespace ProjectExplorer;

    if (folder == m_rootNode)
        return;

    FolderNode *parent = folder->parentFolderNode();
    if (parent && folder->fileNodes().empty() && folder->folderNodes().empty()) {
        parent->removeNode(folder);
        tryRemoveEmptyFolder(parent);
    }
}

void Project::removeNodes(const QSet<QString> &nodes)
{
    using namespace ProjectExplorer;


    for (const QString &node : nodes) {
        QStringList path = m_projectDir.relativeFilePath(node).split('/');
        path.pop_back();
        FolderNode *folder = findFolderFor(path);

        const auto files = folder->fileNodes();
        for (FileNode *file : files) {
            if (file->filePath().toString() == node) {
                folder->removeNode(file);
                tryRemoveEmptyFolder(folder);
                break;
            }
        }
    }
}

ProjectExplorer::FolderNode *Project::findFolderFor(const QStringList &path)
{
    using namespace ProjectExplorer;
    FolderNode *folder = m_rootNode;

    for (const QString &part : path) {
        bool folderFound = false;
        const auto subFolders = folder->folderNodes();
        for (FolderNode *subFolder : subFolders) {
            if (subFolder->displayName() == part) {
                folder = subFolder;
                folderFound = true;
                break;
            }
        }
        if (!folderFound) {
            FolderNode *newFolder = new FolderNode(Utils::FileName::fromString(part));
            folder->addNode(newFolder);
            folder = newFolder;
        }
    }
    return folder;
}

}
