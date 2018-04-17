#include "RubyRunConfiguration.h"
#include "RubyProject.h"
#include "../RubyConstants.h"

#include <projectexplorer/localenvironmentaspect.h>
#include <projectexplorer/runconfigurationaspects.h>
#include <projectexplorer/project.h>
#include <projectexplorer/runnables.h>
#include <projectexplorer/target.h>

#include <QFileInfo>
#include <QFormLayout>

#include <utils/fileutils.h>

using namespace ProjectExplorer;
using namespace Utils;

namespace Ruby {

const char C_RUNCONFIGURATIONPREFIX[] = "Ruby.RunConfiguration.";

RunConfiguration::RunConfiguration(Target *target)
    : ProjectExplorer::RunConfiguration(target, C_RUNCONFIGURATIONPREFIX)
{
    auto terminalAspect = new TerminalAspect(this, "Ruby.RunConfiguration.TerminalAspect");
    terminalAspect->setRunMode(ApplicationLauncher::Gui);
    addExtraAspect(terminalAspect);
    addExtraAspect(new ArgumentsAspect(this, "Ruby.RunConfiguration.ArgumentAspect"));
    addExtraAspect(new ExecutableAspect(this));
    addExtraAspect(new LocalEnvironmentAspect(this, LocalEnvironmentAspect::BaseEnvironmentModifier()));
    addExtraAspect(new WorkingDirectoryAspect(this, "Ruby.RunConfiguration.WorkingDirectoryAspect"));
}

QWidget *RunConfiguration::createConfigurationWidget()
{
    return wrapWidget(new RunConfigurationWidget(this));
}

Runnable RunConfiguration::runnable() const
{
    StandardRunnable result;
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

RunConfigurationWidget::RunConfigurationWidget(RunConfiguration *rc, QWidget *parent)
    : QWidget(parent)
{
    QTC_ASSERT(rc, return);
    auto fl = new QFormLayout(this);
    fl->setMargin(0);
    fl->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    rc->extraAspect<ExecutableAspect>()->addToMainConfigurationWidget(this, fl);
    rc->extraAspect<ArgumentsAspect>()->addToMainConfigurationWidget(this, fl);
    rc->extraAspect<WorkingDirectoryAspect>()->addToMainConfigurationWidget(this, fl);
    rc->extraAspect<TerminalAspect>()->addToMainConfigurationWidget(this, fl);
}

RunConfigurationFactory::RunConfigurationFactory()
{
    registerRunConfiguration<RunConfiguration>(C_RUNCONFIGURATIONPREFIX);
    addSupportedProjectType(Constants::ProjectId);
}

} // namespace Ruby
