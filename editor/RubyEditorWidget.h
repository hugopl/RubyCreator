#ifndef RubyEditorWidget_h
#define RubyEditorWidget_h

#include <texteditor/basetexteditor.h>

namespace Ruby {

class EditorWidget : public TextEditor::BaseTextEditorWidget
{
    Q_OBJECT
public:
    EditorWidget();
    TextEditor::BaseTextEditor* createEditor() override;

    Link findLinkAt(const QTextCursor& cursor, bool, bool inNextSplit) override;
private:
    QRegExp m_wordRegex;
};

}

#endif
