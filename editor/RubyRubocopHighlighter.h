#ifndef Ruby_RubocopHighlighter_h
#define Ruby_RubocopHighlighter_h

#include <QObject>
#include <QFuture>
#include <QFutureWatcher>
#include <QElapsedTimer>

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
    static RubocopHighlighter *instance();

    bool run(TextEditor::TextDocument* document);
private:   
    bool m_rubocopFound;
    QProcess* m_rubocop;
    int m_startRevision;
    TextEditor::TextDocument* m_document;
    QHash<int, QTextCharFormat> m_extraFormats;

    QElapsedTimer m_timer;

    void checkRubocop();
    Offenses processRubocopOutput(const QByteArray& jsonData);
};
}

#endif
