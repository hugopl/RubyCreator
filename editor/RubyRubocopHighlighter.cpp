#include "RubyRubocopHighlighter.h"

#include <texteditor/textdocument.h>
#include <texteditor/textmark.h>
#include <texteditor/semantichighlighter.h>

#include <QDebug>
#include <QLoggingCategory>
#include <QProcess>
#include <QTextDocument>
#include <QtConcurrent>
#include <QMessageBox>
#include <QTextBlock>

Q_LOGGING_CATEGORY(log, "qtc.ruby.rubocop");

namespace Ruby {

static RubocopHighlighter *theInstance = nullptr;

class RubocopFuture : public QFutureInterface<TextEditor::HighlightingResult>, public QObject
{
public:
    RubocopFuture(const Offenses &offenses)
    {
        reportResults(offenses);
    }
};

class TextMark : public TextEditor::TextMark
{
public:
    using RemovedFromEditorHandler = std::function<void(TextMark *)>;

    TextMark(const QString &fileName,
             int line,
             const QString &text,
             const RemovedFromEditorHandler &removedHandler)
        : TextEditor::TextMark(fileName, line, "Rubocop")
        , m_removedFromEditorHandler(removedHandler)
    {
        setColor(Utils::Theme::ClangCodeModel_Warning_TextMarkColor);
        setDefaultToolTip(QCoreApplication::translate("Rubocop", "Rubocop Warning"));
        setPriority(TextEditor::TextMark::NormalPriority);
        setLineAnnotation(text);
    }

private:
    void removedFromEditor() override;

