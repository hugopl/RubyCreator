#ifndef RubyCodeModel_h
#define RubyCodeModel_h

#include <QMetaType>
#include <QObject>
#include <QHash>
#include <ruby.h>

namespace RubyEditor {

struct RubySymbol
{
    RubySymbol() {}
    RubySymbol(const QString& name, int line, int column);
    QString name;
    int line;
    int column;
};

class RubyCodeModel : QObject
{
    Q_OBJECT
public:
    RubyCodeModel();
    ~RubyCodeModel();
    RubyCodeModel(const RubyCodeModel&) = delete;

    static RubyCodeModel* instance();
    void updateModel(const QString& file);

    QList<RubySymbol> methodsIn(const QString& file);

private:
    ID m_getMethodDeclarations;

    QHash<QString, QList<RubySymbol> > m_symbols;
};

}

Q_DECLARE_METATYPE(RubyEditor::RubySymbol)

#endif
