#include "RubyHoverHandler.h"

#include "RubyRubocopHighlighter.h"

#include <QDebug>
#include <texteditor/basetextdocument.h>
#include <texteditor/plaintexteditor.h>

namespace Ruby {

void HoverHandler::identifyMatch(TextEditor::BaseTextEditor *editorWidget, int pos)
{
    RubocopHighlighter *rubocop = RubocopHighlighter::instance();

    QString diagnostic = rubocop->diagnosticAt(Utils::FileName::fromString(editorWidget->baseTextDocument()->filePath()), pos);
    setToolTip(diagnostic);
}

}
