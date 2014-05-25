#include "RubyEditor.h"

#include "RubyCodeModel.h"
#include "RubyConstants.h"
#include "RubyEditorWidget.h"

#include <texteditor/texteditorconstants.h>

#include <QBuffer>

namespace Ruby {

static const auto UpdateDocumentDefaultInterval = 150;

Editor::Editor(EditorWidget* parent)
    : TextEditor::BaseTextEditor(parent)
{
    setId(Constants::EditorId);
    setContext(Core::Context(Constants::EditorId, TextEditor::Constants::C_TEXTEDITOR));

    m_updateCodeModelTimer.setSingleShot(true);
    m_updateCodeModelTimer.setInterval(UpdateDocumentDefaultInterval);
    connect(&m_updateCodeModelTimer, &QTimer::timeout, this, &Editor::updateCodeModel);
    // There's no contentsChanged signal on Core::IDocument interface.
    connect(document(), SIGNAL(contentsChanged()), this, SLOT(scheduleCodeModelUpdate()));

    CodeModel::instance();
}

void Editor::scheduleCodeModelUpdate()
{
    m_updateCodeModelTimer.start();
}

void Editor::updateCodeModel()
{
    // TODO: This is slow, need write an IODevice that get lines directly from QTextBlocks, since readLine is the only used method.
    //       the result? the code is copying the file two times in memory :-(, but it works :-D.
    QByteArray textData = baseTextDocument()->plainText().toUtf8();
    QBuffer buffer(&textData);
    buffer.open(QBuffer::ReadOnly);
    CodeModel::instance()->updateFile(document()->filePath(), buffer);
}

}

#include "RubyEditor.moc"
