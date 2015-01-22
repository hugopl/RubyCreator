#ifndef Ruby_RubocopHighlighter_h
#define Ruby_RubocopHighlighter_h

#include <QObject>
#include <QFuture>
#include <QFutureWatcher>
#include <QElapsedTimer>
#include <QTemporaryFile>

#include <functional>

#include <texteditor/semantichighlighter.h>

class QProcess;
namespace TextEditor {
class TextDocument;
}

namespace Ruby {

typedef TextEditor::HighlightingResult Offense;
typedef QVector<TextEditor::HighlightingResult> Offenses;

class RubocopHighlighter : public QObject {
    Q_OBJECT
public:
    RubocopHighlighter();
    ~RubocopHighlighter();

    static RubocopHighlighter *instance();

    bool run(TextEditor::TextDocument* document);
private:   
    bool m_rubocopFound;
    bool m_busy;
    QProcess* m_rubocop;
    QTemporaryFile m_rubocopScript;
    QString m_outputBuffer;

    int m_startRevision;
    TextEditor::TextDocument* m_document;
    QHash<int, QTextCharFormat> m_extraFormats;

    QElapsedTimer m_timer;

    void initRubocopProcess();
    void finishRuboCopHighlight();
    Offenses processRubocopOutput();
};
}

#endif
