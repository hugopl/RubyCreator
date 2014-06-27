#ifndef Ruby_CodeModel_h
#define Ruby_CodeModel_h

#include <QDateTime>
#include <QMetaType>
#include <QIODevice>
#include <QObject>
#include <QHash>

#include "RubySymbol.h"

namespace Ruby {

class CodeModel : QObject
{
    Q_OBJECT
public:
    CodeModel();
    ~CodeModel();
    CodeModel(const CodeModel&) = delete;

    static CodeModel* instance();
    void removeSymbolsFrom(const QString& file);
    void addFile(const QString& file);
    void addFiles(const QStringList& files);
    // pass a QIODevice because the file may not be saved on file system.
    void updateFile(const QString& fileName, const QString& contents);

    QList<Symbol> methodsIn(const QString& file) const;
    QSet<QString> identifiersIn(const QString& file) const;
    QSet<QString> constantsIn(const QString& file) const;
    QSet<QString> symbolsIn(const QString& file) const;
    QList<Symbol> allMethods() const;
    QList<Symbol> allMethodsNamed(const QString& name) const;

private:

    class Data;
    QHash<QString, Data*> m_model;
};

}

#endif
