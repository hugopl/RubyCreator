#include "RubyProjectWizard.h"

#include <coreplugin/basefilewizard.h>
#include <projectexplorer/customwizard/customwizard.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <utils/filewizardpage.h>
#include <QDebug>
#include <QDir>

namespace Ruby {

ProjectWizard::ProjectWizard()
{
    setWizardKind(Core::IWizardFactory::ProjectWizard);

    setDisplayName(tr("Import Existing Ruby Project"));
    setId(QStringLiteral("Z.Ruby"));
    setDescription(tr("Imports existing Ruby projects."));
    setCategory(QLatin1String(ProjectExplorer::Constants::IMPORT_WIZARD_CATEGORY));
    setDisplayCategory(QLatin1String(ProjectExplorer::Constants::IMPORT_WIZARD_CATEGORY_DISPLAY));

    setIcon(QIcon(QLatin1String(":/rubysupport/Ruby.png")));
}

Core::BaseFileWizard* ProjectWizard::create(QWidget* parent, const Core::WizardDialogParameters& parameters) const
{
    Core::BaseFileWizard* wizard = new Core::BaseFileWizard(parent);
    wizard->setWindowTitle(displayName());

    Utils::FileWizardPage* page = new Utils::FileWizardPage;
    page->setPath(parameters.defaultPath());
    wizard->addPage(page);

    foreach (QWizardPage *p, parameters.extensionPages())
        wizard->addPage(p);

    return wizard;
}

Core::GeneratedFiles ProjectWizard::generateFiles(const QWizard* widget, QString*) const
{
    const Core::BaseFileWizard* wizard = qobject_cast<const Core::BaseFileWizard *>(widget);
    Utils::FileWizardPage* page = wizard->find<Utils::FileWizardPage>();
    const QString projectPath = page->path();
    const QDir dir(projectPath);
    const QString projectName = page->fileName();

    Core::GeneratedFile projectFile(QFileInfo(dir, projectName + QLatin1String(".rubyproject")).absoluteFilePath());
    projectFile.setContents(QLatin1String("# Ruby project\n"));
    projectFile.setAttributes(Core::GeneratedFile::OpenProjectAttribute);

    return Core::GeneratedFiles() << projectFile;
}

bool ProjectWizard::postGenerateFiles(const QWizard*, const Core::GeneratedFiles& files, QString* errorMessage)
{
    return ProjectExplorer::CustomProjectWizard::postGenerateOpen(files, errorMessage);
}

}
