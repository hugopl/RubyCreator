#ifndef RubyHighliter_h
#define RubyHighliter_h

#include <texteditor/syntaxhighlighter.h>
#include "RubyScanner.h"

namespace Ruby {

class Highlighter :  public TextEditor::SyntaxHighlighter
{
public:
    Highlighter(TextEditor::BaseTextDocument* parent);
protected:
    virtual void highlightBlock(const QString& text) override;
private:
    int highlightLine(const QString& text, int state);
    QTextCharFormat formatForToken(const Token &);

    QVector<QTextCharFormat> m_formats;
};

}

#endif
