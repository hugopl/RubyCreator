#include "RubyIndenter.h"
#include "RubyScanner.h"

#include <texteditor/tabsettings.h>
#include <QRegExp>
#include <QSet>
#include <QDebug>

namespace Ruby {

static bool didBlockStart(const QTextBlock& block) {
    QString text = block.text();
    Scanner scanner(&text);
    scanner.readLine();
    return scanner.didBlockStart() || scanner.didBlockInterrupt();
}

void Indenter::indentBlock(QTextDocument*, const QTextBlock& block, const QChar&, const TextEditor::TabSettings& settings)
{
    bool isNewBlock = false;
    int state = block.userState();
    int indent = state >> 8;

    if (indent > 0) {
        if (didBlockStart(block))
            indent--;
    } else if (indent < 0) {
        QTextBlock previous = block.previous();
        while (indent == -1 && previous.isValid()) {
            indent = previous.userState() >> 8;
            previous = block.previous();
        }
        if (didBlockStart(previous))
            indent++;
        isNewBlock = true;
    }

    if (isNewBlock || !block.text().isEmpty())
        settings.indentLine(block, indent * settings.m_indentSize);
}

}
