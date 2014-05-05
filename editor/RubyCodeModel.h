#ifndef Ruby_CodeModel_h
#define Ruby_CodeModel_h

#include <QDateTime>
#include <QMetaType>
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

    QList<Symbol> symbolsIn(const QString& file);

public slots:
    void updateModel(const QString& file);
    void updateModels(const QStringList& files);

private:
    QHash<QString, SymbolGroup> m_symbols;
};

}

Q_DECLARE_METATYPE(Ruby::Symbol)

#endif
