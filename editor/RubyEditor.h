#ifndef Ruby_Editor_h
#define Ruby_Editor_h

#include <texteditor/texteditorplugin.h>
#include <QTimer>

namespace Ruby {

class EditorWidget;

class Editor : public TextEditor::BaseTextEditor
{
    Q_OBJECT

public:
    Editor();
};

}

#endif
