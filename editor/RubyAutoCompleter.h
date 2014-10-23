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
    bool contextAllowsAutoParentheses(const QTextCursor &cursor, const QString &textToInsert) const Q_DECL_OVERRIDE;

    QString insertMatchingBrace(const QTextCursor &cursor, const QString &text, QChar la, int *skippedChars) const Q_DECL_OVERRIDE;
    bool isInComment(const QTextCursor &cursor) const Q_DECL_OVERRIDE;

    int paragraphSeparatorAboutToBeInserted(QTextCursor &cursor, const TextEditor::TabSettings &tabSettings) Q_DECL_OVERRIDE;

};

}

#endif
