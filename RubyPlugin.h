#ifndef Ruby_Plugin_h
#define Ruby_Plugin_h

#include <extensionsystem/iplugin.h>

namespace Ruby {

class EditorFactory;

class Plugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "RubySupportPlugin.json")

public:

    Plugin();
    ~Plugin();

    virtual bool initialize(const QStringList& arguments, QString* errorString) override;
    virtual void extensionsInitialized() override;

private:
    EditorFactory* m_factory;
};

}

#endif

