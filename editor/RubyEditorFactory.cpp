#include "RubyAutoCompleter.h"
#include "RubyCompletionAssist.h"
#include "RubyEditor.h"
#include "RubyEditorFactory.h"
#include "RubyHighlighter.h"
#include "RubyHoverHandler.h"
#include "RubyIndenter.h"

#include "../RubyConstants.h"
#include "RubyEditorWidget.h"

#include <texteditor/texteditoractionhandler.h>
#include <texteditor/texteditorsettings.h>

#include <QCoreApplication>

namespace Ruby {

EditorFactory::EditorFactory()
{
    setId(Constants::EditorId);
    setDisplayName(qApp->translate("OpenWith::Editors", Constants::EditorDisplayName));
    addMimeType(Constants::MimeType);
    addMimeType(Constants::ProjectMimeType);

    //setDocumentCreator([]() { return new TextEditor::TextDocument(Constants::EditorId); });
    //setIndenterCreator([]() { return new Indenter; });
    //setEditorWidgetCreator([]() { return new EditorWidget; });
    //setEditorCreator([]() { return new Editor; });
    //setAutoCompleterCreator([]() { return new AutoCompleter; });
    //setCompletionAssistProvider(new CompletionAssistProvider);
    //setSyntaxHighlighterCreator([]() { return new Highlighter; });
    //setCommentStyle(Utils::CommentDefinition::HashStyle);
    //setParenthesesMatchingEnabled(true);
    //setCodeFoldingSupported(true);
    //setMarksVisible(true);
    //addHoverHandler(new HoverHandler);

    /*setEditorActionHandlers(TextEditor::TextEditorActionHandler::Format
                          | TextEditor::TextEditorActionHandler::UnCommentSelection
                          | TextEditor::TextEditorActionHandler::UnCollapseAll
                          | TextEditor::TextEditorActionHandler::FollowSymbolUnderCursor);
                          */
}

}
