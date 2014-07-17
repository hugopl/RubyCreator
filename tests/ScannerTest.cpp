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
    case Token::Backtick: str = "Backtick"; break;
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

    tokenize("module Foo\n"
             "  class Bar");
    QCOMPARE(m_scanner->contextName(), QStringLiteral("Foo::Bar"));

    tokenize("module FooBar\n"
             "  class Klass\n"
             "end");
    QCOMPARE(m_scanner->contextName(), QStringLiteral("FooBar"));

    tokenize("module Foo\n"
             "  class Klass\n"
             "    def foo\n"
             "      a = 2 if b"
             "    end"
             "  end");
    QCOMPARE(m_scanner->contextName(), QStringLiteral("Foo"));
}

void TestScanner::indentIf()
{
    tokenize("if foo;end");
    QVERIFY(m_scanner->didBlockStart());
    QVERIFY(m_scanner->didBlockEnd());
    tokenize("if foo");
    QVERIFY(m_scanner->didBlockStart());
    tokenize("a = 2 if foo");
    QVERIFY(!m_scanner->didBlockStart());
    tokenize("a = 2;if foo");
    QVERIFY(m_scanner->didBlockStart());
    // ugliest code style ever
    tokenize("a = if foo");
    QVERIFY(m_scanner->didBlockStart());

    // Weird code can show folding mark, but I don't care about show weird code
    // like "if foo; bar; end; if bleh" this will not be folded or indented correctly
}

void TestScanner::lineCount()
{
    tokenize("\nif foo\n\nend");
    QCOMPARE(m_scanner->currentLine(), 4);
}

void TestScanner::ifs()
{
    tokenize("class Foo\n"
             "  def method\n"
             "    something do\n"
             "      if foo\n"
             "        otherthing\n"
             "      end\n"
             "    end\n"
             "  end");
    QCOMPARE(m_scanner->contextName(), QStringLiteral("Foo"));
}

void TestScanner::backtick()
{
    Tokens expectedTokens = { Token::Backtick };
    QCOMPARE(tokenize("`Nice \"backtikc\" son`"), expectedTokens);
}

QTEST_APPLESS_MAIN(TestScanner)
#include "TestScanner.moc"
