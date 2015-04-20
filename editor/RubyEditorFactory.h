#ifndef RubyEditorFactory_h
#define RubyEditorFactory_h

#include <coreplugin/editormanager/ieditorfactory.h>

namespace Ruby {

class EditorFactory : public Core::IEditorFactory
{
    Q_OBJECT
public:
    EditorFactory(QObject* parent);
    Core::IEditor* createEditor() Q_DECL_OVERRIDE;
};

}

#endif
