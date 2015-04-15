#ifndef Ruby_ProjectWizard_h
#define Ruby_ProjectWizard_h

#include <coreplugin/basefilewizard.h>

namespace Ruby {

class ProjectWizard : public Core::BaseFileWizardFactory
{
    Q_OBJECT
public:
    ProjectWizard();
protected:
    Core::BaseFileWizard *create(QWidget *parent, const Core::WizardDialogParameters &wizardDialogParameters) const Q_DECL_OVERRIDE;
    Core::GeneratedFiles generateFiles(const QWizard *widget, QString *) const Q_DECL_OVERRIDE;
    bool postGenerateFiles(const QWizard *, const Core::GeneratedFiles &files, QString *errorMessage);
};

}

#endif
