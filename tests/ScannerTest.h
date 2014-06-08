#ifndef SCANNER_TEST_H
#define SCANNER_TEST_H

#include <QObject>

class TestScanner: public QObject
{
Q_OBJECT
private slots:
    void namespaceIsNotASymbol();
};

#endif
