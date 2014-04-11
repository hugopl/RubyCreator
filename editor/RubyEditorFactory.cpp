#include "RubyEditorFactory.h"

#include "RubyConstants.h"
#include "RubyEditorWidget.h"

#include <texteditor/texteditorsettings.h>

namespace Ruby {

EditorFactory::EditorFactory(QObject* parent)
    : Core::IEditorFactory(parent)
{
    setId(Constants::EditorId);
    setDisplayName(tr(Constants::EditorDisplayName));
    addMimeType(QLatin1String(Constants::MimeType));
}

Core::IEditor* EditorFactory::createEditor()
{
    EditorWidget* widget = new EditorWidget;
    TextEditor::TextEditorSettings::initializeEditor(widget);
    return widget->editor();
}

}

#include "RubyEditorFactory.moc"
