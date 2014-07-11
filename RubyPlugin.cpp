#include "RubyPlugin.h"

#include "RubyConstants.h"

#include "editor/RubyCodeModel.h"
#include "editor/RubyCodeStylePreferencesFactory.h"
#include "editor/RubyEditorFactory.h"
#include "editor/RubySnippetProvider.h"
#include "editor/RubySymbolFilter.h"
#include "editor/RubyCompletionAssist.h"
#include "projectmanager/RubyProjectManager.h"
#include "projectmanager/RubyProjectWizard.h"

#include <coreplugin/icore.h>
#include <coreplugin/mimedatabase.h>
#include <QtPlugin>

#include <texteditor/codestylepool.h>
#include <texteditor/simplecodestylepreferences.h>
#include <texteditor/tabsettings.h>
#include <texteditor/texteditorsettings.h>

namespace Ruby {

Plugin::Plugin()
{
}

Plugin::~Plugin()
{
    TextEditor::TextEditorSettings::unregisterCodeStyle(Constants::SettingsId);
    TextEditor::TextEditorSettings::unregisterCodeStylePool(Constants::SettingsId);
    TextEditor::TextEditorSettings::unregisterCodeStyleFactory(Constants::SettingsId);

    removeObject(m_factory);
}

bool Plugin::initialize(const QStringList&, QString* errorString)
{
    if (!Core::MimeDatabase::addMimeTypes(QLatin1String(":rubysupport/Ruby.mimetypes.xml"), errorString))
        return false;

    initializeToolsSettings();

    addAutoReleasedObject(new SnippetProvider);

    m_factory = new EditorFactory(this);
    addObject(m_factory);
    addAutoReleasedObject(new SymbolFilter([](const QString& file) {
        return CodeModel::instance()->methodsIn(file);
    }, "Ruby Methods in Current Document", '.'));
    addAutoReleasedObject(new SymbolFilter([](const QString&) {
        return CodeModel::instance()->allMethods();
    }, "Ruby methods", 'm'));
    addAutoReleasedObject(new ProjectManager);

    addAutoReleasedObject(new ProjectWizard);

    addAutoReleasedObject(new CompletionAssistProvider);
    return true;
}

void Plugin::extensionsInitialized()
{
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
    QSettings* s = Core::ICore::settings();
    globalCodeStyle->fromSettings(QLatin1String(Constants::SettingsId), s);

    // mimetypes to be handled
    TextEditor::TextEditorSettings::registerMimeTypeForLanguageId(Constants::MimeType, Constants::SettingsId);
}

}

Q_EXPORT_PLUGIN(Ruby::Plugin)

#include "RubyPlugin.moc"
