#include "RubyEditorDocument.h"
#include "../RubyConstants.h"
#include "../RubyPlugin.h"

namespace Ruby
{

EditorDocument::EditorDocument()
{
    setId(Constants::EditorId);
}

TextEditor::QuickFixAssistProvider *EditorDocument::quickFixAssistProvider() const
{
    return Plugin::instance()->quickFixProvider();
}

}
