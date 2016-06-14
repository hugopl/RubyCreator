#ifndef Ruby_ProjectWizard_h
#define Ruby_ProjectWizard_h

#include <coreplugin/basefilewizardfactory.h>

namespace Ruby {

class ProjectWizard : public Core::BaseFileWizardFactory
{
    Q_OBJECT
public:
    ProjectWizard();
protected:
    Core::BaseFileWizard *create(QWidget *parent, const Core::WizardDialogParameters &wizardDialogParameters) const override;
    Core::GeneratedFiles generateFiles(const QWizard *widget, QString *) const override;
    bool postGenerateFiles(const QWizard *, const Core::GeneratedFiles &files, QString *errorMessage) const override;
};

}

#endif
