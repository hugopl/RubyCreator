#ifndef Ruby_Plugin_h
#define Ruby_Plugin_h

#include <extensionsystem/iplugin.h>
#include "editor/RubyQuickFixAssistProvider.h"

namespace Ruby {

class Plugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "Ruby.json")

public:
    Plugin();
    ~Plugin();

    static Plugin *instance();

    virtual bool initialize(const QStringList &arguments, QString *errorString) override;
    virtual void extensionsInitialized() override;
    QuickFixAssistProvider* quickFixProvider();

private:
    void initializeToolsSettings();

    static Plugin* m_instance;
    QuickFixAssistProvider* m_quickFixProvider;

#ifdef WITH_TESTS
private slots:
    void cleanupTestCase();

    void test_context();
    void test_indentIf();
    void test_indentBraces();
    void test_lineCount();
    void test_ifs();
    void test_scanner();
    void test_scanner_data();
#endif
};

}

#endif
