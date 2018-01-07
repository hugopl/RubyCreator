#ifndef Ruby_Project
#define Ruby_Project

#include <projectexplorer/project.h>
#include <projectexplorer/projectnodes.h>

#include <QElapsedTimer>
#include <QFileSystemWatcher>
#include <QFuture>
#include <QTimer>

namespace TextEditor { class TextDocument; }

namespace Ruby {

class ProjectNode;

class Project : public ProjectExplorer::Project
{
    Q_OBJECT

public:
    explicit Project(const Utils::FileName &fileName);
    ~Project();

private:
    void scheduleProjectScan();
    void populateProject();
    void scanProjectNow();
    void recursiveScanDirectory(const QDir &dir);

private:
    QDir m_projectDir;
    QSet<QString> m_files;
    QFuture<void> m_projectScanFuture;
    QFileSystemWatcher m_fsWatcher;
    bool m_populatingProject;

    QElapsedTimer m_lastProjectScan;
    QTimer m_projectScanTimer;
};

}

#endif
