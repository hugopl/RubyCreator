#include "RubyEditor.h"

#include "../RubyConstants.h"
#include "RubyEditorWidget.h"
#include "RubyCompletionAssist.h"

#include <extensionsystem/pluginmanager.h>
#include <texteditor/texteditorconstants.h>

#include <QBuffer>

namespace Ruby {

Editor::Editor(TextEditor::BaseTextEditorWidget* editorWidget) : BaseTextEditor(editorWidget)
{
    Core::Context c = context();
    c.add(Constants::LangRuby);
    setContext(c);

    // There's no contentsChanged signal on Core::IDocument interface.
    connect(document(), SIGNAL(contentsChanged()), widget(), SLOT(scheduleCodeModelUpdate()));
}

TextEditor::CompletionAssistProvider*Editor::completionAssistProvider()
{
    return ExtensionSystem::PluginManager::getObject<CompletionAssistProvider>();
}

}
