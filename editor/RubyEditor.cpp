#include "RubyEditor.h"

#include "RubyCodeModel.h"
#include "RubyCompletionAssist.h"
#include "RubyConstants.h"
#include "RubyEditorWidget.h"

#include <extensionsystem/pluginmanager.h>
#include <texteditor/texteditorconstants.h>

#include <QBuffer>

namespace Ruby {

static const auto UpdateDocumentDefaultInterval = 150;

Editor::Editor(EditorWidget* parent)
    : TextEditor::BaseTextEditor(parent)
{
    setId(Constants::EditorId);
    setContext(Core::Context(Constants::EditorId, TextEditor::Constants::C_TEXTEDITOR));

    m_updateCodeModelTimer.setSingleShot(true);
    m_updateCodeModelTimer.setInterval(UpdateDocumentDefaultInterval);
    connect(&m_updateCodeModelTimer, &QTimer::timeout, this, &Editor::updateCodeModel);
    // There's no contentsChanged signal on Core::IDocument interface.
    connect(document(), SIGNAL(contentsChanged()), this, SLOT(scheduleCodeModelUpdate()));

    CodeModel::instance();
}

TextEditor::CompletionAssistProvider*Editor::completionAssistProvider()
{
    return ExtensionSystem::PluginManager::getObject<CompletionAssistProvider>();
}

void Editor::scheduleCodeModelUpdate()
{
    m_updateCodeModelTimer.start();
}

void Editor::updateCodeModel()
{
    const QString textData = baseTextDocument()->plainText();
    CodeModel::instance()->updateFile(document()->filePath(), textData);
}

}

#include "RubyEditor.moc"
