#ifndef Ruby_QuickFixes_h
#define Ruby_QuickFixes_h

#include <QTextBlock>

#include <extensionsystem/iplugin.h>
#include <texteditor/quickfix.h>

#include "RubyScanner.h"

namespace Ruby {

void registerQuickFixes(ExtensionSystem::IPlugin *plugIn);

class QuickFixFactory : public QObject
{
    Q_OBJECT

public:
    QuickFixFactory();
    ~QuickFixFactory();
    virtual void match(const TextEditor::QuickFixInterface &interface,
                       TextEditor::QuickFixOperations &result) = 0;

    static const QList<QuickFixFactory *> &quickFixFactories();
};

class SwitchStringQuotes : public QuickFixFactory {
public:
    void match(const TextEditor::QuickFixInterface &interface,
               TextEditor::QuickFixOperations &result) override;
};

class SwitchStringQuotesOp : public TextEditor::QuickFixOperation {
public:
    SwitchStringQuotesOp(QTextBlock &block, const Token &token, int userCursorPosition);
    void perform() override;
private:
    QTextBlock m_block;
    Token m_token;
    int m_userCursorPosition;
};
}

#endif
