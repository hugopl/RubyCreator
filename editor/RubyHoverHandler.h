#ifndef Ruby_HoverHandler_h
#define Ruby_HoverHandler_h

#include <texteditor/basehoverhandler.h>

namespace Ruby {

class HoverHandler : public TextEditor::BaseHoverHandler
{
private:
    void identifyMatch(TextEditor::TextEditorWidget *editorWidget,
                       int pos, ReportPriority priority) override;
};

}

#endif
