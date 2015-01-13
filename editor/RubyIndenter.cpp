#include "RubyIndenter.h"
#include "RubyScanner.h"

#include <texteditor/tabsettings.h>
#include <QRegExp>
#include <QSet>
#include <QDebug>

namespace Ruby {

static bool didBlockStart(const QTextBlock &block)
{
    QString text = block.text();
    Scanner scanner(&text);
    scanner.readLine();
    return scanner.didBlockStart() || scanner.didBlockInterrupt();
}

void Indenter::indentBlock(QTextDocument*, const QTextBlock &block, const QChar &, const TextEditor::TabSettings &settings)
{
    bool isNewBlock = false;
    int indent = block.userState() >> 8;

    if (!indent)
        return;

    if (indent < 0) {
        QTextBlock previous = block.previous();
        while (indent == -1 && previous.isValid()) {
            indent = previous.userState() >> 8;
            previous = block.previous();
        }
        isNewBlock = true;
    }

    if (didBlockStart(block))
        indent--;

    if (isNewBlock || !block.text().isEmpty())
        settings.indentLine(block, indent  *settings.m_indentSize);
}

}
