#ifndef RubyEditor_h
#define RubyEditor_h

#include <texteditor/texteditor.h>

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
