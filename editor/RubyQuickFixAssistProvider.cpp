#include "RubyQuickFixAssistProvider.h"

#include "RubyQuickFixes.h"
#include "../RubyConstants.h"

#include <coreplugin/id.h>
#include <extensionsystem/pluginmanager.h>
#include <texteditor/codeassist/quickfixassistprocessor.h>

namespace Ruby {

TextEditor::IAssistProvider::RunType QuickFixAssistProvider::runType() const
{
    return TextEditor::IAssistProvider::Synchronous;
}

bool QuickFixAssistProvider::supportsEditor(Core::Id editorId) const
{
    return editorId == Constants::EditorId;
}

TextEditor::IAssistProcessor*QuickFixAssistProvider::createProcessor() const
{
    return new TextEditor::QuickFixAssistProcessor(this);
}

QList<TextEditor::QuickFixFactory*> QuickFixAssistProvider::quickFixFactories() const
{
    QList<TextEditor::QuickFixFactory *> results;
    foreach (QuickFixFactory *f, ExtensionSystem::PluginManager::getObjects<QuickFixFactory>())
        results.append(f);
    return results;
}

}
