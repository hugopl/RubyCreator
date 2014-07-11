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
        return Constants::SnippetGroupId;
    }

    QString displayName() const override
    {
        return "Ruby";
    }

    void decorateEditor(TextEditor::SnippetEditorWidget *editor) const override
    {
        editor->setSyntaxHighlighter(new Highlighter);
        editor->baseTextDocument()->setIndenter(new Indenter);
        editor->setAutoCompleter(new AutoCompleter);
    }
};

}

#endif
