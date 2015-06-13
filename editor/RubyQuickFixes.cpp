#include "RubyQuickFixes.h"

#include <QTextBlock>
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
    int position = interface->position() - block.position();
    Token token = Scanner::tokenAt(&line, position);

    if (token.kind != Ruby::Token::String)
        return;

    SwitchStringQuotesOp* operation = new SwitchStringQuotesOp(interface->textDocument(), token.position + block.position(), token.length);
    QString description;
    if (line[token.position] == QLatin1Char('"'))
        description = QStringLiteral("Convert to single quotes");
    else
        description = QStringLiteral("Convert to double quotes");
    operation->setDescription(description);

    result.append(operation);
}

SwitchStringQuotesOp::SwitchStringQuotesOp(QTextDocument* document, int position, int length)
    : m_document(document), m_position(position), m_length(length)
{
}

void SwitchStringQuotesOp::perform()
{
    QTextBlock block = m_document->findBlock(m_position);
    QString line = block.text();
    int linePos = m_position - block.position();
    QString string = line.mid(linePos, m_length);

    QString oldQuote = QStringLiteral("\"");
    QString newQuote = QStringLiteral("'");
    if (string[0] != QLatin1Char('"'))
        std::swap(oldQuote, newQuote);

    string.replace(QLatin1String("\\") + oldQuote, oldQuote);
    string.replace(newQuote, QLatin1String("\\") + newQuote);
    string[0] = newQuote[0];
    string[string.length() - 1] = newQuote[0];

    QTextCursor cursor(block);
    cursor.beginEditBlock();
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, linePos);
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, m_length);
    cursor.removeSelectedText();
    cursor.insertText(string);
    cursor.endEditBlock();
}


}
