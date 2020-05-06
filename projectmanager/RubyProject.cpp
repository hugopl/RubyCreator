#include "RubyProject.h"

#include "../editor/RubyCodeModel.h"
#include "../RubyConstants.h"

#include <QDebug>
#include <QElapsedTimer>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QFuture>
#include <QFutureWatcher>
#include <QRegularExpression>
#include <QtConcurrent>
#include <QThread>
#include <QTimer>

#include <coreplugin/progressmanager/progressmanager.h>
#include <projectexplorer/buildsystem.h>
#include <projectexplorer/buildtargetinfo.h>
#include <projectexplorer/kitmanager.h>
#include <projectexplorer/target.h>
#include <texteditor/textdocument.h>

namespace Ruby {

class BuildSystem : public ProjectExplorer::BuildSystem
{
public:
    explicit BuildSystem(ProjectExplorer::Target *target);
    ~BuildSystem();

    void triggerParsing() final;

private:
    QStringList m_rawFileList;
    QHash<QString, QString> m_rawListEntries;
    QSet<QString> m_files;
    QTimer m_projectScanTimer;
    QStringList m_ignoredDirectories;
    QElapsedTimer m_lastProjectScan;
    QFileSystemWatcher m_fsWatcher;
    QFuture<void> m_projectScanFuture;

    void scheduleProjectScan();
    void populateProject();
    void recursiveScanDirectory(const QDir &dir);
    bool shouldIgnoreDir(const QString &filePath) const;
    void readProjectSettings(const Utils::FilePath &fileName);
};

class ProjectNode : public ProjectExplorer::ProjectNode
{
public:
    ProjectNode(const Utils::FilePath &path)
        : ProjectExplorer::ProjectNode(path)
    {
        setDisplayName(path.toFileInfo().completeBaseName());
        setAddFileFilter("*.rb");
    }
};

/**
 * @brief Provides displayName relative to project node
 */
class FileNode : public ProjectExplorer::FileNode
{
public:
    FileNode(const Utils::FilePath &filePath)
        : ProjectExplorer::FileNode(filePath, ProjectExplorer::FileType::Source)
        , m_displayName(filePath.toFileInfo().fileName())
    {
    }

    QString displayName() const override { return m_displayName; }
private:
    QString m_displayName;
};

BuildSystem::BuildSystem(ProjectExplorer::Target *target)
    : ProjectExplorer::BuildSystem(target)
{
    m_projectScanTimer.setSingleShot(true);
    connect(&m_projectScanTimer, &QTimer::timeout, this, &BuildSystem::triggerParsing);
    connect(target->project(), &Project::projectFileIsDirty, this, &BuildSystem::triggerParsing);
    connect(&m_fsWatcher, &QFileSystemWatcher::directoryChanged, this, &BuildSystem::scheduleProjectScan);
    QTimer::singleShot(0, this, &BuildSystem::triggerParsing);
    readProjectSettings(projectFilePath());
}

const int MIN_TIME_BETWEEN_PROJECT_SCANS = 4500;

Project::Project(const Utils::FilePath &fileName) :
    ProjectExplorer::Project(Constants::MimeType, fileName)
{
    setId(Constants::ProjectId);
    setDisplayName(fileName.toFileInfo().completeBaseName());

    setNeedsBuildConfigurations(false);
    setBuildSystemCreator([](ProjectExplorer::Target *t) { return new BuildSystem(t); });
}

BuildSystem::~BuildSystem()
{
    if (m_projectScanFuture.isRunning()) {
        m_projectScanFuture.cancel();
        m_projectScanFuture.waitForFinished();
    }
}

void BuildSystem::readProjectSettings(const Utils::FilePath &fileName)
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

void BuildSystem::scheduleProjectScan()
{
    auto elapsedTime = m_lastProjectScan.elapsed();
    if (elapsedTime < MIN_TIME_BETWEEN_PROJECT_SCANS) {
        if (!m_projectScanTimer.isActive()) {
            m_projectScanTimer.setInterval(MIN_TIME_BETWEEN_PROJECT_SCANS - elapsedTime);
            m_projectScanTimer.start();
        }
    } else {
        triggerParsing();
    }
}

void BuildSystem::populateProject()
{
    m_lastProjectScan.start();
    QSet<QString> oldFiles(m_files);
    m_files.clear();
    const QDir baseDir(projectDirectory().toString());
    recursiveScanDirectory(baseDir);
    if (m_projectScanFuture.isCanceled())
        return;

    const auto removedFiles = oldFiles - m_files;
    const auto addedFiles = m_files - oldFiles;

    for (const QString &file : removedFiles)
        CodeModel::instance()->removeSymbolsFrom(file);
    for (const QString &file : addedFiles)
        CodeModel::instance()->addFile(file);
}

void BuildSystem::triggerParsing()
{
    if (isParsing()) {
        m_projectScanTimer.setInterval(MIN_TIME_BETWEEN_PROJECT_SCANS);
        m_projectScanTimer.start();
        return;
    }
    m_projectScanFuture = QtConcurrent::run(this, &BuildSystem::populateProject);
    auto *watcher = new QFutureWatcher<void>();
    watcher->setFuture(m_projectScanFuture);
    Core::ProgressManager::instance()->addTask(m_projectScanFuture, tr("Parsing Ruby Files"), Constants::RubyProjectTask);
    connect(watcher, &QFutureWatcher<void>::finished,
            this, [this, watcher] {
        BuildSystem::ParseGuard guard(guardParsingRun());
        const QDir baseDir(projectDirectory().toString());
        QList<ProjectExplorer::BuildTargetInfo> appTargets;
        auto newRoot = std::make_unique<ProjectNode>(projectDirectory());
        for (const QString &f : qAsConst(m_files)) {
            const Utils::FilePath filePath = Utils::FilePath::fromString(f);

            newRoot->addNestedNode(std::make_unique<FileNode>(filePath));
            if (f.endsWith(".rubyproject"))
                continue;
            ProjectExplorer::BuildTargetInfo bti;
            bti.buildKey = f;
            bti.targetFilePath = filePath;
            bti.projectFilePath = projectFilePath();
            qDebug() << filePath;
            appTargets.append(bti);
        }
        setRootProjectNode(std::move(newRoot));

        setApplicationTargets(appTargets);

        guard.markAsSuccess();

        emitBuildSystemUpdated();
        watcher->deleteLater();
    });
}

void BuildSystem::recursiveScanDirectory(const QDir &dir)
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

bool BuildSystem::shouldIgnoreDir(const QString &filePath) const
{
    for (const QString& path : m_ignoredDirectories)
        if (filePath.startsWith(path))
          return true;
    return false;
}

Project::RestoreResult Project::fromMap(const QVariantMap &map, QString *errorMessage)
{
    RestoreResult res = ProjectExplorer::Project::fromMap(map, errorMessage);
    if (res == RestoreResult::Ok)
        addTargetForDefaultKit();

    return res;
}

}
