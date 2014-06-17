#include "ScannerTest.h"
#include "RubyScanner.h"

#include <QtTest/QtTest>
#include <vector>

using namespace Ruby;

QDebug& operator<<(QDebug& s, Token::Kind t)
{
    const char* str;
    switch(t) {
    case Token::Number: str = "Number"; break;
    case Token::String: str = "String"; break;
    case Token::Keyword: str = "Keyword"; break;
    case Token::Type: str = "Type"; break;
    case Token::ClassField: str = "ClassField"; break;
    case Token::Operator: str = "Operator"; break;
    case Token::Comment: str = "Comment"; break;
    case Token::Identifier: str = "Identifier"; break;
    case Token::Whitespace: str = "Whitespace"; break;
    case Token::Constant: str = "Constant"; break;
    case Token::Global: str = "Global"; break;
    case Token::Regexp: str = "Regexp"; break;
    case Token::Symbol: str = "Symbol"; break;
    case Token::EndOfBlock: str = "EOB"; break;
    default: str = "???"; break;
    }
    return s << str;
}

typedef QVector<Token::Kind> Tokens;

Tokens tokenize(const QString& code, bool debug = false)
{
    Scanner scanner(&code);
    QVector<Token::Kind> tokens;
    Token token;
    while ((token = scanner.read()).kind != Token::EndOfBlock) {
        tokens << token.kind;
        if (debug)
            qDebug() << "> " << token.kind << code.mid(token.position, token.length);
    }
    return tokens;
}

void TestScanner::namespaceIsNotASymbol()
{
    Tokens expectedTokens = { Token::Type, Token::Operator, Token::Type, Token::Whitespace, Token::Identifier};
    QCOMPARE(tokenize("Foo::Bar oi"), expectedTokens);
}

QTEST_APPLESS_MAIN(TestScanner)
#include "TestScanner.moc"
