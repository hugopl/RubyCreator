#include "RubyProject.h"

#include "../editor/RubyCodeModel.h"
#include "../RubyConstants.h"
#include "RubyProjectFile.h"
#include "RubyProjectNode.h"

#include <QDebug>
#include <QFileInfo>
#include <QThread>

#include <texteditor/textdocument.h>

namespace Ruby {

const int MIN_TIME_BETWEEN_PROJECT_SCANS = 4500;

Project::Project(const Utils::FileName &fileName)
{
    m_projectDir = fileName.toFileInfo().dir();
    m_rootNode = new ProjectNode(Utils::FileName::fromString(m_projectDir.dirName()));
    setDocument(new ProjectFile(fileName));

    m_projectScanTimer.setSingleShot(true);
    connect(&m_projectScanTimer, &QTimer::timeout, this, &Project::populateProject);

    populateProject();
    CodeModel::instance()->addFiles(m_files.toList());

    connect(&m_fsWatcher, &QFileSystemWatcher::directoryChanged, this, &Project::scheduleProjectScan);
}


QString Project::displayName() const
{
    return m_projectDir.dirName();
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
    m_lastProjectScan.start();
    QSet<QString> oldFiles(m_files);
    m_files.clear();
    recursiveScanDirectory(m_projectDir, m_files);

    auto removedFiles = oldFiles - m_files;
    auto addedFiles = m_files - oldFiles;

    removeNodes(removedFiles);
    addNodes(addedFiles);

    foreach (const QString &file, removedFiles)
        CodeModel::instance()->removeSymbolsFrom(file);
    foreach (const QString &file, addedFiles)
        CodeModel::instance()->addFile(file);
}

void Project::recursiveScanDirectory(const QDir &dir, QSet<QString> &container)
{
    QRegExp projectFilePattern(QLatin1String(".*\\.rubyproject(?:\\.user)?$"));
    foreach (const QFileInfo &info, dir.entryInfoList(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::CaseSensitive)) {
        if (info.isDir())
            recursiveScanDirectory(QDir(info.filePath()), container);
        else if (projectFilePattern.indexIn(info.fileName()) == -1)
            container << info.filePath();
    }
    m_fsWatcher.addPath(dir.absolutePath());
}

void Project::addNodes(const QSet<QString> &nodes)
{
    using namespace ProjectExplorer;

    const QChar sep = QLatin1Char('/');
    QStringList path;
    foreach (const QString &node, nodes) {
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

    const QChar sep = QLatin1Char('/');
    QStringList path;

    foreach (const QString &node, nodes) {
        path = m_projectDir.relativeFilePath(node).split(sep);
        path.pop_back();
        FolderNode *folder = findFolderFor(path);

        foreach (FileNode *file, folder->fileNodes()) {
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

    foreach (const QString &part, path) {
        bool folderFound = false;
        foreach (FolderNode *subFolder, folder->folderNodes()) {
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
