#include "RubyIndenter.h"
#include "RubyScanner.h"

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

void Indenter::indentBlock(QTextDocument*, const QTextBlock &block, const QChar &, const TextEditor::TabSettings &settings)
{
    int indent;

    QTextBlock previous = block.previous();
    // Previous line ends on comma, ignore everything and follow the indent
    if (previous.text().endsWith(QLatin1Char(','))) {
        indent = previous.text().indexOf(QRegularExpression(QStringLiteral("[^\\s]"))) / settings.m_indentSize;
    } else {
        // Use the stored indent plus some bizarre heuristics that even myself remember how it works.
        indent = block.userState() >> 20;
        if (indent < 0) {
            while (indent == -1 && previous.isValid()) {
                indent = previous.userState() >> 20;
                previous = previous.previous();
            }
        }

        if (didBlockStart(block) && indent > 0)
            indent--;
    }

    settings.indentLine(block, indent  * settings.m_indentSize);
}

}
