#ifndef Ruby_Project
#define Ruby_Project

#include <projectexplorer/project.h>
#include <projectexplorer/projectnodes.h>

#include <QFileSystemWatcher>
#include <QElapsedTimer>
#include <QTimer>

namespace TextEditor { class TextDocument; }

namespace Ruby {

class ProjectManager;
class ProjectNode;

class Project : public ProjectExplorer::Project
{
    Q_OBJECT
public:
    Project(ProjectManager *projectManager, const QString &fileName);
    QString displayName() const Q_DECL_OVERRIDE;
    Core::IDocument *document() const Q_DECL_OVERRIDE;
    ProjectExplorer::IProjectManager *projectManager() const Q_DECL_OVERRIDE;
    ProjectExplorer::ProjectNode *rootProjectNode() const Q_DECL_OVERRIDE;

    QStringList files(FilesMode) const Q_DECL_OVERRIDE;

private slots:
    void scheduleProjectScan();
    void populateProject();
private:
    ProjectManager *m_projectManager;
    TextEditor::TextDocument *m_document;
    ProjectNode *m_rootNode;

    QDir m_projectDir;
    QSet<QString> m_files;
    QFileSystemWatcher m_fsWatcher;

    QElapsedTimer m_lastProjectScan;
    QTimer m_projectScanTimer;

    void recursiveScanDirectory(const QDir &dir, QSet<QString> &container);
    void addNodes(const QSet<QString> &nodes);
    void removeNodes(const QSet<QString> &nodes);

    ProjectExplorer::FolderNode *findFolderFor(const QStringList &path);
};

}

#endif
