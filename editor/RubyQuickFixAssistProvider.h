#ifndef Ruby_QuickFixAssistProvider_h
#define Ruby_QuickFixAssistProvider_h

#include <texteditor/codeassist/quickfixassistprovider.h>

namespace Ruby {

class QuickFixAssistProvider : public TextEditor::QuickFixAssistProvider
{
public:
    QuickFixAssistProvider(QObject *parent);
    IAssistProvider::RunType runType() const override;
    bool supportsEditor(Core::Id editorId) const override;
    TextEditor::IAssistProcessor *createProcessor() const override;

    QList<TextEditor::QuickFixFactory *> quickFixFactories() const override;
};

}

#endif
