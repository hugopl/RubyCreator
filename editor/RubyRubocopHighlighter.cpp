#include "RubyRubocopHighlighter.h"

#include <QDebug>
#include <QProcess>
#include <QTextDocument>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QtConcurrent>

#include <texteditor/textdocument.h>
#include <texteditor/semantichighlighter.h>

#define RUBOCOP QStringLiteral("rubocop")
namespace Ruby {

class RubocopFuture : public QFutureInterface<TextEditor::HighlightingResult>, public QObject
{
public:
    RubocopFuture(const Offenses& offenses)
    {
        if (!offenses.isEmpty())
            reportResults(offenses);
    }
};

RubocopHighlighter::RubocopHighlighter()
    : m_rubocopFound(false)
    , m_rubocop(nullptr)
    , m_startRevision(0)
    , m_document(nullptr)
{
    checkRubocop();

    QTextCharFormat format;
    format.setUnderlineColor(Qt::red);
    format.setUnderlineStyle(QTextCharFormat::WaveUnderline);
    m_extraFormats[0] = format;
}

RubocopHighlighter *RubocopHighlighter::instance()
{
    static RubocopHighlighter rubocop;
    return &rubocop;
}

// return false if we are busy, true if everything is ok (or rubocop wasn't found)
bool RubocopHighlighter::run(TextEditor::TextDocument* document)
{
    if (!m_rubocopFound)
        return !m_rubocop;

    if (m_rubocop)
        return false;

    m_startRevision = document->document()->revision();

    m_timer.start();
    m_document = document;

    m_rubocop = new QProcess;
    void (QProcess::* signal)(int) = &QProcess::finished;
    connect(m_rubocop, signal, [&](int status) {
        if (status == 1 && m_startRevision == m_document->document()->revision()) {
          // rubocop run is in another process, so we don't need to deal with threads and QFuture here.
          Offenses offenses = processRubocopOutput(m_rubocop->readAllStandardOutput());
          RubocopFuture rubocopFuture(offenses);

          // TODO: Clear all extra additional formats when no offenses where found.
//          if (offenses.isEmpty())
//              clearAllExtraAdditionalFormats();
          TextEditor::SemanticHighlighter::incrementalApplyExtraAdditionalFormats(m_document->syntaxHighlighter(),
                                                                                  rubocopFuture.future(), 0,
                                                                                  offenses.count(), m_extraFormats);
          TextEditor::SemanticHighlighter::clearExtraAdditionalFormatsUntilEnd(m_document->syntaxHighlighter(),
                                                                               rubocopFuture.future());
        }
        m_rubocop->deleteLater();
        m_rubocop = 0;
        qDebug() << "rubocop in" << m_timer.elapsed() << "ms";
    });

    // This wont work on Windows, and rubocop doens't have an option to read data from stdin.
    m_rubocop->start(RUBOCOP, QStringList() << QStringLiteral("-lnf") << QStringLiteral("j") << QStringLiteral("/dev/stdin"));
    m_rubocop->write(document->plainText().toUtf8());
    m_rubocop->closeWriteChannel();
    return true;
}

void RubocopHighlighter::checkRubocop()
{
    m_rubocop = new QProcess;
    void (QProcess::* signal)(int) = &QProcess::finished;
    QObject::connect(m_rubocop, signal, [&](int status) {
        m_rubocopFound = status == 0;
        m_rubocop->deleteLater();
        m_rubocop = 0;
    });
    m_rubocop->start(RUBOCOP, QStringList() << QStringLiteral("--version"));
}

Offenses RubocopHighlighter::processRubocopOutput(const QByteArray& jsonData)
{
    QJsonDocument json = QJsonDocument::fromJson(jsonData);
    if (!json.isObject())
        return Offenses();

    Offenses result;
    QJsonArray files = json.object()[QStringLiteral("files")].toArray();
    for (QJsonValue file : files) {
        QJsonArray offenses = file.toObject()[QStringLiteral("offenses")].toArray();
        for (QJsonValue offense_ : offenses) {
            QJsonObject offense = offense_.toObject();
//            QString message = offense[QStringLiteral("message")].toString();
            QJsonObject location = offense[QStringLiteral("location")].toObject();
            int line = location[QStringLiteral("line")].toInt();
            int column = location[QStringLiteral("column")].toInt();
            int length = location[QStringLiteral("length")].toInt();
            result << TextEditor::HighlightingResult(line, column, length, 0);
        }
    }
    return result;
}

}
