#include "RubyEditorWidget.h"

#include "RubyAmbiguousMethodAssistProvider.h"
#include "RubyAutoCompleter.h"
#include "RubyCodeModel.h"
#include "../RubyConstants.h"
#include "RubyHighlighter.h"
#include "RubyIndenter.h"

#include <texteditor/textdocument.h>

#include <QDebug>
#include <QTextBlock>
#include <QTextCursor>

namespace Ruby {

const int UpdateDocumentDefaultInterval = 150;

EditorWidget::EditorWidget()
    : m_wordRegex(QLatin1String("[\\w!\\?]+"))
    , m_codeModelUpdatePending(false)
    , m_ambigousMethodAssistProvider(new AmbigousMethodAssistProvider)
{
    setLanguageSettingsId(Constants::SettingsId);

    m_commentDefinition.multiLineStart.clear();
    m_commentDefinition.multiLineEnd.clear();
    m_commentDefinition.singleLine = QLatin1Char('#');

    m_updateCodeModelTimer.setSingleShot(true);
    m_updateCodeModelTimer.setInterval(UpdateDocumentDefaultInterval);
    connect(&m_updateCodeModelTimer, &QTimer::timeout, this, &EditorWidget::updateCodeModel);

    CodeModel::instance();
}

EditorWidget::~EditorWidget()
{
    delete m_ambigousMethodAssistProvider;
}

TextEditor::TextEditorWidget::Link EditorWidget::findLinkAt(const QTextCursor &cursor, bool, bool inNextSplit)
{
    QString text = cursor.block().text();
    if (text.isEmpty())
        return Link();

    QString word;
    int cursorPos = cursor.positionInBlock();
    int pos = 0;
    forever {
        pos = m_wordRegex.indexIn(text, pos + word.length());
        if (pos == -1)
            return Link();

        word = m_wordRegex.cap();
        if (pos <= cursorPos && (pos + word.length()) >= cursorPos)
            break;
    }

    const QList<Symbol> symbols = CodeModel::instance()->allMethodsNamed(word);
    if (symbols.empty())
        return Link();

    Link link;
    link.linkTextStart = cursor.position() + (pos - cursorPos);
    link.linkTextEnd = link.linkTextStart + word.length();

    if (symbols.count() > 1) {
        m_ambigousMethodAssistProvider->setSymbols(symbols);
        m_ambigousMethodAssistProvider->setCursorPosition(cursor.position());
        m_ambigousMethodAssistProvider->setInNextSplit(inNextSplit);

        invokeAssist(TextEditor::FollowSymbol, m_ambigousMethodAssistProvider);
        return link;
    }

    link.targetLine = symbols.last().line;
    link.targetColumn = symbols.last().column;
    link.targetFileName = *symbols.last().file;

    return link;
}

void EditorWidget::unCommentSelection()
{
    Utils::unCommentSelection(this, m_commentDefinition);
}

void EditorWidget::scheduleCodeModelUpdate()
{
    m_codeModelUpdatePending = m_updateCodeModelTimer.isActive();
    if (m_codeModelUpdatePending)
        return;

    updateCodeModel();
    m_codeModelUpdatePending = false;
    m_updateCodeModelTimer.start();
}

void EditorWidget::updateCodeModel()
{
    if (!m_codeModelUpdatePending)
        return;

    const QString textData = textDocument()->plainText();
    CodeModel::instance()->updateFile(textDocument()->filePath(), textData);
}

void EditorWidget::finalizeInitialization()
{
    connect(document(), SIGNAL(contentsChanged()), this, SLOT(scheduleCodeModelUpdate()));
}

}
