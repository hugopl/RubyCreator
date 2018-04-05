#include "RubyProjectWizard.h"
#include "../RubyConstants.h"

#include <coreplugin/basefilewizard.h>
#include <coreplugin/id.h>
#include <projectexplorer/customwizard/customwizard.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <utils/filewizardpage.h>
#include <QDebug>
#include <QDir>

namespace Ruby {

ProjectWizard::ProjectWizard()
{
    setSupportedProjectTypes({ Constants::ProjectId });

    setDisplayName(tr("Import Existing Ruby Project"));
    setId("Z.Ruby");
    setDescription(tr("Imports existing Ruby projects."));
    setCategory(ProjectExplorer::Constants::IMPORT_WIZARD_CATEGORY);
    setDisplayCategory(ProjectExplorer::Constants::IMPORT_WIZARD_CATEGORY_DISPLAY);

    setIcon(QIcon(Constants::RubyIcon));
}

Core::BaseFileWizard *ProjectWizard::create(QWidget *parent, const Core::WizardDialogParameters &parameters) const
{
    Core::BaseFileWizard *wizard = new Core::BaseFileWizard(this, parameters.extraValues(), parent);
    wizard->setWindowTitle(displayName());

    Utils::FileWizardPage *page = new Utils::FileWizardPage;
    page->setPath(parameters.defaultPath());
    wizard->addPage(page);

    const auto pages = wizard->extensionPages();
    for (QWizardPage *p : pages)
        wizard->addPage(p);

    return wizard;
}

Core::GeneratedFiles ProjectWizard::generateFiles(const QWizard *widget, QString *) const
{
    const Core::BaseFileWizard *wizard = qobject_cast<const Core::BaseFileWizard *>(widget);
    Utils::FileWizardPage *page = wizard->find<Utils::FileWizardPage>();
    const QString projectPath = page->path();
    const QDir dir(projectPath);
    const QString projectName = page->fileName();

    Core::GeneratedFile projectFile(QFileInfo(dir, projectName + ".rubyproject").absoluteFilePath());
    projectFile.setContents("[Config]\nIgnore=");
    projectFile.setAttributes(Core::GeneratedFile::OpenProjectAttribute);

    return Core::GeneratedFiles() << projectFile;
}

bool ProjectWizard::postGenerateFiles(const QWizard*, const Core::GeneratedFiles &files, QString *errorMessage) const
{
    return ProjectExplorer::CustomProjectWizard::postGenerateOpen(files, errorMessage);
}

}
