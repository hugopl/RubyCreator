#ifndef Ruby_RubocopHighlighter_h
#define Ruby_RubocopHighlighter_h

#include <texteditor/semantichighlighter.h>

#include <utils/fileutils.h>

#include <QObject>
#include <QFuture>
#include <QFutureWatcher>
#include <QElapsedTimer>
#include <QTemporaryFile>

#include <functional>
#include <memory>

QT_FORWARD_DECLARE_CLASS(QProcess)

namespace TextEditor { class TextDocument; }

namespace Ruby {

class TextMark;

class Range {
public:
    int line = 0;
    int pos = 0;
    int length = 0;

    Range() = default;
    Range(int pos, int length) : pos(pos), length(length) { }
    Range(int line, int pos, int length) : line(line), pos(pos), length(length) { }

    // Not really equal, since the length attribute is ignored.
    bool operator==(const Range &other) const {
        const int value = other.pos;
        return value >= pos && value < (pos + length);
    }

    bool operator<(const Range &other) const {
        const int value = other.pos;
        return pos < value && (pos + length) < value;
    }
};

typedef TextEditor::HighlightingResult Offense;
typedef QVector<TextEditor::HighlightingResult> Offenses;

struct Diagnostic
{
    int line;
    int severity;
    QString message;
    std::shared_ptr<TextMark> textMark;
};

using Diagnostics = std::vector<Diagnostic>;

class RubocopHighlighter : public QObject
{
    Q_OBJECT
public:
    RubocopHighlighter();
    ~RubocopHighlighter();

    static RubocopHighlighter *instance();

    bool run(TextEditor::TextDocument *document, const QString &fileNameTip);

private:
    bool m_rubocopFound = true;
    bool m_busy = false;
    QProcess *m_rubocop = nullptr;
    QTemporaryFile m_rubocopScript;
    QString m_outputBuffer;

    int m_startRevision = 0;
    TextEditor::TextDocument *m_document = nullptr;
    QHash<int, QTextCharFormat> m_extraFormats;

    QHash<Utils::FileName, Diagnostics> m_diagnostics;
    std::vector<TextMark *> m_textMarks;

    QElapsedTimer m_timer;

    void initRubocopProcess();
    void finishRuboCopHighlight();
    Offenses processRubocopOutput();

    Range lineColumnLengthToRange(int line, int column, int length);
};
}

#endif
