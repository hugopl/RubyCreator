#include "RubyEditor.h"

#include "RubyEditorConstants.h"
#include "RubyEditorWidget.h"

namespace RubyEditor {

static const auto UpdateDocumentDefaultInterval = 150;

RubyEditor::RubyEditor(RubyEditorWidget* parent)
    : TextEditor::BaseTextEditor(parent)
{
    m_updateDocumentTimer.setSingleShot(true);
    m_updateDocumentTimer.setInterval(UpdateDocumentDefaultInterval);
    connect(&m_updateDocumentTimer, SIGNAL(timeout()), this, SLOT(updateDocumentNow()));
    connect(this, SIGNAL(contentsChanged()), this, SLOT(scheduleDocumentUpdate()));
}

Core::Id RubyEditor::id() const
{
    return Core::Id(Constants::EditorId);
}

void RubyEditor::scheduleDocumentUpdate()
{
    m_updateDocumentTimer.start();
}

void RubyEditor::updateDocumentNow()
{
    puts("Seems a good time to do a semantic highlight");
}

}

#include "RubyEditor.moc"
