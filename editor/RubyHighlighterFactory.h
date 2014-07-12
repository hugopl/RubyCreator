#ifndef RubyHighlighterFactory_h
#define RubyHighlighterFactory_h

#include <texteditor/ihighlighterfactory.h>
#include "RubyConstants.h"
#include "RubyHighlighter.h"

namespace Ruby {

class  HighlighterFactory : public TextEditor::IHighlighterFactory
{
public:
    HighlighterFactory()
    {
        setId(Constants::EditorId);
        addMimeType(Constants::MimeType);
    }

    TextEditor::SyntaxHighlighter* createHighlighter() const override
    {
        return new Highlighter;
    }
};

}

#endif
