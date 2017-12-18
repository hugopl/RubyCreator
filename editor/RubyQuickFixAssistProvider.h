#ifndef Ruby_QuickFixAssistProvider_h
#define Ruby_QuickFixAssistProvider_h

#include <texteditor/codeassist/iassistprovider.h>

namespace Ruby {

class QuickFixAssistProvider : public TextEditor::IAssistProvider
{
public:
    QuickFixAssistProvider(QObject *parent);
    IAssistProvider::RunType runType() const override;
    TextEditor::IAssistProcessor *createProcessor() const override;
};

}

#endif
