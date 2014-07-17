#include "RubyEditorFactory.h"

#include "RubyConstants.h"
#include "RubyEditorWidget.h"

#include <texteditor/texteditoractionhandler.h>
#include <texteditor/texteditorsettings.h>

namespace Ruby {

EditorFactory::EditorFactory(QObject* parent)
    : Core::IEditorFactory(parent)
{
    setId(Constants::EditorId);
    setDisplayName(tr(Constants::EditorDisplayName));
    addMimeType(Constants::MimeType);
    new TextEditor::TextEditorActionHandler(this,
                              Constants::EditorId,
                              TextEditor::TextEditorActionHandler::Format
                              | TextEditor::TextEditorActionHandler::UnCommentSelection
                              | TextEditor::TextEditorActionHandler::UnCollapseAll
                              | TextEditor::TextEditorActionHandler::FollowSymbolUnderCursor);
}

Core::IEditor* EditorFactory::createEditor()
{
    EditorWidget* widget = new EditorWidget;
    TextEditor::TextEditorSettings::initializeEditor(widget);
    return widget->editor();
}

}

#include "RubyEditorFactory.moc"
