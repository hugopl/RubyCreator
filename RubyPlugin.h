#ifndef Ruby_Plugin_h
#define Ruby_Plugin_h

#include <extensionsystem/iplugin.h>

namespace Ruby {

class Plugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "Ruby.json")

public:

    Plugin();
    ~Plugin();

    virtual bool initialize(const QStringList &arguments, QString *errorString) Q_DECL_OVERRIDE;
    virtual void extensionsInitialized() Q_DECL_OVERRIDE;

private:
    void initializeToolsSettings();
};

}

#endif

