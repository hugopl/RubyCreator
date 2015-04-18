#include "RubyEditor.h"

#include "../RubyConstants.h"
#include "RubyEditorWidget.h"

#include <extensionsystem/pluginmanager.h>
#include <texteditor/texteditorconstants.h>

#include <QBuffer>

namespace Ruby {

Editor::Editor(TextEditor::BaseTextEditorWidget* editorWidget) : BaseTextEditor(editorWidget)
{
    //addContext(Constants::LangRuby);
}

}
