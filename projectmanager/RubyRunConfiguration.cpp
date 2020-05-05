#include "RubyRunConfiguration.h"
#include "RubyProject.h"
#include "../RubyConstants.h"

#include <projectexplorer/localenvironmentaspect.h>
#include <projectexplorer/runconfigurationaspects.h>
#include <projectexplorer/runcontrol.h>
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
    addAspect<LocalEnvironmentAspect>(target);
    addAspect<ExecutableAspect>();
    addAspect<ArgumentsAspect>();
    addAspect<WorkingDirectoryAspect>();
    addAspect<TerminalAspect>();
}

Runnable RunConfiguration::runnable() const
{
    Runnable result;
    result.executable = aspect<ExecutableAspect>()->executable();
    result.commandLineArguments = aspect<ArgumentsAspect>()->arguments(macroExpander());
    result.workingDirectory = aspect<WorkingDirectoryAspect>()->workingDirectory(macroExpander()).toString();
    result.environment = aspect<EnvironmentAspect>()->environment();
    return result;
}

RunConfigurationFactory::RunConfigurationFactory()
{
    registerRunConfiguration<RunConfiguration>("Ruby.RunConfiguration.");
    addSupportedProjectType(Constants::ProjectId);
}

} // namespace Ruby
