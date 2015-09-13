#include "../RubyPlugin.h"
#include "../editor/RubyScanner.h"

#include <QtTest/QtTest>
#include <vector>

namespace Ruby {

Scanner *m_scanner;
typedef QVector<Token::Kind> Tokens;

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
    QString strCode = QLatin1String(code);
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
    m_scanner = 0;
}

void Plugin::test_namespaceIsNotASymbol()
{
    Tokens expectedTokens = { Token::Constant, Token::Operator, Token::Constant, Token::Whitespace, Token::Identifier};
    QCOMPARE(tokenize("Foo::Bar oi"), expectedTokens);
}

void Plugin::test_symbolOnArray()
{
    Tokens expectedTokens = { Token::Identifier, Token::OpenBrackets, Token::Symbol, Token::CloseBrackets };
    QCOMPARE(tokenize("foo[:bar]"), expectedTokens);
}

void Plugin::test_methodDefinition()
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

    expectedTokens = { Token::KeywordDef, Token::Whitespace, Token::Method };
    QCOMPARE(tokenize("def foo!"), expectedTokens);
    QCOMPARE(tokenize("def foo?"), expectedTokens);
    QCOMPARE(tokenize("def foo="), expectedTokens);
    QCOMPARE(tokenize("def <=>"), expectedTokens);
    QCOMPARE(tokenize("def +="), expectedTokens);

    expectedTokens = { Token::KeywordDef, Token::Whitespace, Token::Method, Token::Comment };
    QCOMPARE(tokenize("def foo# comment"), expectedTokens);
    expectedTokens = { Token::KeywordDef, Token::Whitespace, Token::Method, Token::Whitespace, Token::Parameter };
    QCOMPARE(tokenize("def foo oi"), expectedTokens);
    expectedTokens = { Token::KeywordDef, Token::Whitespace, Token::Method, Token::Operator, Token::Parameter,
                       Token::Operator };
    QCOMPARE(tokenize("def foo(oi)"), expectedTokens);
    expectedTokens = { Token::KeywordDef, Token::Whitespace, Token::Method, Token::Parameter };
    QCOMPARE(tokenize("def foo!bar"), expectedTokens);

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

void Plugin::test_strings()
{
    Tokens expectedTokens = { Token::Backtick };
    QCOMPARE(tokenize("`Nice \"backtikc\" son`"), expectedTokens);
    expectedTokens = { Token::Backtick, Token::Backtick };
    QCOMPARE(tokenize("`Nice \"backt \\\nikc\" son`"), expectedTokens);
    expectedTokens = { Token::String };
    QCOMPARE(tokenize("\"Nice \\\"escape!\""), expectedTokens);
}

void Plugin::test_inStringCode()
{
    Tokens expectedTokens = { Token::Backtick, Token::InStringCode, Token::Backtick };
    QCOMPARE(tokenize("`Nice #{Hello}`"), expectedTokens);
    expectedTokens = { Token::String };
    QCOMPARE(tokenize("'Nice #{Hello}'"), expectedTokens);
    expectedTokens = { Token::String, Token::InStringCode, Token::String };
    QCOMPARE(tokenize("\"#{foo}bar"), expectedTokens);

}

void Plugin::test_percentageNotation()
{
    Tokens expectedTokens = { Token::String };
    QCOMPARE(tokenize("%(Hello)"), expectedTokens);
    QCOMPARE(tokenize("%w/Hello asas/"), expectedTokens);
    expectedTokens = { Token::Operator, Token::Number };
    QCOMPARE(tokenize("%2"), expectedTokens);
    expectedTokens = { Token::String, Token::OperatorDot, Token::Identifier };
    QCOMPARE(tokenize("%w(a b).length"), expectedTokens);
}

} // namespace Ruby
