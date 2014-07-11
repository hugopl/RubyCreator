#ifndef Ruby_Indenter_h
#define Ruby_Indenter_h

#include <texteditor/indenter.h>
#include <QStringList>

namespace Ruby {

class Indenter : public TextEditor::Indenter
{
public:
    bool isElectricCharacter(const QChar&) const override { return false; }
    void indentBlock(QTextDocument*, const QTextBlock& block, const QChar&, const TextEditor::TabSettings& settings) override;
};

}

#endif // PYTHONINDENTER_H
