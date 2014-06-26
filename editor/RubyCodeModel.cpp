#include "RubyCodeModel.h"
#include "RubyScanner.h"

#include <QElapsedTimer>
#include <QFile>
#include <QFileInfo>
#include <QDebug>

namespace Ruby {

Symbol::Symbol(const QString& name, int line, int column)
    : name(name), line(line), column(column)
{
}

CodeModel::CodeModel()
{
}

CodeModel::~CodeModel()
{
}

CodeModel* CodeModel::instance()
{
    static CodeModel model;
    return &model;
}

void CodeModel::removeSymbolsFrom(const QString& file)
{
    m_symbols.remove(file);
}

void CodeModel::addFile(const QString& file)
{
    QFileInfo info(file);
    SymbolGroup& group = m_symbols[file];

    if (!group.lastUpdate.isNull() && group.lastUpdate > info.lastModified())
        return;

    QFile fp(file);
    if (!fp.open(QFile::ReadOnly))
        return;
    updateFile(file, fp.readAll());
}

void CodeModel::addFiles(const QStringList& files)
{
    QElapsedTimer timer;
    timer.start();

    for (const QString& file : files)
        addFile(file);

    qDebug() << "Code model updated in" << timer.elapsed() << "ms";
}

void CodeModel::updateFile(const QString& fileName, const QString& contents)
{
    SymbolGroup& group = m_symbols[fileName];
    group.symbols.clear();

    Scanner scanner(&contents);
    scanner.enableContextRecognition();

    Token token;
    while ((token = scanner.read()).kind != Token::EndOfBlock) {
        if (token.kind == Token::Method) {
            Symbol sym(contents.mid(token.position, token.length), scanner.currentLine(), scanner.currentColumn(token));
            sym.file = fileName;
            sym.context = scanner.contextName();
            group.symbols << sym;
        }
    }

    group.lastUpdate = QDateTime::currentDateTime();
}

QList<Symbol> CodeModel::methodsIn(const QString& file) const
{
    return m_symbols[file].symbols;
}

QList<Symbol> CodeModel::allMethods() const
{
    // TODO: cache this!?
    QList<Symbol> result;
    for (const SymbolGroup& group : m_symbols)
        result << group.symbols;
    return result;
}

QList<Symbol> CodeModel::allMethodsNamed(const QString& name) const
{
    QList<Symbol> result;
    // FIXME: Replace this linear brute force approach
    for (const SymbolGroup& group : m_symbols) {
        for (const Symbol& symbol : group.symbols) {
            if (symbol.name == name)
                result << symbol;
        }
    }
    return result;
}

}

#include "RubyCodeModel.moc"
