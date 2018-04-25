#ifndef RUBYRUNCONFIGURATION_H
#define RUBYRUNCONFIGURATION_H

#include <projectexplorer/runconfiguration.h>

#include <QWidget>

namespace Ruby {

class RunConfiguration : public ProjectExplorer::RunConfiguration
{
    Q_OBJECT

public:
    explicit RunConfiguration(ProjectExplorer::Target *target, const Core::Id &id);

    ProjectExplorer::Runnable runnable() const override;
    void doAdditionalSetup(const ProjectExplorer::RunConfigurationCreationInfo &info) override;
};

class RunConfigurationFactory : public ProjectExplorer::RunConfigurationFactory
{
public:
    RunConfigurationFactory();
};

} // namespace Ruby

#endif // RUBYRUNCONFIGURATION_H
