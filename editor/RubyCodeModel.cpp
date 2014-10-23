#include "RubyCodeModel.h"
#include "RubyScanner.h"

#include <QElapsedTimer>
#include <QFile>
#include <QFileInfo>
#include <QDebug>

namespace Ruby {

class CodeModel::Data
{
    Q_DISABLE_COPY(CodeModel::Data)

public:
    CodeModel::Data(const QString &fileName = QString()) : fileName(fileName) {}

    void clear()
    {
        methods.clear();
        identifiers.clear();
        constants.clear();
        symbols.clear();
    }

    QDateTime lastUpdate;
    QString fileName;

    QList<Symbol> methods;
    QSet<QString> identifiers;
    QSet<QString> constants;
    QSet<QString> symbols;
};

CodeModel::CodeModel()
{
}

CodeModel::~CodeModel()
{
    qDeleteAll(m_model);
}

CodeModel *CodeModel::instance()
{
    static CodeModel model;
    return &model;
}

void CodeModel::removeSymbolsFrom(const QString &file)
{
    delete m_model[file];
    m_model.remove(file);
}

void CodeModel::addFile(const QString &file)
{
    if (!file.endsWith(QLatin1String(".rb")) && !file.endsWith(QLatin1String(".rake")))
        return;

    QFileInfo info(file);
    Data *&data = m_model[file];
    if (!data)
        data = new Data(file);

    if (!data->lastUpdate.isNull() && data->lastUpdate > info.lastModified())
        return;

    QFile fp(file);
    if (!fp.open(QFile::ReadOnly))
        return;
    updateFile(file, QString::fromUtf8(fp.readAll()));
}

void CodeModel::addFiles(const QStringList &files)
{
    QElapsedTimer timer;
    timer.start();

    foreach (const QString &file, files)
        addFile(file);

    qDebug() << "Code model updated in" << timer.elapsed() << "ms";
}

static Symbol createSymbol(const QString *fileName, const QString &contents, Scanner &scanner, Token token)
{
    Symbol sym(fileName);
    sym.name = contents.mid(token.position, token.length);
    sym.line = scanner.currentLine();
    sym.column = scanner.currentColumn(token);
    sym.context = scanner.contextName();
    return sym;
}

void CodeModel::updateFile(const QString &fileName, const QString &contents)
{
    Data* &data = m_model[fileName];
    if (!data)
        data = new Data(fileName);
    data->clear();

    Scanner scanner(&contents);
    scanner.enableContextRecognition();

    const QString *fileNamePtr = &data->fileName;
    Token token;
    while ((token = scanner.read()).kind != Token::EndOfBlock) {
        switch (token.kind) {
        case Token::Method:
            data->methods << createSymbol(fileNamePtr, contents, scanner, token);
            break;
        case Token::Identifier:
            data->identifiers << contents.mid(token.position, token.length);
            break;
        case Token::Constant:
            data->constants << contents.mid(token.position, token.length);
            break;
        case Token::Symbol:
            data->symbols << contents.mid(token.position, token.length);
            break;
        default:
            break;
        }
    }

    data->lastUpdate = QDateTime::currentDateTime();
}

QList<Symbol> CodeModel::methodsIn(const QString &file) const
{
    Data *data = m_model[file];
    return data ? data->methods : QList<Symbol>();
}

QSet<QString> CodeModel::identifiersIn(const QString &file) const
{
    Data *data = m_model[file];
    return data ? data->identifiers : QSet<QString>();
}

QSet<QString> CodeModel::constantsIn(const QString &file) const
{
    Data *data = m_model[file];
    return data ? data->constants : QSet<QString>();
}

QSet<QString> CodeModel::symbolsIn(const QString &file) const
{
    Data *data = m_model[file];
    return data ? data->symbols : QSet<QString>();
}

QList<Symbol> CodeModel::allMethods() const
{
    QList<Symbol> result;
    foreach (const Data *data, m_model)
        result << data->methods;
    return result;
}

QList<Symbol> CodeModel::allMethodsNamed(const QString &name) const
{
    QList<Symbol> result;
    // FIXME: Replace this linear brute force approach
    foreach (const Data *data, m_model) {
        foreach (const Symbol &symbol, data->methods) {
            if (symbol.name == name)
                result << symbol;
        }
    }
    return result;
}

}
