#ifndef Ruby_Indenter_h
#define Ruby_Indenter_h

#include <texteditor/textindenter.h>
#include <QStringList>

namespace Ruby {

class Indenter : public TextEditor::TextIndenter
{
public:
    explicit Indenter(QTextDocument *doc);
    bool isElectricCharacter(const QChar &) const override { return false; }
    void indentBlock(const QTextBlock &block,
                     const QChar &,
                     const TextEditor::TabSettings &settings,
                     int cursorPositionInEditor = -1) override;
};

}

#endif
