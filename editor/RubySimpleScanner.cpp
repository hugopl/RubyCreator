#include "RubySimpleScanner.h"
#include <array>
#include <QFile>
#include <QDebug>

namespace Ruby {

namespace Language {
QRegExp symbolDefinition("^\\s*\\b(def|class|module)\\b\\s*<{0,2}\\s*([+:\\w\?!><=/-]+)");
QRegExp startOfBlock("^[^#]*\\b(begin|case|do|if|while|until)\\b");
QRegExp endKeyword("^[^#]*\\bend\\b");
}

using namespace Language;

SimpleScanner::SimpleScanner(QIODevice* device)
    : m_src(device)
    , m_lineCount(0)
    , m_contextDepth(0)
{
}

Symbol SimpleScanner::nextSymbol()
{
    // TODO:
    // - Skip multiline string.
    // - Don't consider keyword when it' on strings.

    QString line;

    Symbol symbol;

    do {
        line = m_src.readLine();
        m_lineCount++;

        if (line.startsWith("#"))
            continue;

        if (startOfBlock.indexIn(line) != -1) {
            m_contextDepth++;
            continue;
        } else if (symbolDefinition.indexIn(line) != -1) {
            m_contextDepth++;

            QString keyword = symbolDefinition.cap(1);
            QString symbolName = symbolDefinition.cap(2);

            // Avoid class << self, etc.
            if (keyword[0] == 'c' && symbolName[0].isLower())
                continue;

            if (keyword[0] == 'c' || keyword[0] == 'm') {
                m_context = symbolName;
                // Disable class and modules while there's no locator for them.
                continue;
            }

            symbol.name = symbolName;
            symbol.line = m_lineCount;
            symbol.column = line.indexOf(symbolName);
            symbol.context = m_context;
        } else if (endKeyword.indexIn(line) != -1) {
            m_contextDepth--;
            if (!m_contextDepth)
                m_context.clear();
        }
    } while (symbol.name.isNull() && !m_src.atEnd());

    return symbol;
}

}
