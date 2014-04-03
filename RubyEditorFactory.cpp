#include "RubyEditorFactory.h"

#include "RubyEditorConstants.h"
#include "RubyEditorWidget.h"

namespace RubyEditor {

RubyEditorFactory::RubyEditorFactory(QObject* parent)
{
    setId(Constants::EditorId);
    setDisplayName(tr(Constants::EditorDisplayName));
    addMimeType(QLatin1String(Constants::MimeType));
}

Core::IEditor* RubyEditorFactory::createEditor(QWidget* parent)
{
    RubyEditorWidget* widget = new RubyEditorWidget(parent);
    return widget->editor();
}

}

#include "RubyEditorFactory.moc"