    RemovedFromEditorHandler m_removedFromEditorHandler;
};

void TextMark::removedFromEditor()
{
    m_removedFromEditorHandler(this);
}

RubocopHighlighter::RubocopHighlighter()
{
    theInstance = this;
    QTextCharFormat format;
    format.setUnderlineColor(Qt::darkYellow);
    format.setUnderlineStyle(QTextCharFormat::WaveUnderline);
    m_extraFormats[0] = format;
    m_extraFormats[1] = format;
    m_extraFormats[1].setUnderlineColor(Qt::darkGreen);
    m_extraFormats[2] = format;
    m_extraFormats[2].setUnderlineColor(Qt::red);

    initRubocopProcess();
}

RubocopHighlighter::~RubocopHighlighter()
{
    clearTextMarks();
    m_rubocop->closeWriteChannel();
    m_rubocop->waitForFinished(3000);
    delete m_rubocop;
}

RubocopHighlighter *RubocopHighlighter::instance()
{
    return theInstance;
}

// return false if we are busy, true if everything is ok (or rubocop wasn't found)
bool RubocopHighlighter::run(TextEditor::TextDocument *document, const QString &fileNameTip)
{
    if (m_busy || m_rubocop->state() == QProcess::Starting)
        return false;
    if (!m_rubocopFound)
        return true;

    m_busy = true;
    m_startRevision = document->document()->revision();

    m_timer.start();
    m_document = document;

    const QString filePath = document->filePath().isEmpty() ? fileNameTip
                                                            : document->filePath().toString();
    m_rubocop->write(filePath.toUtf8());
    m_rubocop->write("\n");
    QByteArray data = document->plainText().toUtf8();
    m_rubocop->write(data.constData(), data.length() + 1);
    return true;
}

QString RubocopHighlighter::diagnosticAt(const Utils::FileName &file, int pos)
{
    auto it = m_diagnostics.find(file);
    if (it == m_diagnostics.end())
        return QString();

    return it->messages[Range(pos + 1, 0)];
}

void RubocopHighlighter::clearTextMarks()
{
    for (TextMark *textMark : m_textMarks) {
        m_document->removeMark(textMark);
        delete textMark;
    }
    m_textMarks.clear();
}

void RubocopHighlighter::initRubocopProcess()
{
    if (m_rubocopScript.open()) {
        QFile script(":/rubysupport/rubocop.rb");
        script.open(QFile::ReadOnly);
        m_rubocopScript.write(script.readAll());
        m_rubocopScript.close();
    }

    m_rubocop = new QProcess;
    void (QProcess::*signal)(int) = &QProcess::finished;
    QObject::connect(m_rubocop, signal, [&](int status) {
        if (status) {
            QMessageBox::critical(0, "Rubocop", QString::fromUtf8(m_rubocop->readAllStandardError().trimmed()));
            m_rubocopFound = false;
        }
    });

    QObject::connect(m_rubocop, &QProcess::readyReadStandardOutput, [&]() {
        m_outputBuffer.append(QString::fromUtf8(m_rubocop->readAllStandardOutput()));
        if (m_outputBuffer.endsWith("--\n"))
            finishRuboCopHighlight();
    });

    m_rubocop->start("ruby", {m_rubocopScript.fileName()});
}

void RubocopHighlighter::finishRuboCopHighlight()
{
    if (m_startRevision != m_document->document()->revision()) {
        m_busy = false;
        return;
    }

    Offenses offenses = processRubocopOutput();
    const Utils::FileName filePath = m_document->filePath();
    Diagnostics &diag = m_diagnostics[filePath];
    QMapIterator<Range, QString> it(diag.messages);
    while (it.hasNext()) {
        it.next();
        const auto onMarkRemoved = [this](const TextMark *mark) {
            const auto it = std::remove(m_textMarks.begin(), m_textMarks.end(), mark);
            m_textMarks.erase(it, m_textMarks.end());
            delete mark;
        };
        auto textMark = new TextMark(filePath.toString(), it.key().line, it.value(), onMarkRemoved);
        m_textMarks.push_back(textMark);
        m_document->addMark(textMark);
    }
    RubocopFuture rubocopFuture(offenses);
    TextEditor::SemanticHighlighter::incrementalApplyExtraAdditionalFormats(m_document->syntaxHighlighter(),
                                                                            rubocopFuture.future(), 0,
                                                                            offenses.count(), m_extraFormats);
    TextEditor::SemanticHighlighter::clearExtraAdditionalFormatsUntilEnd(m_document->syntaxHighlighter(),
                                                                         rubocopFuture.future());
    m_busy = false;

    qCDebug(log) << "rubocop in" << m_timer.elapsed() << "ms," << offenses.count() << "offenses found.";
}

static int kindOfSeverity(const QStringRef &severity)
{
    switch (severity.at(0).toLatin1()) {
    case 'W': return 0; // yellow
    case 'C': return 1; // green
    default:  return 2; // red
    }
}

Offenses RubocopHighlighter::processRubocopOutput()
{
    Offenses result;
    Diagnostics &diag = m_diagnostics[m_document->filePath()] = Diagnostics();
    clearTextMarks();

    const QVector<QStringRef> lines = m_outputBuffer.splitRef('\n');
    for (const QStringRef &line : lines) {
        if (line == "--")
            break;
        QVector<QStringRef> fields = line.split(':');
        if (fields.size() < 5)
            continue;
        int kind = kindOfSeverity(fields[0]);
        int lineN = fields[1].toInt();
        int column = fields[2].toInt();
        int length = fields[3].toInt();
        result << TextEditor::HighlightingResult(uint(lineN), uint(column), uint(length), kind);

        int messagePos = fields[4].position();
        QStringRef message(line.string(), messagePos, line.position() + line.length() - messagePos);
        diag.messages[lineColumnLengthToRange(lineN, column, length)] = message.toString();
    }
    m_outputBuffer.clear();

    return result;
}

Ruby::Range RubocopHighlighter::lineColumnLengthToRange(int line, int column, int length)
{
    const QTextBlock block = m_document->document()->findBlockByLineNumber(line - 1);
    const int pos = block.position() + column;
    return Range(line, pos, length);
}

}
