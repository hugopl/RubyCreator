#ifndef RUBYRUNCONFIGURATION_H
#define RUBYRUNCONFIGURATION_H

#include <projectexplorer/runconfiguration.h>

#include <QWidget>

namespace ProjectExplorer {
class WorkingDirectoryAspect;
class ArgumentsAspect;
class TerminalAspect;
class LocalEnvironmentAspect;
}

namespace Ruby {

class RunConfiguration : public ProjectExplorer::RunConfiguration
{
    Q_OBJECT

public:
    explicit RunConfiguration(ProjectExplorer::Target *target);

    QWidget *createConfigurationWidget() override;
    ProjectExplorer::Runnable runnable() const override;
    QString extraId() const override;
    bool fromMap(const QVariantMap &map) override;

private:
    ProjectExplorer::WorkingDirectoryAspect* m_workingDirectoryAspect;
    ProjectExplorer::ArgumentsAspect* m_argumentAspect;
    ProjectExplorer::TerminalAspect* m_terminalAspect;
    ProjectExplorer::LocalEnvironmentAspect* m_localEnvironmentAspect;
    QString m_script;
};

class RunConfigurationWidget : public QWidget
{
    Q_OBJECT

public:
	explicit RunConfigurationWidget(RunConfiguration *rc, QWidget *parent = nullptr);
};

class RunConfigurationFactory : public ProjectExplorer::IRunConfigurationFactory
{
public:
    RunConfigurationFactory();

    QList<ProjectExplorer::BuildTargetInfo> availableBuildTargets(
            ProjectExplorer::Target *parent, CreationMode mode) const override;

    bool canCreateHelper(ProjectExplorer::Target *parent, const QString &buildTarget) const override;
};

} // namespace Ruby

#endif // RUBYRUNCONFIGURATION_H
