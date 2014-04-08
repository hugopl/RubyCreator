#include "RubyEditor.h"

#include "RubyCodeModel.h"
#include "RubyEditorConstants.h"
#include "RubyEditorWidget.h"

namespace RubyEditor {

static const auto UpdateDocumentDefaultInterval = 150;

RubyEditor::RubyEditor(RubyEditorWidget* parent)
    : TextEditor::BaseTextEditor(parent)
{
    m_updateCodeModelTimer.setSingleShot(true);
    m_updateCodeModelTimer.setInterval(UpdateDocumentDefaultInterval);
    connect(&m_updateCodeModelTimer, SIGNAL(timeout()), this, SLOT(updateCodeModel()));
    connect(this, SIGNAL(contentsChanged()), this, SLOT(scheduleCodeModelUpdate()));
}

Core::Id RubyEditor::id() const
{
    return Core::Id(Constants::EditorId);
}

void RubyEditor::scheduleCodeModelUpdate()
{
    m_updateCodeModelTimer.start();
}

void RubyEditor::updateCodeModel()
{
    RubyCodeModel::instance()->updateModel(document()->filePath());
}

}

#include "RubyEditor.moc"
