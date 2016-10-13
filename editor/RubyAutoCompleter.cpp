#include "RubyAutoCompleter.h"
#include "RubyIndenter.h"

#include <QTextBlock>
#include <QTextCursor>

namespace Ruby {

bool AutoCompleter::contextAllowsAutoQuotes(const QTextCursor &cursor, const QString &textToInsert) const
{
    if (isInComment(cursor))
        return false;

    QChar ch;

    if (!textToInsert.isEmpty())
        ch = textToInsert.at(0);

    switch (ch.unicode()) {
    case '"':
    case '\'':
        return true;

    default:
        return false;
    }
}

bool AutoCompleter::contextAllowsAutoBrackets(const QTextCursor &cursor, const QString &textToInsert) const
{
    if (isInComment(cursor))
        return false;

    QChar ch;

    if (!textToInsert.isEmpty())
        ch = textToInsert.at(0);

    switch (ch.unicode()) {
    case '(':
    case '[':
    case '{':
    case ')':
    case ']':
    case '}':
        return true;

    default:
        return false;
    }
}

QString AutoCompleter::insertMatchingBrace(const QTextCursor &, const QString &text, QChar la, bool skipChars, int *skippedChars) const
{
    if (text.length() != 1)
        return QString();

    const QChar ch = text.at(0);
    switch (ch.unicode()) {
    case '(':
        return QStringLiteral(")");

    case '[':
        return QStringLiteral("]");

    case '{':
        return QStringLiteral("}");

    case ')':
    case ']':
    case '}':
    case ';':
        if (skipChars && la == ch)
            ++*skippedChars;
        break;

    default:
        break;
    } // end of switch

    return QString();
}

QString AutoCompleter::insertMatchingQuote(const QTextCursor &, const QString &text, QChar la, bool skipChars, int *skippedChars) const
{
    if (text.length() != 1)
        return QString();

    const QChar ch = text.at(0);
    switch (ch.unicode()) {
    case '\'':
        if (la != ch)
            return QString(ch);
        if (skipChars)
            ++*skippedChars;
        break;

    case '"':
        if (la != ch)
            return QString(ch);
        if (skipChars)
            ++*skippedChars;
        break;

    case ';':
        if (skipChars && la == ch)
            ++*skippedChars;
        break;

    default:
        break;
    } // end of switch

    return QString();
}

bool AutoCompleter::isInComment(const QTextCursor &cursor) const
{
    QString line = cursor.block().text();
    int hashIndex = line.indexOf(QLatin1Char('#'));
    if (hashIndex == -1 || hashIndex < cursor.columnNumber())
        return false;
    return true;
}

int AutoCompleter::paragraphSeparatorAboutToBeInserted(QTextCursor &cursor, const TextEditor::TabSettings &tabSettings)
{
    QTextBlock block = cursor.block();
    QString text = block.text().trimmed();
    if (text == QLatin1String("end")
            || text == QLatin1String("else")
            || text.startsWith(QLatin1String("elsif"))
            || text.startsWith(QLatin1String("rescue"))
            || text == QLatin1String("ensure")) {
        Indenter indenter;
        indenter.indentBlock(const_cast<QTextDocument*>(block.document()), block, QChar(), tabSettings);
    }

    // This implementation is buggy
#if 0
    const QString textFromCursor = text.mid(cursor.positionInBlock()).trimmed();
    if (!textFromCursor.isEmpty())
        return 0;

    if (Language::symbolDefinition.indexIn(text) == -1
            && Language::startOfBlock.indexIn(text) == -1) {
        return 0;
    }

    int spaces = 0;
    foreach (const QChar c, text) {
        if (!c.isSpace())
            break;
        spaces++;
    }
    QString indent = text.left(spaces);

    QString line;
    QTextBlock nextBlock = block.next();
    while (nextBlock.isValid()) {
        line = nextBlock.text();
        if (Language::endKeyword.indexIn(line) != -1 && line.startsWith(indent))
            return 0;
        if (!line.trimmed().isEmpty())
            break;
        nextBlock = nextBlock.next();
    }

    int pos = cursor.position();
    cursor.insertBlock();
    cursor.insertText(QLatin1String("end"));
    cursor.setPosition(pos);

    return 1;
#endif
    return 0;
}

}
