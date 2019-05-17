#include "RubyPlugin.h"

#include "RubyConstants.h"

#include "editor/RubyCodeModel.h"
#include "editor/RubyCodeStylePreferencesFactory.h"
#include "editor/RubyEditorFactory.h"
#include "editor/RubyHighlighter.h"
#include "editor/RubyQuickFixAssistProvider.h"
#include "editor/RubyQuickFixes.h"
#include "editor/RubySymbolFilter.h"
#include "editor/RubyCompletionAssist.h"
#include "editor/RubyRubocopHighlighter.h"
#include "projectmanager/RubyProject.h"
#include "projectmanager/RubyProjectWizard.h"
#include "projectmanager/RubyRunConfiguration.h"

#include <coreplugin/icore.h>
#include <projectexplorer/projectmanager.h>
#include <texteditor/codestylepool.h>
#include <texteditor/simplecodestylepreferences.h>
#include <texteditor/snippets/snippetprovider.h>
#include <texteditor/tabsettings.h>
#include <texteditor/texteditor.h>
#include <texteditor/texteditorsettings.h>

namespace Ruby {

class PluginPrivate
{
public:
    EditorFactory editorFactory;
    RunConfigurationFactory runConfigFactory;
    SymbolFilter curDocumentMethodsSymbolFilter;
    SymbolFilter methodsSymbolFilter;
    SymbolFilter classesSymbolFilter;
    ProjectWizard projectWizard;
    CompletionAssistProvider completionAssistProvider;
    QuickFixAssistProvider m_quickFixProvider;
    RubocopHighlighter highlighter;

    PluginPrivate() :
        curDocumentMethodsSymbolFilter([](const QString &file) {
            return CodeModel::instance()->methodsIn(file);
        }, "Ruby Methods in Current Document", '.'),
        methodsSymbolFilter([](const QString &) {
            return CodeModel::instance()->allMethods();
        }, "Ruby methods", 'm'),
        classesSymbolFilter([](const QString &) {
            return CodeModel::instance()->allClasses();
        }, "Ruby classes", 'c')
    {}
};

Plugin *Plugin::m_instance = nullptr;

Plugin::Plugin()
{
    m_instance = this;
}

Plugin::~Plugin()
{
    TextEditor::TextEditorSettings::unregisterCodeStyle(Constants::SettingsId);
    TextEditor::TextEditorSettings::unregisterCodeStylePool(Constants::SettingsId);
    TextEditor::TextEditorSettings::unregisterCodeStyleFactory(Constants::SettingsId);

    m_instance = nullptr;
    delete d;
}

Plugin *Plugin::instance()
{
    return m_instance;
}

bool Plugin::initialize(const QStringList &, QString *errorString)
{
    Q_UNUSED(errorString)

    initializeToolsSettings();

    d = new PluginPrivate;
    ProjectExplorer::ProjectManager::registerProjectType<Project>(Constants::ProjectMimeType);

    Core::IWizardFactory::registerFactoryCreator([]() {
        return QList<Core::IWizardFactory *>() << new ProjectWizard;
    });

    registerQuickFixes(this);
    TextEditor::SnippetProvider::registerGroup(Constants::SnippetGroupId,
                                               tr("Ruby", "SnippetProvider"),
                                               &EditorFactory::decorateEditor);

    return true;
}

void Plugin::extensionsInitialized()
{
}

QuickFixAssistProvider *Plugin::quickFixProvider()
{
    return &d->m_quickFixProvider;
}

void Plugin::initializeToolsSettings()
{
    // code style factory
    auto factory = new CodeStylePreferencesFactory;
    TextEditor::TextEditorSettings::registerCodeStyleFactory(factory);

    // code style pool
    auto pool = new TextEditor::CodeStylePool(factory, this);
    TextEditor::TextEditorSettings::registerCodeStylePool(Constants::SettingsId, pool);

    // global code style settings
    auto globalCodeStyle = new TextEditor::SimpleCodeStylePreferences(this);
    globalCodeStyle->setDelegatingPool(pool);
    globalCodeStyle->setDisplayName(tr("Global", "Settings"));
    globalCodeStyle->setId("RubyGlobal");
    pool->addCodeStyle(globalCodeStyle);
    TextEditor::TextEditorSettings::registerCodeStyle(Constants::SettingsId, globalCodeStyle);

    // built-in settings
    // Ruby style
    auto rubyCodeStyle = new TextEditor::SimpleCodeStylePreferences;
    rubyCodeStyle->setId("ruby");
    rubyCodeStyle->setDisplayName(tr("RubyCreator"));
    rubyCodeStyle->setReadOnly(true);
    TextEditor::TabSettings tabSettings;
    tabSettings.m_tabPolicy = TextEditor::TabSettings::SpacesOnlyTabPolicy;
    tabSettings.m_tabSize = 2;
    tabSettings.m_indentSize = 2;
    tabSettings.m_continuationAlignBehavior = TextEditor::TabSettings::ContinuationAlignWithIndent;
    rubyCodeStyle->setTabSettings(tabSettings);
    pool->addCodeStyle(rubyCodeStyle);

    // default delegate for global preferences
    globalCodeStyle->setCurrentDelegate(rubyCodeStyle);

    pool->loadCustomCodeStyles();

    // load global settings (after built-in settings are added to the pool)
    globalCodeStyle->fromSettings(Constants::SettingsId, Core::ICore::settings());

    // mimetypes to be handled
    TextEditor::TextEditorSettings::registerMimeTypeForLanguageId(Constants::MimeType, Constants::SettingsId);
}

}
