#include "RubyProject.h"

#include "../editor/RubyCodeModel.h"
#include "../RubyConstants.h"
#include "RubyProjectNode.h"

#include <QDebug>
#include <QtConcurrent>
#include <QFileInfo>
#include <QFutureWatcher>
#include <QRegularExpression>
#include <QThread>

#include <texteditor/textdocument.h>
#include <coreplugin/progressmanager/progressmanager.h>

namespace Ruby {

const int MIN_TIME_BETWEEN_PROJECT_SCANS = 4500;

Project::Project(const Utils::FileName &fileName) :
    ProjectExplorer::Project(Constants::MimeType, fileName, [this] { scheduleProjectScan(); })
{
    setId(Constants::ProjectId);
    m_projectDir = QDir(fileName.parentDir().toString());

    m_projectScanTimer.setSingleShot(true);
    connect(&m_projectScanTimer, &QTimer::timeout, this, &Project::scanProjectNow);

    scanProjectNow();

    connect(&m_fsWatcher, &QFileSystemWatcher::directoryChanged, this, &Project::scheduleProjectScan);

    setDisplayName(m_projectDir.dirName());
}

Project::~Project()
{
    if (m_projectScanFuture.isRunning()) {
        m_projectScanFuture.cancel();
        m_projectScanFuture.waitForFinished();
    }
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
        scanProjectNow();
    }
}

void Project::populateProject()
{
    m_lastProjectScan.start();
    QSet<QString> oldFiles(m_files);
    m_files.clear();
    recursiveScanDirectory(m_projectDir);
    if (m_projectScanFuture.isCanceled())
        return;

    const auto removedFiles = oldFiles - m_files;
    const auto addedFiles = m_files - oldFiles;

    for (const QString &file : removedFiles)
        CodeModel::instance()->removeSymbolsFrom(file);
    for (const QString &file : addedFiles)
        CodeModel::instance()->addFile(file);
}

void Project::scanProjectNow()
{
    if (isParsing()) {
        m_projectScanTimer.setInterval(MIN_TIME_BETWEEN_PROJECT_SCANS);
        m_projectScanTimer.start();
        return;
    }
    emitParsingStarted();
    m_projectScanFuture = QtConcurrent::run(this, &Project::populateProject);
    auto *watcher = new QFutureWatcher<void>();
    watcher->setFuture(m_projectScanFuture);
    Core::ProgressManager::instance()->addTask(m_projectScanFuture, tr("Parsing Ruby Files"), Constants::RubyProjectTask);
    connect(watcher, &QFutureWatcher<void>::finished,
            this, [this, watcher] {
        auto newRoot = new ProjectNode(projectDirectory());
        for (const QString &f : m_files) {
            const Utils::FileName path = Utils::FileName::fromString(f);
            newRoot->addNestedNode(new ProjectExplorer::FileNode(
                                       path, ProjectExplorer::FileNode::fileTypeForFileName(path), false));
        }
        setRootProjectNode(newRoot);
        emitParsingFinished(true);
        watcher->deleteLater();
    });
}

void Project::recursiveScanDirectory(const QDir &dir)
{
    if (m_projectScanFuture.isCanceled())
        return;
    QRegularExpression projectFilePattern(".*\\.rubyproject(?:\\.user)?$");
    const auto files = dir.entryInfoList(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot
                                         | QDir::NoSymLinks | QDir::CaseSensitive);
    for (const QFileInfo &info : files) {
        if (m_projectScanFuture.isCanceled())
            return;
        if (info.isDir())
            recursiveScanDirectory(QDir(info.filePath()));
        else if (!projectFilePattern.match(info.fileName()).hasMatch())
            m_files << info.filePath();
    }
    m_fsWatcher.addPath(dir.absolutePath());
}

}
