#ifndef Ruby_Symbol_h
#define Ruby_Symbol_h

#include <QDateTime>
#include <QMetaType>

namespace Ruby {

struct Symbol
{
    Symbol(const QString *file = nullptr) : file(file) { }
    const QString *file;
    QString name;
    QString context;
    int line;
    int column;
};

}

Q_DECLARE_METATYPE(Ruby::Symbol)

#endif
