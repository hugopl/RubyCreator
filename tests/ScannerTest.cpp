#include "ScannerTest.h"

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
    case Token::KeywordClass: str = "Keyword-Class"; break;
    case Token::KeywordModule: str = "Keyword-Module"; break;
    case Token::Method: str = "Method"; break;
    case Token::Parameter: str = "Parameter"; break;
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

TestScanner::TestScanner(QObject* parent)
    : QObject(parent)
    , m_scanner(nullptr)
{

}

TestScanner::Tokens TestScanner::tokenize(const QString& code, bool debug)
{
    if (m_scanner)
        delete m_scanner;
    m_scanner = new Scanner(&code);
    m_scanner->enableContextRecognition();

    QVector<Token::Kind> tokens;
    Token token;
    while ((token = m_scanner->read()).kind != Token::EndOfBlock) {
        tokens << token.kind;
        if (debug)
            qDebug() << "> " << token.kind << code.mid(token.position, token.length);
    }
    return tokens;
}

void TestScanner::cleanup()
{
    delete m_scanner;
    m_scanner = nullptr;

}

void TestScanner::namespaceIsNotASymbol()
{
    Tokens expectedTokens = { Token::Constant, Token::Operator, Token::Constant, Token::Whitespace, Token::Identifier};
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

void TestScanner::context()
{
    Tokens expectedTokens = { Token::KeywordClass, Token::Whitespace, Token::Constant };
    QCOMPARE(tokenize("class Foo"), expectedTokens);
    QCOMPARE(m_scanner->contextName(), QStringLiteral("Foo"));

    expectedTokens = { Token::KeywordModule, Token::Whitespace, Token::Constant };
    QCOMPARE(tokenize("module Bar"), expectedTokens);
    QCOMPARE(m_scanner->contextName(), QStringLiteral("Bar"));

    tokenize("module Foo\n class Bar");
    QCOMPARE(m_scanner->contextName(), QStringLiteral("Foo::Bar"));
}

void TestScanner::indentIf()
{
    tokenize("if foo;end");
    QCOMPARE(m_scanner->indentLevel(), 0);
    tokenize("if foo");
    QCOMPARE(m_scanner->indentLevel(), 1);
    tokenize("a = 2 if foo");
    QCOMPARE(m_scanner->indentLevel(), 0);
    tokenize("a = 2;if foo");
    QCOMPARE(m_scanner->indentLevel(), 1);
    // ugliest code style ever
    tokenize("a = if foo");
    QCOMPARE(m_scanner->indentLevel(), 1);
}

QTEST_APPLESS_MAIN(TestScanner)
#include "TestScanner.moc"
