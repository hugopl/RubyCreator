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

    QList<Symbol> methodsIn(const QString& file) const;
    QList<Symbol> allMethods() const;
    QList<Symbol> allMethodsNamed(const QString& name) const;

public slots:
    // pass a QIODevice because the file may not be saved on file system.
    void updateModel(const QString& fileName, QIODevice &contents);
    void updateModels(const QStringList& files);

private:
    QHash<QString, SymbolGroup> m_symbols;
};

}

#endif
