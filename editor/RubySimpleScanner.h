#ifndef Ruby_SimpleScanner_h
#define Ruby_SimpleScanner_h

#include <QStringRef>
#include <QTextStream>

#include "RubySymbol.h"

namespace Ruby {

class SimpleScanner
{
public:
    SimpleScanner(QIODevice* device);

    Symbol nextSymbol();

private:
    QTextStream m_src;
    int m_lineCount;
    int m_contextDepth;
    QString m_context;
};

}

#endif
