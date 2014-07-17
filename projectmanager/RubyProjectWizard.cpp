#include "RubyProjectWizard.h"

#include <projectexplorer/customwizard/customwizard.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <utils/filewizardpage.h>

#include <QDebug>
#include <QDir>

namespace Ruby {

ProjectWizardDialog::ProjectWizardDialog(QWidget* parent, const QString& path)
    : Utils::Wizard(parent)
{
    setWindowTitle(tr("Import Existing Ruby Project"));

    // first page
    m_page = new Utils::FileWizardPage;
    m_page->setTitle(tr("Project Name and Location"));
    m_page->setFileNameLabel(tr("Project name:"));
    m_page->setPathLabel(tr("Location:"));
    m_page->setPath(path);

    const int firstPageId = addPage(m_page);
    wizardProgress()->item(firstPageId)->setTitle(tr("Location"));
}

QString ProjectWizardDialog::path() const
{
    return m_page->path();
}

QString ProjectWizardDialog::projecyName() const
{
    return m_page->fileName();
}

ProjectWizard::ProjectWizard()
{
    setWizardKind(IWizard::ProjectWizard);

    setDisplayName(tr("Import Existing Ruby Project"));
    setId(QStringLiteral("Z.Ruby"));
    setDescription(tr("Imports existing Ruby projects."));
    setCategory(ProjectExplorer::Constants::IMPORT_WIZARD_CATEGORY);
    setDisplayCategory(ProjectExplorer::Constants::IMPORT_WIZARD_CATEGORY_DISPLAY);
    setFlags(Core::IWizard::PlatformIndependent);

    setIcon(QIcon(":/rubysupport/Ruby.png"));
}

QWizard* ProjectWizard::createWizardDialog(QWidget* parent, const Core::WizardDialogParameters& wizardDialogParameters) const
{
    ProjectWizardDialog* wizard = new ProjectWizardDialog(parent, wizardDialogParameters.defaultPath());

    foreach (QWizardPage* p, wizardDialogParameters.extensionPages())
        BaseFileWizard::applyExtensionPageShortTitle(wizard, wizard->addPage(p));

    return wizard;
}

Core::GeneratedFiles ProjectWizard::generateFiles(const QWizard* widget, QString*) const
{
    const ProjectWizardDialog* wizard = qobject_cast<const ProjectWizardDialog*>(widget);
    const QString projectPath = wizard->path();
    const QDir dir(projectPath);
    const QString projectName = wizard->projecyName();

    Core::GeneratedFile projectFile(QFileInfo(dir, projectName + ".rubyproject").absoluteFilePath());
    projectFile.setContents(QStringLiteral("# Ruby project\n"));
    projectFile.setAttributes(Core::GeneratedFile::OpenProjectAttribute);

    return Core::GeneratedFiles() << projectFile;
}

bool ProjectWizard::postGenerateFiles(const QWizard*, const Core::GeneratedFiles& files, QString* errorMessage)
{
    return ProjectExplorer::CustomProjectWizard::postGenerateOpen(files, errorMessage);
}

}

#include "RubyProjectWizard.moc"
