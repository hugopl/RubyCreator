#include "RubyRubocopHighlighter.h"

#include <QDebug>
#include <QProcess>
#include <QTextDocument>
#include <QtConcurrent>
#include <QMessageBox>

#include <texteditor/textdocument.h>
#include <texteditor/semantichighlighter.h>

#define RUBOCOP QStringLiteral("rubocop")
namespace Ruby {

class RubocopFuture : public QFutureInterface<TextEditor::HighlightingResult>, public QObject
{
public:
    RubocopFuture(const Offenses& offenses)
    {
        reportResults(offenses);
    }
};

RubocopHighlighter::RubocopHighlighter()
    : m_rubocopFound(true)
    , m_busy(false)
    , m_rubocop(nullptr)
    , m_startRevision(0)
    , m_document(nullptr)
{
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
    m_rubocop->closeWriteChannel();
    m_rubocop->waitForFinished(3000);
    delete m_rubocop;
}

RubocopHighlighter *RubocopHighlighter::instance()
{
    static RubocopHighlighter rubocop;
    return &rubocop;
}

// return false if we are busy, true if everything is ok (or rubocop wasn't found)
bool RubocopHighlighter::run(TextEditor::TextDocument* document)
{
    if (m_busy || m_rubocop->state() == QProcess::Starting)
        return false;
    if (!m_rubocopFound)
        return true;

    m_busy = true;
    m_startRevision = document->document()->revision();

    m_timer.start();
    m_document = document;
    m_rubocop->write(document->filePath().toUtf8());
    m_rubocop->write("\n");
    QByteArray data = document->plainText().toUtf8();
    m_rubocop->write(data.constData(), data.length() + 1);
    return true;
}

void RubocopHighlighter::initRubocopProcess()
{
    if (m_rubocopScript.open()) {
        QFile script(QStringLiteral(":/rubysupport/rubocop.rb"));
        script.open(QFile::ReadOnly);
        m_rubocopScript.write(script.readAll());
        m_rubocopScript.close();
    }

    m_rubocop = new QProcess;
    void (QProcess::* signal)(int) = &QProcess::finished;
    QObject::connect(m_rubocop, signal, [&](int status) {
        if (status) {
            QMessageBox::critical(0, QStringLiteral("Rubocop"), QString::fromUtf8(m_rubocop->readAllStandardError().trimmed()));
            m_rubocopFound = false;
        }
    });

    QObject::connect(m_rubocop, &QProcess::readyReadStandardOutput, [&]() {
        m_outputBuffer.append(QString::fromUtf8(m_rubocop->readAllStandardOutput()));
        if (m_outputBuffer.endsWith(QStringLiteral("--\n")))
            finishRuboCopHighlight();
    });

    m_rubocop->start(QStringLiteral("ruby"), QStringList(m_rubocopScript.fileName()));
}

void RubocopHighlighter::finishRuboCopHighlight()
{
    if (m_startRevision != m_document->document()->revision())
        return;

    Offenses offenses = processRubocopOutput();
    RubocopFuture rubocopFuture(offenses);
    TextEditor::SemanticHighlighter::incrementalApplyExtraAdditionalFormats(m_document->syntaxHighlighter(),
                                                                            rubocopFuture.future(), 0,
                                                                            offenses.count(), m_extraFormats);
    TextEditor::SemanticHighlighter::clearExtraAdditionalFormatsUntilEnd(m_document->syntaxHighlighter(),
                                                                         rubocopFuture.future());
    m_busy = false;

    qDebug() << "rubocop in" << m_timer.elapsed() << "ms," << offenses.count() << "offenses found.";
}

static int kindOfSeverity(const QStringRef& severity)
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

    for (const QStringRef& line : m_outputBuffer.splitRef(QLatin1Char('\n'))) {
        if (line == QStringLiteral("--"))
            break;
        QVector<QStringRef> fields = line.split(QLatin1Char(':'));
        int kind = kindOfSeverity(fields[0]);
        int lineN = fields[1].toInt();
        int column = fields[2].toInt();
        int length = fields[3].toInt();
        result << TextEditor::HighlightingResult(lineN, column, length, kind);
    }
    m_outputBuffer.clear();

    return result;
}

}
