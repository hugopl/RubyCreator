#ifndef Ruby_Symbol_h
#define Ruby_Symbol_h

#include <QDateTime>

namespace Ruby {

struct Symbol
{
    Symbol() {}
    Symbol(const QString& name, int line, int column);
    QString name;
    QString context;
    QString file;
    int line;
    int column;
};

struct SymbolGroup
{
    QDateTime lastUpdate;
    QList<Symbol> symbols;
};

}

Q_DECLARE_METATYPE(Ruby::Symbol)

#endif
