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
const QString C_WORKINGDIRECTORYASPECT_ID = QStringLiteral("Ruby.RunConfiguration.WorkingDirectoryAspect");
const QString C_ARGUMENTASPECT_ID = QStringLiteral("Ruby.RunConfiguration.ArgumentAspect");
const QString C_TERMINALASPECT_ID = QStringLiteral("Ruby.RunConfiguration.TerminalAspect");
const QString C_EXECUTABLE_KEY = QStringLiteral("Ruby.RunConfiguration.Executable");
const QString C_WORKINGDIRECTORY_KEY = QStringLiteral("Ruby.RunConfiguration.WorkingDirectory");
const QString C_COMMANDLINEARGS_KEY = QStringLiteral("Ruby.RunConfiguration.CommandlineArgs");

RunConfiguration::RunConfiguration(Target *target)
	: ProjectExplorer::RunConfiguration(target, C_RUNCONFIGURATIONPREFIX)
	, m_workingDirectoryAspect(new WorkingDirectoryAspect(this, C_WORKINGDIRECTORYASPECT_ID))
	, m_argumentAspect(new ArgumentsAspect(this, C_ARGUMENTASPECT_ID))
	, m_terminalAspect(new TerminalAspect(this, C_TERMINALASPECT_ID))
	, m_localEnvironmentAspect(new LocalEnvironmentAspect(this, LocalEnvironmentAspect::BaseEnvironmentModifier()))
{
	m_terminalAspect->setRunMode(ApplicationLauncher::Gui);

    addExtraAspect(m_argumentAspect);
    addExtraAspect(m_terminalAspect);
    addExtraAspect(m_localEnvironmentAspect);
}

QWidget *RunConfiguration::createConfigurationWidget()
{
    return new RunConfigurationWidget(this);
}

Runnable RunConfiguration::runnable() const
{
	StandardRunnable result;
    result.runMode = m_terminalAspect->runMode();
    result.executable = m_script;
    result.commandLineArguments = m_argumentAspect->arguments();
    result.workingDirectory = m_workingDirectoryAspect->workingDirectory().toString();
    result.environment = m_localEnvironmentAspect->environment();
    return result;
}

QString RunConfiguration::extraId() const
{
    return m_script;
}

bool RunConfiguration::fromMap(const QVariantMap &map)
{
	if (!ProjectExplorer::RunConfiguration::fromMap(map))
        return false;
    QString extraId = idFromMap(map).suffixAfter(id());
    if (!extraId.isEmpty())
        m_script = extraId;

    setDefaultDisplayName(tr("Run %1").arg(Utils::FileName::fromString(m_script).fileName()));
    return true;
}

RunConfigurationWidget::RunConfigurationWidget(RunConfiguration *rc, QWidget *parent)
    : QWidget(parent)
{
    QTC_ASSERT(rc, return);
    auto fl = new QFormLayout(this);
    fl->setMargin(0);
    fl->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
	rc->extraAspect<ArgumentsAspect>()->addToMainConfigurationWidget(this, fl);
	rc->extraAspect<TerminalAspect>()->addToMainConfigurationWidget(this,fl);
}

RunConfigurationFactory::RunConfigurationFactory()
{
	setObjectName("RunConfigurationFactory");
    registerRunConfiguration<RunConfiguration>(C_RUNCONFIGURATIONPREFIX);
    addSupportedProjectType(Constants::ProjectId);
}

QList<BuildTargetInfo> RunConfigurationFactory::availableBuildTargets(
		Target *parent, IRunConfigurationFactory::CreationMode mode) const
{
    Q_UNUSED(mode);
    Ruby::Project *project = static_cast<Ruby::Project *>(parent->project());
    return Utils::transform(project->files(Project::AllFiles), [project](const FileName &fn) {
		BuildTargetInfo bti(fn.toString(), fn, project->projectFilePath());
        bti.displayName = fn.fileName();
        return bti;
    });
}

bool RunConfigurationFactory::canCreateHelper(Target *parent, const QString &buildTarget) const
{
    Ruby::Project *project = static_cast<Ruby::Project *>(parent->project());
    const QString script = buildTarget;
    if (script.endsWith(".rubyproject"))
        return false;
    return project->files(ProjectExplorer::Project::AllFiles).contains(FileName::fromString(script));
}

} // namespace Ruby
