#include "RubyCodeStylePreferencesFactory.h"
#include "RubyConstants.h"
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

TextEditor::ICodeStylePreferences* CodeStylePreferencesFactory::createCodeStyle() const
{
    return new TextEditor::SimpleCodeStylePreferences();
}

QWidget* CodeStylePreferencesFactory::createEditor(TextEditor::ICodeStylePreferences*, QWidget* parent) const
{
    return new QLabel("There's no configuration widget yet, sorry.", parent);
}

TextEditor::Indenter* CodeStylePreferencesFactory::createIndenter() const
{
    return new Indenter;
}

TextEditor::ISnippetProvider* CodeStylePreferencesFactory::snippetProvider() const
{
    const QList<TextEditor::ISnippetProvider *> &providers =
    ExtensionSystem::PluginManager::getObjects<TextEditor::ISnippetProvider>();
    foreach (TextEditor::ISnippetProvider *provider, providers)
        if (provider->groupId() == Constants::SnippetGroupId)
            return provider;
    return 0;
}

QString CodeStylePreferencesFactory::previewText() const
{
    return "module Foo\n"
            "  class Bar\n"
            "    def foo(a, b)\n"
            "      0...b.each do |i|\n"
            "        bar i\n"
            "      end\n"
            "      b...a.each do |i|\n"
            "        bar i\n"
            "        bar b\n"
            "      end\n"
            "    end\n"
            "  end\n"
            "end\n";
}

}
