#include "RubyEditorWidget.h"

#include "RubyAutoCompleter.h"
#include "RubyCodeModel.h"
#include "RubyConstants.h"
#include "RubyEditor.h"
#include "RubyHighlighter.h"
#include "RubyIndenter.h"

#include <texteditor/basetextdocument.h>

#include <QDebug>
#include <QTextBlock>
#include <QTextCursor>

namespace Ruby {

EditorWidget::EditorWidget()
    : m_wordRegex("\\w+")
{
    setParenthesesMatchingEnabled(true);
    setMarksVisible(true);
    setCodeFoldingSupported(true);
    setAutoCompleter(new AutoCompleter);
    setLanguageSettingsId(Constants::SettingsId);

    TextEditor::BaseTextDocument* baseDoc = baseTextDocument();
    baseDoc->setIndenter(new Indenter());

    m_commentDefinition.multiLineStart.clear();
    m_commentDefinition.multiLineEnd.clear();
    m_commentDefinition.singleLine = '#';

    new Highlighter(document());
}

TextEditor::BaseTextEditor* EditorWidget::createEditor()
{
    return new Editor(this);
}

TextEditor::BaseTextEditorWidget::Link EditorWidget::findLinkAt(const QTextCursor& cursor, bool, bool)
{
    QString text = cursor.block().text();
    if (text.isEmpty())
        return Link();

    QString word;
    int cursorPos = cursor.positionInBlock();
    int pos = 0;
    while (true) {
        pos = m_wordRegex.indexIn(text, pos + word.length());
        if (pos == -1)
            return Link();

        word = m_wordRegex.cap();
        if (pos <= cursorPos && (pos + word.length()) >= cursorPos)
            break;
    }

    const QList<Symbol> symbols = CodeModel::instance()->allMethodsNamed(word);
    if (symbols.empty())
        return Link();

    // TODO: Implement an asssit to let the user choose the implementation to go.
    if (symbols.count() > 1)
        return Link();

    Link link;
    link.linkTextStart = cursor.position() + (pos - cursorPos);
    link.linkTextEnd = link.linkTextStart + word.length();
    link.targetLine = symbols.last().line;
    link.targetColumn = symbols.last().column;
    link.targetFileName = *symbols.last().file;

    return link;
}

void EditorWidget::unCommentSelection()
{
    Utils::unCommentSelection(this, m_commentDefinition);
}

}

#include "RubyEditorWidget.moc"
