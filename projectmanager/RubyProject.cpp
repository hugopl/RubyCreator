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

#include <coreplugin/progressmanager/progressmanager.h>
#include <projectexplorer/buildtargetinfo.h>
#include <projectexplorer/kitmanager.h>
#include <projectexplorer/target.h>
#include <texteditor/textdocument.h>

namespace Ruby {

const int MIN_TIME_BETWEEN_PROJECT_SCANS = 4500;

Project::Project(const Utils::FilePath &fileName) :
    ProjectExplorer::Project(Constants::MimeType, fileName, [this] { scheduleProjectScan(); })
{
    setId(Constants::ProjectId);
    m_projectDir = QDir(fileName.parentDir().toString());
    readProjectSettings(fileName);

    m_projectScanTimer.setSingleShot(true);
    connect(&m_projectScanTimer, &QTimer::timeout, this, [this] { refresh(); });

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

void Project::readProjectSettings(const Utils::FilePath &fileName)
{
    QString base = fileName.toFileInfo().absoluteDir().absolutePath();
    base.append("/");
    QSettings settings(fileName.toString(), QSettings::IniFormat);
    settings.beginGroup("Config");
    for (const QString &path : settings.value("Ignore").toStringList()) {
        if (path.isEmpty())
            continue;
        m_ignoredDirectories << base + path;
    }
    settings.endGroup();
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
        refresh();
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

void Project::refresh(ProjectExplorer::Target *target)
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
    if (!target)
        target = activeTarget();
    connect(watcher, &QFutureWatcher<void>::finished,
            this, [this, watcher, target] {
        QList<ProjectExplorer::BuildTargetInfo> appTargets;
        auto newRoot = std::make_unique<ProjectNode>(projectDirectory());
        for (const QString &f : m_files) {
            const Utils::FilePath path = Utils::FilePath::fromString(f);
            newRoot->addNestedNode(std::make_unique<ProjectExplorer::FileNode>(
                                       path, ProjectExplorer::FileNode::fileTypeForFileName(path)));
            using ProjectExplorer::FileType;
            if (!f.endsWith(".rubyproject")) {
                ProjectExplorer::BuildTargetInfo bti;
                bti.buildKey = f;
                bti.targetFilePath = Utils::FilePath::fromString(f);
                bti.projectFilePath = projectFilePath();
                appTargets.append(bti);
            }
        }
        setRootProjectNode(std::move(newRoot));
        if (target)
            target->setApplicationTargets(appTargets);
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
                                         | QDir::NoSymLinks | QDir::CaseSensitive | QDir::Hidden);
    for (const QFileInfo &info : files) {
        if (m_projectScanFuture.isCanceled())
            return;
        if (info.isDir() && !info.isHidden() && !shouldIgnoreDir(info.filePath()))
            recursiveScanDirectory(QDir(info.filePath()));
        else if (!projectFilePattern.match(info.fileName()).hasMatch())
            m_files << info.filePath();
    }
    m_fsWatcher.addPath(dir.absolutePath());
}

bool Project::shouldIgnoreDir(const QString &filePath) const
{
    for (const QString& path : m_ignoredDirectories)
        if (filePath.startsWith(path))
          return true;
    return false;
}

Project::RestoreResult Project::fromMap(const QVariantMap &map, QString *errorMessage)
{
    RestoreResult res = ProjectExplorer::Project::fromMap(map, errorMessage);
    if (res == RestoreResult::Ok) {
        refresh();

        ProjectExplorer::Kit *defaultKit = ProjectExplorer::KitManager::defaultKit();
        if (!activeTarget() && defaultKit)
            addTarget(createTarget(defaultKit));
    }

    return res;
}

bool Project::setupTarget(ProjectExplorer::Target *t)
{
    refresh(t);
    return ProjectExplorer::Project::setupTarget(t);
}

}
