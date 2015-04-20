#ifndef Ruby_RubocopHighlighter_h
#define Ruby_RubocopHighlighter_h

#include <texteditor/semantichighlighter.h>
#include <texteditor/basetextdocument.h>

#include <utils/fileutils.h>

#include <QObject>
#include <QFuture>
#include <QFutureWatcher>
#include <QElapsedTimer>
#include <QTemporaryFile>

#include <functional>

QT_FORWARD_DECLARE_CLASS(QProcess)

namespace TextEditor { class TextDocument; }

namespace Ruby {

class Range {
public:
    int pos;
    int length;

    Range() : pos(0), length(0) { }
    Range(int pos, int length) : pos(pos), length(length) { }

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

struct Diagnostics {
    QMap<Range, QString> messages;
};

class RubocopHighlighter : public QObject {
    Q_OBJECT
public:
    RubocopHighlighter();
    ~RubocopHighlighter();

    static RubocopHighlighter *instance();

    bool run(TextEditor::BaseTextDocument *document, const QString &fileNameTip);
    QString diagnosticAt(const Utils::FileName &file, int pos);
private:
    bool m_rubocopFound;
    bool m_busy;
    QProcess *m_rubocop;
    QTemporaryFile m_rubocopScript;
    QString m_outputBuffer;

    int m_startRevision;
    TextEditor::BaseTextDocument *m_document;
    QHash<int, QTextCharFormat> m_extraFormats;


    QHash<Utils::FileName, Diagnostics> m_diagnostics;

    QElapsedTimer m_timer;

    void initRubocopProcess();
    void finishRuboCopHighlight();
    Offenses processRubocopOutput();

    Range lineColumnLengthToRange(int line, int column, int length);
};
}

#endif
