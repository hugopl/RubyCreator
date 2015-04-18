#ifndef Ruby_HoverHandler_h
#define Ruby_HoverHandler_h

#include <texteditor/basehoverhandler.h>
#include <texteditor/basetexteditor.h>

namespace Ruby {

class HoverHandler : public TextEditor::BaseHoverHandler
{
private:
    bool acceptEditor(Core::IEditor *editor);
    void identifyMatch(TextEditor::BaseTextEditor* editorWidget, int pos);
};

}

#endif
