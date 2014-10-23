#ifndef RubySnippetProvider_h
#define RubySnippetProvider_h

#include <texteditor/snippets/isnippetprovider.h>
#include <texteditor/snippets/snippeteditor.h>

#include "RubyConstants.h"
#include "RubyHighlighter.h"
#include "RubyIndenter.h"
#include "RubyAutoCompleter.h"

namespace Ruby {

class SnippetProvider : public TextEditor::ISnippetProvider
{
public:
    QString groupId() const override
    {
        return QLatin1String(Constants::SnippetGroupId);
    }

    QString displayName() const override
    {
        return QLatin1String("Ruby");
    }

    void decorateEditor(TextEditor::SnippetEditorWidget *editor) const override
    {
        if (TextEditor::TextDocument *document = editor->textDocument()) {
            document->setSyntaxHighlighter(new Highlighter);
            document->setIndenter(new Indenter);
        }
    }
};

}

#endif
