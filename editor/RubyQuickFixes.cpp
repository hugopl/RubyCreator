#include "RubyQuickFixes.h"

#include <QTextDocument>
#include <texteditor/codeassist/assistinterface.h>

#include "RubyScanner.h"

namespace Ruby {

static QList<QuickFixFactory *> g_quickFixFactories;

QuickFixFactory::QuickFixFactory()
{
    g_quickFixFactories.append(this);
}

QuickFixFactory::~QuickFixFactory()
{
    g_quickFixFactories.removeOne(this);
}

const QList<QuickFixFactory *> &QuickFixFactory::quickFixFactories()
{
    return g_quickFixFactories;
}

void registerQuickFixes(ExtensionSystem::IPlugin *plugIn)
{
    plugIn->addAutoReleasedObject(new SwitchStringQuotes);
}

void SwitchStringQuotes::match(const TextEditor::QuickFixInterface &interface,
                               TextEditor::QuickFixOperations &result)
{
    QTextBlock block = interface->textDocument()->findBlock(interface->position());
    QString line = block.text();
    int userCursorPosition = interface->position();
    int position = userCursorPosition - block.position();
    Token token = Scanner::tokenAt(&line, position);

    if (token.kind != Ruby::Token::String)
        return;

    SwitchStringQuotesOp* operation = new SwitchStringQuotesOp(block, token, userCursorPosition);
    QString description;
    if (line[token.position] == '"')
        description = tr("Convert to single quotes");
    else
        description = tr("Convert to double quotes");
    operation->setDescription(description);

    result << operation;
}

SwitchStringQuotesOp::SwitchStringQuotesOp(QTextBlock &block, const Token &token, int userCursorPosition)
    : m_block(block), m_token(token), m_userCursorPosition(userCursorPosition)
{
}

void SwitchStringQuotesOp::perform()
{
    QString string = m_block.text().mid(m_token.position, m_token.length);

    QString oldQuote = "\"";
    QString newQuote = "'";
    if (string[0] != '"')
        std::swap(oldQuote, newQuote);

    string.replace("\\" + oldQuote, oldQuote);
    string.replace(newQuote, "\\" + newQuote);
    string[0] = newQuote[0];
    string[string.length() - 1] = newQuote[0];

    QTextCursor cursor(m_block);
    cursor.beginEditBlock();
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, m_token.position);
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, m_token.length);
    cursor.removeSelectedText();
    cursor.insertText(string);
    cursor.endEditBlock();
}

}
