#ifndef Ruby_AutoCompleter_h
#define Ruby_AutoCompleter_h

#include <texteditor/autocompleter.h>

namespace TextEditor {
class TabSettings;
}

namespace Ruby {

class AutoCompleter : public TextEditor::AutoCompleter
{
public:
    bool contextAllowsAutoParentheses(const QTextCursor &cursor, const QString &textToInsert) const override;

    QString insertMatchingBrace(const QTextCursor &cursor, const QString &text, QChar la, int *skippedChars) const override;
    bool isInComment(const QTextCursor &cursor) const override;

    int paragraphSeparatorAboutToBeInserted(QTextCursor &cursor, const TextEditor::TabSettings &tabSettings) override;

};

}

#endif
