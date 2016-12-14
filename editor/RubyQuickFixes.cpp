#include "RubyQuickFixes.h"

#include <QTextDocument>
#include <texteditor/codeassist/assistinterface.h>
#include <algorithm>

#include "RubyScanner.h"

namespace Ruby {
void registerQuickFixes(ExtensionSystem::IPlugin *plugIn)
{
    plugIn->addAutoReleasedObject(new SwitchStringQuotes);
}

void SwitchStringQuotes::matchingOperations(const TextEditor::QuickFixInterface &interface, TextEditor::QuickFixOperations &result)
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
    if (line[token.position] == QLatin1Char('"'))
        description = QStringLiteral("Convert to single quotes");
    else
        description = QStringLiteral("Convert to double quotes");
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

    QString oldQuote = QStringLiteral("\"");
    QString newQuote = QStringLiteral("'");
    if (string[0] != QLatin1Char('"'))
        std::swap(oldQuote, newQuote);

    string.replace(QLatin1String("\\") + oldQuote, oldQuote);
    string.replace(newQuote, QLatin1String("\\") + newQuote);
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
