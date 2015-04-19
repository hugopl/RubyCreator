#ifndef Ruby_ProjectWizard_h
#define Ruby_ProjectWizard_h

#include <coreplugin/basefilewizard.h>
#include <utils/wizard.h>

namespace Utils {
class FileWizardPage;
}

namespace Ruby {

class ProjectWizardDialog : public Utils::Wizard
{
    Q_OBJECT
public:
    ProjectWizardDialog(QWidget* parent, const QString& path);
    QString path() const;
    QString projecyName() const;
private:
    Utils::FileWizardPage* m_page;
};

class ProjectWizard : public Core::BaseFileWizard
{
    Q_OBJECT
public:
    ProjectWizard();
protected:
    QWizard *createWizardDialog(QWidget *parent, const Core::WizardDialogParameters &wizardDialogParameters) const Q_DECL_OVERRIDE;
    Core::GeneratedFiles generateFiles(const QWizard *widget, QString *) const Q_DECL_OVERRIDE;
    bool postGenerateFiles(const QWizard *, const Core::GeneratedFiles &files, QString *errorMessage);
};

}

#endif
