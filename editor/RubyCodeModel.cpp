#include "RubyCodeModel.h"
#include "RubyScanner.h"

#include <QElapsedTimer>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QDebug>

namespace Ruby {

class CodeModel::Data
{
    Q_DISABLE_COPY(Data)

public:
    Data(const QString &fileName = QString()) : fileName(fileName) {}

    void clear()
    {
        methods.clear();
        identifiers.clear();
        constants.clear();
        classes.clear();
        symbols.clear();
    }

    QDateTime lastUpdate;
    QString fileName;

    QList<Symbol> methods;
    QList<Symbol> classes;
    QList<Symbol> constantsDelc;
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
    if (!file.endsWith(".rb") && !file.endsWith(".rake"))
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

    for (const QString &file : files)
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

// This does not support multiline symbols defined by %i.
static void parseRubySymbol(const QString &contents, Token token, QSet<QString>& symbols)
{
    if (contents[token.position] == ':') {
        symbols << contents.mid(token.position, token.length);
    } else if (contents[token.position + token.length - 1] == ':') {
        QString symbol = contents.mid(token.position, token.length - 1);
        symbol.prepend(':');
        symbols << symbol;
    } else {
        QStringRef symbolsToSplit;
        if (contents[token.position] == '%') {
            if (token.length < 4 || contents[token.position + 1] != 'i')
                return;

            QChar endDelimiter = translateDelimiter(contents[token.position + 2]);
            int start = token.position + 3;
            QStringRef contentsRef(&contents, start, token.length - 3);
            int end = contentsRef.indexOf(endDelimiter);
            symbolsToSplit = end < 0 ? contentsRef : contentsRef.left(end);
        } else {
            // Work 90% of the time... but not for the last item in a multiline symbol declaration.
            symbolsToSplit = QStringRef(&contents, token.position, token.length);
        }

        // To be able to use QStringRef everywhere we split things by spaces instead of by the regexp /\s+/
        // But who cares for the ones using TABS!? :-)
        QVector<QStringRef> result = symbolsToSplit.split(' ');
        for (QStringRef item : result)
            symbols << item.toString().prepend(':');
    }
}

void addMethodParameter(Symbol& method, const QString& parameter)
{
    QString& name = method.name;
    const QChar end = name[name.length() -1];

    if (end == ')') {
        name.chop(1);
        name.append(", ");
    } else {
        name.append('(');
    }
    name.append(parameter);
    name.append(')');
}

void CodeModel::updateFile(const QString &fileName, const QString &contents)
{
    if (fileName.isEmpty())
        return;

    Data *&data = m_model[fileName];
    if (!data)
        data = new Data(fileName);
    data->clear();

    Scanner scanner(&contents);
    scanner.enableContextRecognition();

    const QString *fileNamePtr = &data->fileName;

    Token token;
    Token previousToken;
    while ((token = scanner.read()).kind != Token::EndOfBlock) {
        switch (token.kind) {
        case Token::Method:
            data->methods << createSymbol(fileNamePtr, contents, scanner, token);
            break;
        case Token::Parameter:
            addMethodParameter(data->methods.last(), contents.mid(token.position, token.length));
            break;
        case Token::Identifier:
            data->identifiers << contents.mid(token.position, token.length);
            break;
        case Token::Constant:
            if (previousToken.kind == Token::KeywordClass)
                data->classes << createSymbol(fileNamePtr, contents, scanner, token);
            data->constants << contents.mid(token.position, token.length);
            break;
        case Token::Symbol:
        case Token::SymbolHashKey:
            parseRubySymbol(contents, token, data->symbols);
            break;
        case Token::OperatorAssign:
            if (previousToken.kind == Token::Constant)
                data->constantsDelc << createSymbol(fileNamePtr, contents, scanner, previousToken);
            break;
        default:
            break;
        }
        if (token.kind != Token::Whitespace)
            previousToken = token;
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
    for (const Data *data : m_model)
        result << data->methods;
    return result;
}

QList<Symbol> CodeModel::allClasses() const
{
    QList<Symbol> result;
    for (const Data *data : m_model)
        result << data->classes;
    return result;
}

QList<Symbol> CodeModel::allMethodsNamed(const QString &name) const
{
    QList<Symbol> result;
    const int nameLength = name.length();
    // FIXME: Replace this linear brute force approach
    for (const Data *data : m_model) {
        for (const Symbol &symbol : data->methods) {
            const QString &symbolName = symbol.name;
            if (symbolName.startsWith(name)) {
                if (symbolName.length() > nameLength && symbolName[nameLength] != '(')
                    continue;
                result << symbol;
            }
        }
    }
    return result;
}

QList<Symbol> CodeModel::allClassesAndConstantsNamed(const QString &name) const
{
    QList<Symbol> result;
    // FIXME: Replace this linear brute force approach
    for (const Data *data : m_model) {
        for (const Symbol &symbol : data->classes) {
            if (symbol.name == name)
                result << symbol;
        }
    }

    // constants are less important, keep them at the bottom.
    for (const Data *data : m_model) {
        for (const Symbol &symbol : data->constantsDelc) {
            if (symbol.name == name)
                result << symbol;
        }
    }

    return result;
}

}
