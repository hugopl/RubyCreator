#ifndef RubyEditorPlugin_h
#define RubyEditorPlugin_h

#include <extensionsystem/iplugin.h>

class RubyEditorPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "RubyEditor.json")

public:

    RubyEditorPlugin();

    virtual bool initialize(const QStringList& arguments, QString* errorString) override;
    virtual void extensionsInitialized() override;
};

#endif

