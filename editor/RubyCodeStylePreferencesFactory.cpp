#include "RubyCodeStylePreferencesFactory.h"
#include "../RubyConstants.h"
#include "RubyIndenter.h"

#include <extensionsystem/pluginmanager.h>
#include <texteditor/snippets/isnippetprovider.h>
#include <texteditor/simplecodestylepreferences.h>

#include <QLabel>

namespace Ruby {

Core::Id CodeStylePreferencesFactory::languageId()
{
    return Constants::SettingsId;
}

QString CodeStylePreferencesFactory::displayName()
{
    return QStringLiteral("Ruby");
}

TextEditor::ICodeStylePreferences *CodeStylePreferencesFactory::createCodeStyle() const
{
    return new TextEditor::SimpleCodeStylePreferences();
}

QWidget *CodeStylePreferencesFactory::createEditor(TextEditor::ICodeStylePreferences*, QWidget *parent) const
{
    return new QLabel(tr("There's no configuration widget yet, sorry."), parent);
}

TextEditor::Indenter *CodeStylePreferencesFactory::createIndenter() const
{
    return new Indenter;
}

TextEditor::ISnippetProvider *CodeStylePreferencesFactory::snippetProvider() const
{
    const QList<TextEditor::ISnippetProvider *> &providers =
    ExtensionSystem::PluginManager::getObjects<TextEditor::ISnippetProvider>();
    foreach (TextEditor::ISnippetProvider *provider, providers)
        if (provider->groupId() == QLatin1String(Constants::SnippetGroupId))
            return provider;
    return 0;
}

QString CodeStylePreferencesFactory::previewText() const
{
    return QLatin1String(
            "module Foo\n"
            "  class Bar\n"
            "    attr_reader :something\n"
            "\n"
            "    def foo(arg1, arg2)\n"
            "      arg1.each do |i|\n"
            "        bar(i + 2) if arg2 =~ /^fo+/\n"
            "      end\n"
            "    end\n"
            "  end\n"
            "end\n");
}

}
