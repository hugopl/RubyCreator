#include "RubyEditorWidget.h"

#include "RubyAutoCompleter.h"
#include "RubyEditor.h"
#include "RubyHighlighter.h"
#include "RubyIndenter.h"

#include <texteditor/basetextdocument.h>

namespace Ruby {

EditorWidget::EditorWidget()
{
    setParenthesesMatchingEnabled(true);
    setMarksVisible(true);
    setCodeFoldingSupported(true);
    setAutoCompleter(new AutoCompleter);

    TextEditor::BaseTextDocument* baseDoc = baseTextDocument();
    baseDoc->setIndenter(new Indenter());

    new Highlighter(baseDoc);
}

TextEditor::BaseTextEditor* EditorWidget::createEditor()
{
    return new Editor(this);
}

}

#include "RubyEditorWidget.moc"
