#ifndef RubyHighliter_h
#define RubyHighliter_h

#include <texteditor/basetextdocumentlayout.h>
#include <texteditor/syntaxhighlighter.h>
#include "RubyScanner.h"

namespace Ruby {

class Highlighter :  public TextEditor::SyntaxHighlighter
{
public:
    Highlighter(QTextDocument* parent = 0);
protected:
    virtual void highlightBlock(const QString& text) override;
private:
    int highlightLine(const QString& text, int state);
    QTextCharFormat formatForToken(const Token &);

    QVector<QTextCharFormat> m_formats;

    typedef TextEditor::Parenthesis Parenthesis;
    typedef TextEditor::Parentheses Parentheses;

    Parentheses m_currentBlockParentheses;
};

}

#endif
