#ifndef SCANNER_TEST_H
#define SCANNER_TEST_H

#include <QObject>
#include "RubyScanner.h"

class TestScanner: public QObject
{
Q_OBJECT
public:
    TestScanner(QObject* parent = 0);
private slots:
    void cleanup();

    void namespaceIsNotASymbol();
    void symbolOnArray();
    void def();
    void context();
    void indentIf();
    void lineCount();
private:
    Ruby::Scanner* m_scanner;
    typedef QVector<Ruby::Token::Kind> Tokens;

    Tokens tokenize(const QString& code, bool debug = false);
};

#endif
