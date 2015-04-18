#ifndef Ruby_AmbigousMethodAssistProvider_h
#define Ruby_AmbigousMethodAssistProvider_h

#include <QList>

#include <texteditor/codeassist/iassistprovider.h>

#include "RubySymbol.h"

namespace Ruby {

class AmbigousMethodAssistProvider : public TextEditor::IAssistProvider
{
public:
    bool isAsynchronous() const Q_DECL_OVERRIDE;
    bool supportsEditor(const Core::Id &editorId) const Q_DECL_OVERRIDE;
    TextEditor::IAssistProcessor *createProcessor() const Q_DECL_OVERRIDE;

    void setSymbols(const QList<Symbol> &symbols) { m_symbols = symbols; }
    void setCursorPosition(int cursorPosition) { m_cursorPosition = cursorPosition; }
    void setInNextSplit(bool value) { m_inNexSplit = value; }
private:
    QList<Symbol> m_symbols;
    int m_cursorPosition;
    bool m_inNexSplit;
};

}

#endif
