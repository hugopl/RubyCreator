#ifndef Ruby_QuickFixAssistProvider_h
#define Ruby_QuickFixAssistProvider_h

#include <texteditor/codeassist/quickfixassistprovider.h>

namespace Ruby {

class QuickFixAssistProvider : public TextEditor::QuickFixAssistProvider
{
public:
    QuickFixAssistProvider() { }
    IAssistProvider::RunType runType() const Q_DECL_OVERRIDE;
    bool supportsEditor(Core::Id editorId) const Q_DECL_OVERRIDE;
    TextEditor::IAssistProcessor *createProcessor() const Q_DECL_OVERRIDE;

    QList<TextEditor::QuickFixFactory *> quickFixFactories() const Q_DECL_OVERRIDE;
};

}

#endif
