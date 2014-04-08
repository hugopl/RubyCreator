#include "RubyEditorFactory.h"

#include "RubyConstants.h"
#include "RubyEditorWidget.h"

#include <texteditor/texteditorsettings.h>

namespace Ruby {

EditorFactory::EditorFactory(QObject* parent)
{
    setId(Constants::EditorId);
    setDisplayName(tr(Constants::EditorDisplayName));
    addMimeType(QLatin1String(Constants::MimeType));
}

Core::IEditor* EditorFactory::createEditor(QWidget* parent)
{
    EditorWidget* widget = new EditorWidget(parent);
    TextEditor::TextEditorSettings::initializeEditor(widget);
    return widget->editor();
}

}

#include "RubyEditorFactory.moc"
