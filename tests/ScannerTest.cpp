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
    case Token::KeywordDef: str = "Keyword-Def"; break;
    case Token::KeywordSelf: str = "Keyword-Def"; break;
    case Token::Method: str = "Method"; break;
    case Token::Parameter: str = "Parameter"; break;
    case Token::Type: str = "Type"; break;
    case Token::ClassField: str = "ClassField"; break;
    case Token::Operator: str = "Operator"; break;
    case Token::OperatorComma: str = "OperatorComma"; break;
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

void TestScanner::symbolOnArray()
{
    Tokens expectedTokens = { Token::Identifier, Token::Operator, Token::Symbol, Token::Operator };
    QCOMPARE(tokenize("foo[:bar]"), expectedTokens);
}

void TestScanner::def()
{
    Tokens expectedTokens = { Token::KeywordDef, Token::Whitespace, Token::Method, Token::Whitespace,
                              Token::Parameter, Token::OperatorComma, Token::Whitespace, Token::Parameter};
    QCOMPARE(tokenize("def foo bar, tender"), expectedTokens);
    expectedTokens = { Token::KeywordDef, Token::Whitespace, Token::Method, Token::Whitespace,
                              Token::Parameter, Token::Whitespace, Token::OperatorComma, Token::Whitespace, Token::Parameter};
    QCOMPARE(tokenize("def foo bar  , tender"), expectedTokens);
    expectedTokens = { Token::KeywordDef, Token::Whitespace, Token::KeywordSelf, Token::OperatorDot, Token::Method, Token::Whitespace,
                              Token::Parameter};
    QCOMPARE(tokenize("def self.foo bar"), expectedTokens);
    expectedTokens = { Token::KeywordDef, Token::Whitespace, Token::Method, Token::Operator, Token::Parameter, Token::Operator};
    QCOMPARE(tokenize("def foo(bar)"), expectedTokens);
    expectedTokens = { Token::KeywordDef, Token::Whitespace, Token::Method, Token::Operator, Token::Parameter,
                       Token::OperatorComma, Token::Whitespace, Token::Operator, Token::Parameter, Token::Operator};
    QCOMPARE(tokenize("def foo(bar, &tender)"), expectedTokens);
    expectedTokens = { Token::KeywordDef, Token::Whitespace, Token::Method, Token::Whitespace, Token::Operator, Token::Parameter,
                       Token::OperatorComma, Token::Whitespace, Token::Parameter};
    QCOMPARE(tokenize("def foo &bar, tender"), expectedTokens);
}

QTEST_APPLESS_MAIN(TestScanner)
#include "TestScanner.moc"
