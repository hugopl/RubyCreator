#include "RubyRunConfiguration.h"
#include "RubyProject.h"
#include "../RubyConstants.h"

#include <projectexplorer/localenvironmentaspect.h>
#include <projectexplorer/runconfigurationaspects.h>
#include <projectexplorer/project.h>
#include <projectexplorer/target.h>

#include <QFileInfo>
#include <QFormLayout>

#include <utils/fileutils.h>

using namespace ProjectExplorer;
using namespace Utils;

namespace Ruby {

RunConfiguration::RunConfiguration(Target *target, const Core::Id &id)
    : ProjectExplorer::RunConfiguration(target, id)
{
    auto terminalAspect = new TerminalAspect(this, "Ruby.RunConfiguration.TerminalAspect");
    terminalAspect->setRunMode(ApplicationLauncher::Gui);
    addExtraAspect(terminalAspect);
    addExtraAspect(new ArgumentsAspect(this, "Ruby.RunConfiguration.ArgumentAspect"));
    addExtraAspect(new ExecutableAspect(this));
    addExtraAspect(new LocalEnvironmentAspect(this, LocalEnvironmentAspect::BaseEnvironmentModifier()));
    addExtraAspect(new WorkingDirectoryAspect(this, "Ruby.RunConfiguration.WorkingDirectoryAspect"));
}

Runnable RunConfiguration::runnable() const
{
    Runnable result;
    result.runMode = extraAspect<TerminalAspect>()->runMode();
    result.executable = extraAspect<ExecutableAspect>()->executable().toString();
    result.commandLineArguments = extraAspect<ArgumentsAspect>()->arguments();
    result.workingDirectory = extraAspect<WorkingDirectoryAspect>()->workingDirectory().toString();
    result.environment = extraAspect<EnvironmentAspect>()->environment();
    return result;
}

void RunConfiguration::doAdditionalSetup(const RunConfigurationCreationInfo &info)
{
    const FileName script = FileName::fromString(info.buildKey);
    extraAspect<ExecutableAspect>()->setExecutable(script);
    setDefaultDisplayName(tr("Run %1").arg(script.fileName()));
}

RunConfigurationFactory::RunConfigurationFactory()
{
    registerRunConfiguration<RunConfiguration>("Ruby.RunConfiguration.");
    addSupportedProjectType(Constants::ProjectId);
}

} // namespace Ruby
