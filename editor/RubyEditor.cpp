#include "RubyEditor.h"

#include "RubyCodeModel.h"
#include "RubyConstants.h"
#include "RubyEditorWidget.h"

namespace Ruby {

static const auto UpdateDocumentDefaultInterval = 150;

Editor::Editor(EditorWidget* parent)
    : TextEditor::BaseTextEditor(parent)
{
    m_updateCodeModelTimer.setSingleShot(true);
    m_updateCodeModelTimer.setInterval(UpdateDocumentDefaultInterval);
    connect(&m_updateCodeModelTimer, SIGNAL(timeout()), this, SLOT(updateCodeModel()));
    connect(this, SIGNAL(contentsChanged()), this, SLOT(scheduleCodeModelUpdate()));
}

Core::Id Editor::id() const
{
    return Core::Id(Constants::EditorId);
}

void Editor::scheduleCodeModelUpdate()
{
    m_updateCodeModelTimer.start();
}

void Editor::updateCodeModel()
{
    CodeModel::instance()->updateModel(document()->filePath());
}

}

#include "RubyEditor.moc"
