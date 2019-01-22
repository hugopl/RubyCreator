#include "RubyAutoCompleter.h"
#include "RubyCompletionAssist.h"
#include "RubyEditorDocument.h"
#include "RubyEditor.h"
#include "RubyEditorFactory.h"
#include "RubyHighlighter.h"
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

    setDocumentCreator([]() { return new EditorDocument; });
    setIndenterCreator([](QTextDocument *doc) { return new Indenter(doc); });
    setEditorWidgetCreator([]() { return new EditorWidget; });
    setEditorCreator([]() { return new Editor; });
    setAutoCompleterCreator([]() { return new AutoCompleter; });
    setCompletionAssistProvider(new CompletionAssistProvider);
    setSyntaxHighlighterCreator([]() { return new Highlighter; });
    setCommentDefinition(Utils::CommentDefinition::HashStyle);
    setParenthesesMatchingEnabled(true);
    setCodeFoldingSupported(true);
    setMarksVisible(true);

    setEditorActionHandlers(TextEditor::TextEditorActionHandler::Format
                          | TextEditor::TextEditorActionHandler::UnCommentSelection
                          | TextEditor::TextEditorActionHandler::UnCollapseAll
                          | TextEditor::TextEditorActionHandler::FollowSymbolUnderCursor);
}

void EditorFactory::decorateEditor(TextEditor::TextEditorWidget *editor)
{
    if (TextEditor::TextDocument *document = editor->textDocument()) {
        document->setSyntaxHighlighter(new Highlighter);
        document->setIndenter(new Indenter(document->document()));
    }
}

}
