#ifndef RubyEditorPlugin_h
#define RubyEditorPlugin_h

#include <extensionsystem/iplugin.h>

namespace RubyEditor {

class RubyEditorFactory;

class RubyEditorPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "RubyEditor.json")

public:

    RubyEditorPlugin();
    ~RubyEditorPlugin();

    virtual bool initialize(const QStringList& arguments, QString* errorString) override;
    virtual void extensionsInitialized() override;

private:
    RubyEditorFactory* m_factory;
};

}

#endif

