#ifndef Ruby_CodeModel_h
#define Ruby_CodeModel_h

#include <QMetaType>
#include <QObject>
#include <QHash>
#include <ruby.h>

namespace Ruby {

struct Symbol
{
    Symbol() {}
    Symbol(const QString& name, int line, int column);
    QString name;
    int line;
    int column;
};

class CodeModel : QObject
{
    Q_OBJECT
public:
    CodeModel();
    ~CodeModel();
    CodeModel(const CodeModel&) = delete;

    static CodeModel* instance();
    void updateModel(const QString& file);

    QList<Symbol> methodsIn(const QString& file);

private:
    ID m_getMethodDeclarations;

    QHash<QString, QList<Symbol> > m_symbols;
};

}

Q_DECLARE_METATYPE(Ruby::Symbol)

#endif
