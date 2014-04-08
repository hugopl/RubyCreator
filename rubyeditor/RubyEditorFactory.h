#ifndef RubyEditorFactory_h
#define RubyEditorFactory_h

#include <coreplugin/editormanager/ieditorfactory.h>

namespace RubyEditor {

class RubyEditorFactory : public Core::IEditorFactory
{
    Q_OBJECT
public:
    RubyEditorFactory(QObject* parent);
    Core::IEditor* createEditor(QWidget *parent) override;
};

}

#endif
