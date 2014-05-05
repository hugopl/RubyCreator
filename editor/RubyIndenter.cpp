#include "RubyIndenter.h"

#include <texteditor/tabsettings.h>
#include <QRegExp>
#include <QSet>
#include <QDebug>

namespace Ruby {

static const int TAB_SIZE = 4;

void Indenter::indentBlock(QTextDocument*, const QTextBlock& block, const QChar&, const TextEditor::TabSettings& settings)
{
    QTextBlock previous = block.previous();
    QString previousLine = previous.text();
    while (previousLine.trimmed().isEmpty()) {
        previous = previous.previous();
        if (!previous.isValid())
            return;
        previousLine = previous.text();
    }

    int indentation = settings.indentationColumn(previousLine);

    if (isElectricLine(previousLine))
        indentation += TAB_SIZE;

    settings.indentLine(block, indentation);
}

bool Indenter::isElectricLine(const QString& line) const
{
    static QRegExp r("^\\s*(if|def|class|module)");
    if (line.isEmpty())
        return false;

    return r.indexIn(line) != -1;
}

} // namespace Ruby
