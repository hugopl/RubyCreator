#ifndef Ruby_QuickFixes_h
#define Ruby_QuickFixes_h

#include <QTextBlock>

#include <extensionsystem/iplugin.h>
#include <texteditor/quickfix.h>

#include "RubyScanner.h"

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
    SwitchStringQuotesOp(QTextBlock &block, const Token &token, int userCursorPosition);
    void perform() Q_DECL_OVERRIDE;
private:
    QTextBlock m_block;
    Token m_token;
    int m_userCursorPosition;
};
}

#endif
