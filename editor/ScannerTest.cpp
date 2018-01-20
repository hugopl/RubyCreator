#include "../RubyPlugin.h"
#include "../editor/RubyScanner.h"

#include <QtTest/QtTest>
#include <vector>

typedef QVector<Ruby::Token::Kind> Tokens;

struct TestData
{
    TestData() = default;
    TestData(const QByteArray &input, const Tokens &tokens) :
        input(input), tokens(tokens)
    {}

    const QByteArray input;
    const Tokens tokens;

};

Q_DECLARE_METATYPE(TestData);

namespace Ruby {

static Scanner *m_scanner = nullptr;

#define CASE_STR(x) case Token::x: str = #x; break
QDebug &operator<<(QDebug &s, Token::Kind t)
{
    const char *str = "";
    switch (t) {
        CASE_STR(Number);
        CASE_STR(String);
        CASE_STR(Whitespace);
        CASE_STR(Operator);
        CASE_STR(Comment);
        CASE_STR(Identifier);
        CASE_STR(Regexp);
        CASE_STR(Symbol);
        CASE_STR(SymbolHashKey);
        CASE_STR(Method);
        CASE_STR(Parameter);
        CASE_STR(ClassField);
        CASE_STR(Constant);
        CASE_STR(Global);
        CASE_STR(Keyword);
        CASE_STR(KeywordDef);
        CASE_STR(KeywordSelf);
        CASE_STR(OperatorComma);
        CASE_STR(OperatorDot);
        CASE_STR(KeywordClass);
        CASE_STR(KeywordModule);
        CASE_STR(KeywordFlowControl);
        CASE_STR(KeywordLoop);
        CASE_STR(KeywordBlockStarter);
        CASE_STR(KeywordEnd);
        CASE_STR(OperatorAssign);
        CASE_STR(OperatorSemiColon);
        CASE_STR(KeywordElseElsIfRescueEnsure);
        CASE_STR(OpenBraces);
        CASE_STR(CloseBraces);
        CASE_STR(OpenBrackets);
        CASE_STR(CloseBrackets);

        CASE_STR(Backtick);
        CASE_STR(InStringCode);
        CASE_STR(KeywordVisibility);
        case Token::EndOfBlock: str = "EOB"; break;
        // No default. Let the compiler warn when new values are added.
    }
    return s << str;
}

static Tokens tokenize(const QByteArray &code, bool debug = false)
{
    if (m_scanner)
        delete m_scanner;
    QString strCode = QString::fromUtf8(code);
    m_scanner = new Scanner(&strCode);
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

void Plugin::cleanupTestCase()
{
    delete m_scanner;
    m_scanner = nullptr;
}

void Plugin::test_context()
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

void Plugin::test_indentIf()
{
    tokenize("if foo;end");
    QCOMPARE(m_scanner->indentVariation(), 0);
    tokenize("if foo");
    QCOMPARE(m_scanner->indentVariation(), 1);
    tokenize("a = 2 if foo");
    QCOMPARE(m_scanner->indentVariation(), 0);
    tokenize("a = 2;if foo");
    QCOMPARE(m_scanner->indentVariation(), 1);
    // ugliest code style ever
    tokenize("a = if foo");
    QCOMPARE(m_scanner->indentVariation(), 1);

    // Weird code can show folding mark, but I don't care about show weird code
    // like "if foo; bar; end; if bleh" this will not be folded or indented correctly
}

void Plugin::test_indentBraces()
{
    tokenize("foo.bar {|x|");
    QCOMPARE(m_scanner->indentVariation(), 1);

    tokenize("foo.bar {|x| }");
    QCOMPARE(m_scanner->indentVariation(), 0);

    tokenize("def foo a = {}");
    QCOMPARE(m_scanner->indentVariation(), 1);
}

void Plugin::test_lineCount()
{
    tokenize("\nif foo\n\nend");
    QCOMPARE(m_scanner->currentLine(), 4);
}

void Plugin::test_ifs()
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

void Plugin::test_scanner()
{
    QFETCH(TestData, td);
    QEXPECT_FAIL("Brackets + newline", "Not working", Continue);
    QCOMPARE(tokenize(td.input), td.tokens);
}

void Plugin::test_scanner_data()
{
    QTest::addColumn<TestData>("td");

    QTest::newRow("symbol on array")
            << TestData("foo[:bar]", {
                            Token::Identifier,
                            Token::OpenBrackets, Token::Symbol, Token::CloseBrackets });
    QTest::newRow("method def - param after comma")
            << TestData("def foo bar, tender", {
                            Token::KeywordDef, Token::Whitespace, Token::Method, Token::Whitespace,
                            Token::Parameter, Token::OperatorComma, Token::Whitespace, Token::Parameter });
    QTest::newRow("method def - param after WS and comma")
            << TestData("def foo bar  , tender", {
                            Token::KeywordDef, Token::Whitespace, Token::Method, Token::Whitespace,
                            Token::Parameter, Token::Whitespace, Token::OperatorComma,
                            Token::Whitespace, Token::Parameter });
    QTest::newRow("method def - self with param")
            << TestData("def self.foo bar", {
                            Token::KeywordDef, Token::Whitespace, Token::KeywordSelf,
                            Token::OperatorDot, Token::Method, Token::Whitespace, Token::Parameter });
    QTest::newRow("method def - parentheses")
            << TestData("def foo(bar)", {
                            Token::KeywordDef, Token::Whitespace, Token::Method, Token::Operator,
                            Token::Parameter, Token::Operator });
    QTest::newRow("method def - param and block")
            << TestData("def foo(bar, &tender)", {
                            Token::KeywordDef, Token::Whitespace, Token::Method, Token::Operator,
                            Token::Parameter, Token::OperatorComma, Token::Whitespace,
                            Token::Operator, Token::Parameter, Token::Operator });
    QTest::newRow("method def - block and param, no parens")
            << TestData("def foo &bar, tender", {
                            Token::KeywordDef, Token::Whitespace, Token::Method, Token::Whitespace,
                            Token::Operator, Token::Parameter, Token::OperatorComma,
                            Token::Whitespace, Token::Parameter });

    const Tokens simpleFunc{ Token::KeywordDef, Token::Whitespace, Token::Method };
    QTest::newRow("method excl") << TestData("def foo!", simpleFunc);
    QTest::newRow("method question") << TestData("def foo?", simpleFunc);
    QTest::newRow("method equals") << TestData("def foo=", simpleFunc);
    QTest::newRow("method spaceship") << TestData("def <=>", simpleFunc);
    QTest::newRow("method plus-equals") << TestData("def +=", simpleFunc);
    QTest::newRow("method with comment")
            << TestData("def foo# comment", {
                            Token::KeywordDef, Token::Whitespace, Token::Method, Token::Comment });
    QTest::newRow("method with param no paren")
            << TestData("def foo oi", {
                            Token::KeywordDef, Token::Whitespace, Token::Method,
                            Token::Whitespace, Token::Parameter });
    QTest::newRow("method with param with paren")
            << TestData("def foo(oi)", {
                            Token::KeywordDef, Token::Whitespace, Token::Method, Token::Operator,
                            Token::Parameter, Token::Operator });
    QTest::newRow("method excl with param")
            << TestData("def foo!bar", { Token::KeywordDef, Token::Whitespace, Token::Method, Token::Parameter });
    QTest::newRow("namespace is not a symbol")
            << TestData("Foo::Bar oi", {
                            Token::Constant, Token::Operator, Token::Constant,
                            Token::Whitespace, Token::Identifier });
    QTest::newRow("Backtick") << TestData("`Nice \"backtick\" son`", { Token::Backtick });
    QTest::newRow("Newline in Backtick")
            << TestData("`Nice \"backt \\\nick\" son`", { Token::Backtick, Token::Backtick });
    QTest::newRow("Escpae in string") << TestData("\"Nice \\\"escape!\"", { Token::String });
    QTest::newRow("Single-quote string") << TestData("'Nice #{Hello}'", { Token::String });
    QTest::newRow("In string code")
            << TestData("\"#{foo}bar\"", { Token::String, Token::InStringCode, Token::String });
    QTest::newRow("In string code in backtick")
            << TestData("`Nice #{Hello}`", { Token::Backtick, Token::InStringCode, Token::Backtick });
    QTest::newRow("Percentage") << TestData("%(Hello)", { Token::String });
    QTest::newRow("%w") << TestData("%w/Hello asas/", { Token::String });
    QTest::newRow("Percentage operator") << TestData("%2", { Token::Operator, Token::Number });
    QTest::newRow("%w with function call")
            << TestData("%w(a b).length", { Token::String, Token::OperatorDot, Token::Identifier });
    QTest::newRow("Regexp") << TestData("%r{foo/bar}", { Token::Regexp });
    QTest::newRow("Regexp + newline") << TestData("%r{foo\n/bar}x", { Token::Regexp, Token::Regexp });
    QTest::newRow("Brackets 1") << TestData("%r{{}}", { Token::Regexp });
    QTest::newRow("Brackets 2") << TestData("%r<<>>", { Token::Regexp });
    QTest::newRow("Brackets 3") << TestData("%q[[]]", { Token::String });
    QTest::newRow("Brackets 4") << TestData("%w(())", { Token::String });
    QTest::newRow("Brackets 5") << TestData("%q!\\!!", { Token::String });
    QTest::newRow("Brackets + newline") << TestData("%q{{\n}\n}", { Token::String, Token::String });
    QTest::newRow("keyword symbols 1")
            << TestData("if: :foo", { Token::SymbolHashKey, Token::Whitespace, Token::Symbol });
    QTest::newRow("keyword symbols 2")
            << TestData("a= :if", { Token::Identifier, Token::OperatorAssign, Token::Whitespace, Token::Symbol });
}

} // namespace Ruby
