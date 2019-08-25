#include "RubyIndenter.h"
#include "RubyScanner.h"
#include "RubyBlockState.h"

#include <texteditor/tabsettings.h>
#include <QRegularExpression>
#include <QDebug>

namespace Ruby {

static bool didBlockStart(const QTextBlock &block)
{
    QString text = block.text();
    Scanner scanner(&text);
    scanner.readLine();
    return scanner.indentVariation() > 0 || scanner.didBlockInterrupt();
}

Indenter::Indenter(QTextDocument *doc) : TextEditor::TextIndenter(doc)
{
}

void Indenter::indentBlock(const QTextBlock &block,
                           const QChar &,
                           const TextEditor::TabSettings &settings,
                           int)
{
    int indent;

    QTextBlock previous = block.previous();
    // Previous line ends on comma, ignore everything and follow the indent
    if (previous.text().endsWith(',')) {
        indent = previous.text().indexOf(QRegularExpression("\\S")) / settings.m_indentSize;
    } else {
        // Use the stored indent plus some bizarre heuristics that even myself remember how it works.
        indent = RUBY_BLOCK_IDENT(block.userState());
        if (indent < 0) {
            while (indent == -1 && previous.isValid()) {
                indent = RUBY_BLOCK_IDENT(previous.userState());
                previous = previous.previous();
            }
        }

        if (didBlockStart(block) && indent > 0)
            indent--;
    }

    settings.indentLine(block, indent  * settings.m_indentSize);
}

}
