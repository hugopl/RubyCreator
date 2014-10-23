#ifndef Ruby_Indenter_h
#define Ruby_Indenter_h

#include <texteditor/indenter.h>
#include <QStringList>

namespace Ruby {

class Indenter : public TextEditor::Indenter
{
public:
    bool isElectricCharacter(const QChar &) const Q_DECL_OVERRIDE { return false; }
    void indentBlock(QTextDocument*, const QTextBlock &block, const QChar &, const TextEditor::TabSettings &settings) Q_DECL_OVERRIDE;
};

}

#endif // PYTHONINDENTER_H
