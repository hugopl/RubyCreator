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
    QString groupId() const Q_DECL_OVERRIDE
    {
        return QLatin1String(Constants::SnippetGroupId);
    }

    QString displayName() const Q_DECL_OVERRIDE
    {
        return QLatin1String("Ruby");
    }

    void decorateEditor(TextEditor::SnippetEditorWidget *editor) const Q_DECL_OVERRIDE
    {
        if (TextEditor::BaseTextDocument *document = editor->baseTextDocument()) {
            document->setSyntaxHighlighter(new Highlighter);
            document->setIndenter(new Indenter);
        }
    }
};

}

#endif
