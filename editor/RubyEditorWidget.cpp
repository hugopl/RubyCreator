#include "RubyEditorWidget.h"

#include "RubyAmbiguousMethodAssistProvider.h"
#include "RubyAutoCompleter.h"
#include "RubyCodeModel.h"
#include "../RubyConstants.h"
#include "RubyHighlighter.h"
#include "RubyIndenter.h"
#include "RubyRubocopHighlighter.h"

#include <texteditor/textdocument.h>

#include <QDebug>
#include <QTextBlock>
#include <QTextCursor>

using Utils::Link;

namespace Ruby {

const int CODEMODEL_UPDATE_INTERVAL = 150;
const int RUBOCOP_UPDATE_INTERVAL = 300;

EditorWidget::EditorWidget()
    : m_wordRegex("[\\w!\\?]+")
    , m_ambigousMethodAssistProvider(new AmbigousMethodAssistProvider)
{
    setLanguageSettingsId(Constants::SettingsId);

    m_commentDefinition.multiLineStart.clear();
    m_commentDefinition.multiLineEnd.clear();
    m_commentDefinition.singleLine = '#';

    m_updateCodeModelTimer.setSingleShot(true);
    m_updateCodeModelTimer.setInterval(CODEMODEL_UPDATE_INTERVAL);
    connect(&m_updateCodeModelTimer, &QTimer::timeout, this, [this] {
        if (m_codeModelUpdatePending)
            updateCodeModel();
    });

    m_updateRubocopTimer.setSingleShot(true);
    m_updateRubocopTimer.setInterval(RUBOCOP_UPDATE_INTERVAL);
    connect(&m_updateRubocopTimer, &QTimer::timeout, this, [this] {
        if (m_rubocopUpdatePending)
            updateRubocop();
    });

    CodeModel::instance();
}

EditorWidget::~EditorWidget()
{
    delete m_ambigousMethodAssistProvider;
}

void EditorWidget::findLinkAt(const QTextCursor &cursor,
                              Utils::ProcessLinkCallback &&processLinkCallback,
                              bool resolveTarget,
                              bool inNextSplit)
{
    Q_UNUSED(resolveTarget);
    QString text = cursor.block().text();
    if (text.isEmpty()) {
        processLinkCallback(Utils::Link());
        return;
    }

    QString word;
    int cursorPos = cursor.positionInBlock();
    int pos = 0;
    for (;;) {
        QRegularExpressionMatch match = m_wordRegex.match(text, pos + word.length());
        if (!match.hasMatch()) {
            processLinkCallback(Utils::Link());
            return;
        }

        word = match.captured();
        pos = match.capturedStart();
        if (pos <= cursorPos && (pos + word.length()) >= cursorPos)
            break;
    }

    if (word.isEmpty() || word[0].isDigit()) {
        processLinkCallback(Utils::Link());
        return;
    }

    CodeModel* codeModel = CodeModel::instance();

    const QList<Symbol> symbols = word[0].isUpper() ? codeModel->allClassesAndConstantsNamed(word) : codeModel->allMethodsNamed(word);
    if (symbols.empty()) {
        processLinkCallback(Utils::Link());
        return;
    }

    Link link;
    link.linkTextStart = cursor.position() + (pos - cursorPos);
    link.linkTextEnd = link.linkTextStart + word.length();

    if (symbols.count() > 1) {
        m_ambigousMethodAssistProvider->setSymbols(symbols);
        m_ambigousMethodAssistProvider->setCursorPosition(cursor.position());
        m_ambigousMethodAssistProvider->setInNextSplit(inNextSplit);

        invokeAssist(TextEditor::FollowSymbol, m_ambigousMethodAssistProvider);
        processLinkCallback(link);
        return;
    }

    link.targetLine = symbols.last().line;
    link.targetColumn = symbols.last().column;
    link.targetFileName = *symbols.last().file;

    processLinkCallback(link);
}

void EditorWidget::unCommentSelection()
{
    Utils::unCommentSelection(this, m_commentDefinition);
}

void EditorWidget::aboutToOpen(const QString &fileName, const QString &realFileName)
{
    Q_UNUSED(fileName);
    m_filePathDueToMaybeABug = realFileName;
}

void EditorWidget::scheduleCodeModelUpdate()
{
    m_codeModelUpdatePending = m_updateCodeModelTimer.isActive();
    if (m_codeModelUpdatePending)
        return;

    m_codeModelUpdatePending = false;
    updateCodeModel();
    m_updateCodeModelTimer.start();
}

void EditorWidget::updateCodeModel()
{
    const QString textData = textDocument()->plainText();
    CodeModel::instance()->updateFile(textDocument()->filePath().toString(), textData);
}

void EditorWidget::scheduleRubocopUpdate()
{
    m_rubocopUpdatePending = m_updateRubocopTimer.isActive();
    if (m_rubocopUpdatePending)
        return;

    m_rubocopUpdatePending = false;
    updateRubocop();
    m_updateRubocopTimer.start();
}

void EditorWidget::updateRubocop()
{
    if (!RubocopHighlighter::instance()->run(textDocument(), m_filePathDueToMaybeABug)) {
        m_rubocopUpdatePending = true;
        m_updateRubocopTimer.start();
    }
}

void EditorWidget::finalizeInitialization()
{
    connect(document(), &QTextDocument::contentsChanged, this, &EditorWidget::scheduleCodeModelUpdate);
    connect(document(), &QTextDocument::contentsChanged, this, &EditorWidget::scheduleRubocopUpdate);
}

}
