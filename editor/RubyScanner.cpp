#include "RubyScanner.h"

#include <QRegExp>
#include <QString>
#include <QSet>
#include <QDebug>

#include <cstring>

namespace Ruby {

static const char *const RUBY_KEYWORDS[] = {
    "BEGIN",
    "END",
    "__ENCODING__",
    "__END__",
    "__FILE__",
    "__LINE__",
    "alias",
    "and",
    "break",
    "defined?",
    "false",
    "in",
    "next",
    "nil",
    "not",
    "or",
    "raise",
    "redo",
    "require",
    "retry",
    "return",
    "self",
    "super",
    "then",
    "true",
    "undef",
    "when",
    "yield"
};

static const int N_KEYWORDS = std::extent<decltype(RUBY_KEYWORDS)>::value;

#define SELF_DOT_PATTERN "(16_(2_)?18_(2_)?)?"
#define METHOD_PATTERN "15_2_" SELF_DOT_PATTERN
#define CLASS_MODULE_PATTERN "(19|20)_2_" SELF_DOT_PATTERN
//                                 if        ;if         a=if
#define FLOWCTL_SHOULD_INC_INDENT  "^(2_)?21_|26_(2_)?21_|25_(2_)?21_"
// Version without 21_ at end, used on readIdentifier
#define FLOWCTL_SHOULD_INC_INDENT2 "^(2_)?" "|26_(2_)?" "|25_(2_)?"
#define INDENT_INC "(" CLASS_MODULE_PATTERN "|" METHOD_PATTERN "|" FLOWCTL_SHOULD_INC_INDENT "|22_|23_|28_|30_)"

QRegExp Scanner::m_methodPattern(QLatin1String(METHOD_PATTERN "$"));
//                                            METHOD  (          &        parameter,         &
QRegExp Scanner::m_parameterPattern(QLatin1String(METHOD_PATTERN "8_(2_)?(3_)?((2_)?(3_)?(2_)?9_(2_)?(17_)?(2_)?(3_)?(2_)?)*$"));
QRegExp Scanner::m_contextPattern(QLatin1String(CLASS_MODULE_PATTERN "$"));
QRegExp Scanner::m_controlFlowShouldIncIndentPattern(QLatin1String("(" FLOWCTL_SHOULD_INC_INDENT2 ")$"));

static bool isLineFeed(QChar ch)
{
    return ch == QLatin1Char('\n');
}

Scanner::Scanner(const QString *text)
    : m_src(text)
    , m_state(0)
    , m_hasContextRecognition(false)
    , m_line(1)
    , m_lineStartOffset(0)
    , m_indentDepth(0)
{
}

void Scanner::enableContextRecognition()
{
    m_hasContextRecognition = true;
}

void Scanner::setState(int state)
{
    m_state = state;
}

int Scanner::state() const
{
    return m_state;
}

Token Scanner::read()
{
    m_src.setAnchor();
    if (m_src.isEnd())
        return Token(Token::EndOfBlock, m_src.anchor(), 0);

    State state;
    QChar saved;
    parseState(state, saved);
    switch (state) {
    case State_String:
        return readStringLiteral(saved, true);
    default:
        return onDefaultState();
    }
}

void Scanner::readLine()
{
    Token token;
    while ((token = read()).kind != Token::EndOfBlock);
}

Token Scanner::tokenAt(const QString* line, int position)
{
    Scanner scanner(line);
    Token token;
    while ((token = scanner.read()).kind != Token::EndOfBlock) {
        if (position > token.position && (token.position + token.length) > position)
            break;
    }
    return token;
}

QString Scanner::contextName() const
{
    return m_context.join(QLatin1String("::"));
}

int Scanner::indentVariation() const
{
    static const QRegExp indent(QLatin1String(INDENT_INC));
    static const QRegExp unindent(QLatin1String("24_|29_|31_"));
    int delta = 0;

    int offset = -1;
    while ((offset = indent.indexIn(m_tokenSequence, offset + 1)) != -1)
        delta++;
    offset = -1;
    while ((offset = unindent.indexIn(m_tokenSequence, offset + 1)) != -1)
        delta--;

    return delta;
}

bool Scanner::didBlockInterrupt()
{
    static const QRegExp regex(QLatin1String("27_"));
    return regex.indexIn(m_tokenSequence) != -1;
}

Token Scanner::onDefaultState()
{
    QChar first = m_src.peek();
    m_src.move();

    // Ignore new lines
    bool hasNewLine = false;
    while (isLineFeed(first)) {
        hasNewLine = true;
        m_line++;
        m_lineStartOffset = m_src.position();
        first = m_src.peek();
        m_src.setAnchor();
        m_src.move();
    }
    if (hasNewLine)
        m_tokenSequence.clear();

    Token token;

    if (first.isDigit()) {
        token = readFloatNumber();
    } else if (first == QLatin1Char('\'') || first == QLatin1Char('\"') || first == QLatin1Char('`')) {
        token = readStringLiteral(first);
    } else if (m_methodPattern.indexIn(m_tokenSequence) != -1) {
        token = readMethodDefinition();
    } else if (first.isLetter() || first == QLatin1Char('_') || first == QLatin1Char('@')
               || first == QLatin1Char('$') || (first == QLatin1Char(':') && m_src.peek() != QLatin1Char(':'))) {
        token = readIdentifier();
    } else if (first.isDigit()) {
        token = readNumber();
    } else if (first == QLatin1Char('#')) {
        token = readComment();
    } else if (first == QLatin1Char('/')) {
        token = readRegexp();
    } else if (first.isSpace()) {
        token = readWhiteSpace();
    } else if (first == QLatin1Char(',')) {
        token = Token(Token::OperatorComma, m_src.anchor(), m_src.length());
    } else if (first == QLatin1Char('.')) {
        token = Token(Token::OperatorDot, m_src.anchor(), m_src.length());
    } else if (first == QLatin1Char('=') && m_src.peek() != QLatin1Char('=')) {
        token = Token(Token::OperatorAssign, m_src.anchor(), m_src.length());
    } else if (first == QLatin1Char(';')) {
        token = Token(Token::OperatorSemiColon, m_src.anchor(), m_src.length());
    } else if (first == QLatin1Char('%')) {
        token = readPercentageNotation();
    } else if (first == QLatin1Char('{')) {
        token = Token(Token::OpenBraces, m_src.anchor(), m_src.length());
    } else if (first == QLatin1Char('}')) {
        token = Token(Token::CloseBraces, m_src.anchor(), m_src.length());
    } else if (first == QLatin1Char('[')) {
        token = Token(Token::OpenBrackets, m_src.anchor(), m_src.length());
    } else if (first == QLatin1Char(']')) {
        token = Token(Token::CloseBrackets, m_src.anchor(), m_src.length());
    } else {
        token = readOperator(first);
    }

    m_tokenSequence += QString::number(token.kind);
    m_tokenSequence += QLatin1Char('_');

    return token;
}

static Token::Kind tokenKindFor(QChar ch)
{
    switch(ch.toLatin1()) {
    case '`':
        return Token::Backtick;
    case '\'':
    case '"':
    default:
        return Token::String;
    }
}

Token Scanner::readStringLiteral(QChar quoteChar, bool stateRestored)
{
    QChar ch = m_src.peek();

    if (stateRestored && quoteChar != QLatin1Char('\'') && ch == QLatin1Char('#') && m_src.peek(1) == QLatin1Char('{')) {
        m_src.move();
        m_src.move();
        ch = m_src.peek();
        while (ch != QLatin1Char('}') && !ch.isNull()) {
            m_src.move();
            ch = m_src.peek();
        }
        m_src.move();
        return Token(Token::InStringCode, m_src.anchor(), m_src.length());
    }

    if (isLineFeed(ch)) {
        m_src.move();
        ch = m_src.peek();
        m_src.setAnchor();
        m_line++;
    }

    while (ch != quoteChar) {
        if (ch == QLatin1Char('\\')) {
            m_src.move();
            ch = m_src.peek();
            m_src.move();
            if (isLineFeed(ch) || ch.isNull()) {
                saveState(State_String, quoteChar);
                break;
            }
            ch = m_src.peek();
        } else if (quoteChar != QLatin1Char('\'') && ch == QLatin1Char('#') && m_src.peek(1) == QLatin1Char('{')) {
            saveState(State_String, quoteChar);
            break;
        } else if (isLineFeed(ch) || ch.isNull()) {
            saveState(State_String, quoteChar);
            break;
        } else {
            m_src.move();
            ch = m_src.peek();
        }
    }

    if (ch == quoteChar) {
        clearState();
        m_src.move();
    }

    return Token(tokenKindFor(quoteChar), m_src.anchor(), m_src.length());
}

Token Scanner::readRegexp()
{
    const QChar slash = QLatin1Char('/');
    const QChar backSlash = QLatin1Char('\\');
    QChar ch = m_src.peek();
    while (ch != slash && !ch.isNull()) {
        m_src.move();
        ch = m_src.peek();
        if (ch == backSlash) {
            m_src.move();
            m_src.move();
            ch = m_src.peek();
        }
    }
    m_src.move();

    // Read modifiers
    ch = m_src.peek();
    while (ch.isLetter() && ch.isLower()) {
        m_src.move();
        ch = m_src.peek();
    }
    return Token(Token::Regexp, m_src.anchor(), m_src.length());
}

/**
  reads identifier and classifies it
  */
Token Scanner::readIdentifier()
{
    QChar ch = m_src.peek();
    while (ch.isLetterOrNumber() || ch == QLatin1Char('_') || ch == QLatin1Char('?') || ch == QLatin1Char('!')) {
        m_src.move();
        ch = m_src.peek();
    }
    QStringRef value = m_src.value();

    Token::Kind kind = Token::Identifier;
    if (value.at(0) == QLatin1Char('@')) {
        kind = Token::ClassField;
    } else if (value.length() > 1 && value.at(0) == QLatin1Char(':')) {
        kind = Token::Symbol;
    } else if (value.at(0) == QLatin1Char('$')) {
        kind = Token::Global;
    } else if (value.at(0).isUpper()) {
        kind = Token::Constant;
        if (m_hasContextRecognition && m_contextPattern.indexIn(m_tokenSequence) != -1) {
            m_context << value.toString();
            m_contextDepths << m_indentDepth;
        }
    // TODO: Use gperf for this keywords hash
    } else if (value == QLatin1String("end")) {
        kind = Token::KeywordEnd;
        m_indentDepth--;
        if (!m_contextDepths.empty() && m_indentDepth < m_contextDepths.last()) {
            m_context.pop_back();
            m_contextDepths.pop_back();
        }
    } else if (value == QLatin1String("self")) {
        kind = Token::KeywordSelf;
    } else if (value == QLatin1String("def")) {
        kind = Token::KeywordDef;
        m_indentDepth++;
    } else if (value == QLatin1String("module")) {
        kind = Token::KeywordModule;
        m_indentDepth++;
    } else if (value == QLatin1String("class")) {
        kind = Token::KeywordClass;
        m_indentDepth++;
    } else if (value == QLatin1String("if") || value == QLatin1String("unless")) {
        kind = Token::KeywordFlowControl;
        if (m_controlFlowShouldIncIndentPattern.indexIn(m_tokenSequence) != -1)
            m_indentDepth++;
    } else if (value == QLatin1String("while") || value == QLatin1String("until")) {
        kind = Token::KeywordLoop;
        m_indentDepth++;
    } else if (value == QLatin1String("do") || value == QLatin1String("begin") || value == QLatin1String("case")) {
        kind = Token::KeywordBlockStarter;
        m_indentDepth++;
    } else if (value == QLatin1String("else")
               || value == QLatin1String("elsif")
               || value == QLatin1String("ensure")
               || value == QLatin1String("rescue")) {
        kind = Token::KeywordElseElsIfRescueEnsure;
    } else if (value == QLatin1String("protected") || value == QLatin1String("private") || value == QLatin1String("public")) {
        kind = Token::KeywordVisibility;
    } else if (std::find(&RUBY_KEYWORDS[0], &RUBY_KEYWORDS[N_KEYWORDS], value.toUtf8()) != &RUBY_KEYWORDS[N_KEYWORDS]) {
        kind = Token::Keyword;
    } else if (m_methodPattern.indexIn(m_tokenSequence) != -1) {
        QChar ch = m_src.peek();
        while (!ch.isNull() && !ch.isSpace() && ch != QLatin1Char('(') &&  ch != QLatin1Char('#')) {
            m_src.move();
            ch = m_src.peek();
        }
        kind = Token::Method;
    } else if (m_parameterPattern.indexIn(m_tokenSequence) != -1) {
        kind = Token::Parameter;
    }

    return Token(kind, m_src.anchor(), m_src.length());
}

inline static bool isHexDigit(QChar ch)
{
    return (ch.isDigit()
            || (ch >= QLatin1Char('a') && ch <= QLatin1Char('f'))
            || (ch >= QLatin1Char('A') && ch <= QLatin1Char('F')));
}

inline static bool isOctalDigit(QChar ch)
{
    return (ch.isDigit() && ch != QLatin1Char('8') && ch != QLatin1Char('9'));
}

inline static bool isBinaryDigit(QChar ch)
{
    return (ch == QLatin1Char('0') || ch == QLatin1Char('1'));
}

inline static bool isValidIntegerSuffix(QChar ch)
{
    return (ch == QLatin1Char('l') || ch == QLatin1Char('L'));
}

Token Scanner::readNumber()
{
    if (!m_src.isEnd()) {
        QChar ch = m_src.peek();
        if (ch.toLower() == QLatin1Char('b')) {
            m_src.move();
            while (isBinaryDigit(m_src.peek()))
                m_src.move();
        } else if (ch.toLower() == QLatin1Char('o')) {
            m_src.move();
            while (isOctalDigit(m_src.peek()))
                m_src.move();
        } else if (ch.toLower() == QLatin1Char('x')) {
            m_src.move();
            while (isHexDigit(m_src.peek()))
                m_src.move();
        } else { // either integer or float number
            return readFloatNumber();
        }
        if (isValidIntegerSuffix(m_src.peek()))
            m_src.move();
    }
    return Token(Token::Number, m_src.anchor(), m_src.length());
}

Token Scanner::readFloatNumber()
{
    enum
    {
        State_INTEGER,
        State_FRACTION,
        State_EXPONENT
    } state;
    state = State_INTEGER;
    bool hadFraction = false;

    for (;;) {
        QChar ch = m_src.peek();
        if (ch.isNull())
            break;

        if (state == State_INTEGER) {
            if (ch == QLatin1Char('.') && m_src.peek(1).isDigit() && !hadFraction) {
                m_src.move();
                hadFraction = true;
                state = State_FRACTION;
            } else if (!ch.isDigit()) {
                break;
            }
        } else if (state == State_FRACTION) {
            if (ch == QLatin1Char('e') || ch == QLatin1Char('E')) {
                QChar next = m_src.peek(1);
                QChar next2 = m_src.peek(2);
                bool isExp = next.isDigit()
                        || (((next == QLatin1Char('-')) || (next == QLatin1Char('+'))) && next2.isDigit());
                if (isExp) {
                    m_src.move();
                    state = State_EXPONENT;
                } else {
                    break;
                }
            } else if (!ch.isDigit()) {
                break;
            }
        } else if (!ch.isDigit()) {
            break;
        }
        m_src.move();
    }

    return Token(Token::Number, m_src.anchor(), m_src.length());
}

/**
  reads single-line ruby comment, started with "#"
  */
Token Scanner::readComment()
{
    QChar ch = m_src.peek();
    while (!isLineFeed(ch) && !ch.isNull()) {
        m_src.move();
        ch = m_src.peek();
    }
    return Token(Token::Comment, m_src.anchor(), m_src.length());
}

/**
  reads whitespace
  */
Token Scanner::readWhiteSpace()
{
    QChar chr = m_src.peek();
    while (chr.isSpace() && !isLineFeed(chr)) {
        m_src.move();
        chr = m_src.peek();
    }
    return Token(Token::Whitespace, m_src.anchor(), m_src.length());
}

/**
  reads punctuation symbols, excluding some special
  */
Token Scanner::readOperator(QChar first)
{
    static const QString singleCharOperators = QStringLiteral("[]{}()");
    if (singleCharOperators.contains(first))
        return Token(Token::Operator, m_src.anchor(), m_src.length());

    static const QString operators = QStringLiteral("<=>+-/*%!");
    static const QString colon = QStringLiteral(":");
    const QString &acceptedChars = first == QLatin1Char(':') ? colon : operators;
    QChar ch = m_src.peek();

    while (acceptedChars.contains(ch)) {
        m_src.move();
        ch = m_src.peek();
    }
    return Token(Token::Operator, m_src.anchor(), m_src.length());
}

static QChar translateDelimiter(QChar ch)
{
    switch (ch.toLatin1()) {
    case '(': return QLatin1Char(')');
    case '[': return QLatin1Char(']');
    case '{': return QLatin1Char('}');
    case '<': return QLatin1Char('>');
    default: return ch;
    }
}

Token Scanner::readPercentageNotation()
{
    QChar ch = m_src.peek();
    if (ch.isSpace() || ch.isDigit())
        return Token(Token::Operator, m_src.anchor(), m_src.length());

    if (ch.isLetter()) // Don't care if the user wrote the wrong % modifier.
        m_src.move();
    QChar delimiter = translateDelimiter(m_src.peek());
    m_src.move();
    return readStringLiteral(delimiter, false);
}

Token Scanner::readMethodDefinition()
{
    consumeUntil("(#", "!?");
    return Token(Token::Method, m_src.anchor(), m_src.length());
}

void Scanner::consumeUntil(const char* stopAt, const char* stopAfter)
{
    QChar ch = m_src.peek();
    while (!ch.isNull() && !ch.isSpace() && !std::strchr(stopAt, ch.toLatin1())) {
        m_src.move();
        ch = m_src.peek();
        if (!ch.isNull() && std::strchr(stopAfter, ch.toLatin1())) {
            m_src.move();
            break;
        }
    }
}

void Scanner::clearState()
{
    m_state = 0;
}

void Scanner::saveState(State state, QChar savedData)
{
    m_state = (state << 16) | static_cast<int>(savedData.unicode());
}

void Scanner::parseState(State &state, QChar &savedData) const
{
    state = static_cast<State>((m_state >> 16) & 0xf);
    savedData = m_state & 0xffff;
}
}
