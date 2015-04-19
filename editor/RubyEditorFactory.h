#ifndef RubyEditorFactory_h
#define RubyEditorFactory_h

#include <coreplugin/editormanager/ieditorfactory.h>
#include <coreplugin/idocument.h>
#include <coreplugin/editormanager/ieditor.h>

namespace Ruby {

class EditorFactory : public Core::IEditorFactory
{
    Q_OBJECT

public:
    EditorFactory();
    Core::IEditor *createEditor() { return NULL; }
    Core::IDocument *open(const QString &) { return NULL; }
};

}

#endif
