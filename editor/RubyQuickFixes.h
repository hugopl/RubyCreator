#ifndef Ruby_QuickFixes_h
#define Ruby_QuickFixes_h

#include <QTextDocument>

#include <extensionsystem/iplugin.h>
#include <texteditor/quickfix.h>

namespace Ruby {

void registerQuickFixes(ExtensionSystem::IPlugin *plugIn);

class QuickFixFactory : public TextEditor::QuickFixFactory {
    Q_OBJECT
};

class SwitchStringQuotes : public QuickFixFactory {
public:
    void matchingOperations(const TextEditor::QuickFixInterface &interface, TextEditor::QuickFixOperations &result) Q_DECL_OVERRIDE;
};

class SwitchStringQuotesOp : public TextEditor::QuickFixOperation {
public:
    SwitchStringQuotesOp(QTextDocument* document, int position, int length);
    void perform() Q_DECL_OVERRIDE;
private:
    QTextDocument *m_document;
    int m_position;
    int m_length;
};
}

#endif
