#include "RubyQuickFixAssistProvider.h"

#include "RubyQuickFixes.h"
#include "../RubyConstants.h"

#include <coreplugin/id.h>
#include <texteditor/codeassist/quickfixassistprocessor.h>
#include <extensionsystem/pluginmanager.h>

namespace Ruby {

QuickFixAssistProvider::QuickFixAssistProvider(QObject *parent)
    : TextEditor::QuickFixAssistProvider(parent)
{
}

TextEditor::IAssistProvider::RunType QuickFixAssistProvider::runType() const
{
    return TextEditor::IAssistProvider::Synchronous;
}

TextEditor::IAssistProcessor*QuickFixAssistProvider::createProcessor() const
{
    return new TextEditor::QuickFixAssistProcessor(this);
}

QList<TextEditor::QuickFixFactory*> QuickFixAssistProvider::quickFixFactories() const
{
    QList<TextEditor::QuickFixFactory *> results;
    const auto factories = ExtensionSystem::PluginManager::getObjects<QuickFixFactory>();
    for (QuickFixFactory *f : factories)
        results.append(f);
    return results;
}

}
