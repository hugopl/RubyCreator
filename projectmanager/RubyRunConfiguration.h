#ifndef RUBYRUNCONFIGURATION_H
#define RUBYRUNCONFIGURATION_H

#include <projectexplorer/runconfiguration.h>

#include <QWidget>

namespace Ruby {

class RunConfiguration : public ProjectExplorer::RunConfiguration
{
    Q_OBJECT

public:
    explicit RunConfiguration(ProjectExplorer::Target *target);

    QWidget *createConfigurationWidget() override;
    ProjectExplorer::Runnable runnable() const override;
    void doAdditionalSetup(const ProjectExplorer::RunConfigurationCreationInfo &info) override;
};

class RunConfigurationWidget : public QWidget
{
    Q_OBJECT

public:
	explicit RunConfigurationWidget(RunConfiguration *rc, QWidget *parent = nullptr);
};

class RunConfigurationFactory : public ProjectExplorer::RunConfigurationFactory
{
public:
    RunConfigurationFactory();
};

} // namespace Ruby

#endif // RUBYRUNCONFIGURATION_H
