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
    addAspect<LocalEnvironmentAspect>(target, LocalEnvironmentAspect::BaseEnvironmentModifier());
    addAspect<ExecutableAspect>();
    addAspect<ArgumentsAspect>();
    addAspect<WorkingDirectoryAspect>();
    addAspect<TerminalAspect>();
}

Runnable RunConfiguration::runnable() const
{
    Runnable result;
    result.executable = extraAspect<ExecutableAspect>()->executable().toString();
    result.commandLineArguments = extraAspect<ArgumentsAspect>()->arguments(macroExpander());
    result.workingDirectory = extraAspect<WorkingDirectoryAspect>()->workingDirectory(macroExpander()).toString();
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
