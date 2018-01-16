#include "RubyScanner.h"

#include <QRegularExpression>
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

QChar translateDelimiter(QChar ch)
{
    switch (ch.toLatin1()) {
    case '(': return ')';
    case '[': return ']';
    case '{': return '}';
    case '<': return '>';
    case ')': return '(';
    case ']': return '[';
    case '}': return '{';
    case '>': return '<';
    default: return ch;
    }
}

static bool delimiterHasPair(QChar ch)
{
    switch (ch.toLatin1()) {
    case '(':
    case '[':
    case '{':
    case '<':
    case ')':
    case ']':
    case '}':
    case '>':
        return true;
    default:
        return false;
    }
}

#define SELF_DOT_PATTERN "(16_(2_)?18_(2_)?)?"
#define METHOD_PATTERN "15_2_" SELF_DOT_PATTERN
#define CLASS_MODULE_PATTERN "(19|20)_2_" SELF_DOT_PATTERN
//                                 if        ;if         a=if
#define FLOWCTL_SHOULD_INC_INDENT  "^(2_)?21_|26_(2_)?21_|25_(2_)?21_"
// Version without 21_ at end, used on readIdentifier
#define FLOWCTL_SHOULD_INC_INDENT2 "^(2_)?" "|26_(2_)?" "|25_(2_)?"
#define INDENT_INC "(" CLASS_MODULE_PATTERN "|" METHOD_PATTERN "|" FLOWCTL_SHOULD_INC_INDENT "|22_|23_|28_|30_)"

static const QRegularExpression m_methodPattern(METHOD_PATTERN "$");
//                                            METHOD  (          &        parameter,         &
static const QRegularExpression m_parameterPattern(
        METHOD_PATTERN "8_(2_)?(3_)?((2_)?(3_)?(2_)?9_(2_)?(17_)?(2_)?(3_)?(2_)?)*$");
static const QRegularExpression m_contextPattern(CLASS_MODULE_PATTERN "$");
static const QRegularExpression m_controlFlowShouldIncIndentPattern("(" FLOWCTL_SHOULD_INC_INDENT2 ")$");

static bool isLineFeed(QChar ch)
{
    return ch == '\n';
}

Scanner::Scanner(const QString *text)
    : m_src(text)
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
    case State_Regexp:
    case State_Symbols:
        return readStringLiteral(saved, state);
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
        if (position > token.position && (token.position + token.length) >= position)
            break;
    }
    return token;
}

QString Scanner::contextName() const
{
    return m_context.join("::");
}

int Scanner::indentVariation() const
{
    static const QRegularExpression indent(INDENT_INC);
    static const QRegularExpression unindent("24_|29_|31_");
    int delta = 0;

    int offset = -1;
    while ((offset = m_tokenSequence.indexOf(indent, offset + 1)) != -1)
        delta++;
    offset = -1;
    while ((offset = m_tokenSequence.indexOf(unindent, offset + 1)) != -1)
        delta--;

    return delta;
}

bool Scanner::didBlockInterrupt()
{
    static const QRegularExpression regex("27_");
    return regex.match(m_tokenSequence).hasMatch();
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
    } else if (first == '\'' || first == '\"' || first == '`') {
        token = readStringLiteral(first, State_String);
    } else if (m_methodPattern.match(m_tokenSequence).hasMatch()) {
        token = readMethodDefinition();
    } else if (first == '$' && (m_src.peek() == '`' || m_src.peek() == '\'')) {
        m_src.move();
        token = Token(Token::String, m_src.anchor(), m_src.length());
    } else if (first.isLetter() || first == '_' || first == '@'
               || first == '$' || (first == ':' && m_src.peek() != ':')) {
        token = readIdentifier();
    } else if (first.isDigit()) {
        token = readNumber();
    } else if (first == '#') {
        token = readComment();
    } else if (first == '/') {
        token = readRegexp();
    } else if (first.isSpace()) {
        token = readWhiteSpace();
    } else if (first == ',') {
        token = Token(Token::OperatorComma, m_src.anchor(), m_src.length());
    } else if (first == '.') {
        token = Token(Token::OperatorDot, m_src.anchor(), m_src.length());
    } else if (first == '=' && m_src.peek() != '=') {
        token = Token(Token::OperatorAssign, m_src.anchor(), m_src.length());
    } else if (first == ';') {
        token = Token(Token::OperatorSemiColon, m_src.anchor(), m_src.length());
    } else if (first == '%') {
        token = readPercentageNotation();
    } else if (first == '{') {
        token = Token(Token::OpenBraces, m_src.anchor(), m_src.length());
    } else if (first == '}') {
        token = Token(Token::CloseBraces, m_src.anchor(), m_src.length());
    } else if (first == '[') {
        token = Token(Token::OpenBrackets, m_src.anchor(), m_src.length());
    } else if (first == ']') {
        token = Token(Token::CloseBrackets, m_src.anchor(), m_src.length());
        // For historic reasons, ( and ) are the Operator token, this will
        // be changed soon.
    } else if (first == '(' || first == ')') {
        token = Token(Token::Operator, m_src.anchor(), m_src.length());
    } else {
        token = readOperator(first);
    }

    m_tokenSequence += QString::number(token.kind);
    m_tokenSequence += '_';

    return token;
}

static Token::Kind tokenKindFor(QChar ch, Scanner::State state)
{
    if (state == Scanner::State_Regexp)
        return Token::Regexp;
    else if (state == Scanner::State_Symbols)
        return Token::Symbol;

    switch(ch.toLatin1()) {
    case '`':
        return Token::Backtick;
    case '\'':
    case '"':
    default:
        return Token::String;
    }
}

Token Scanner::readStringLiteral(QChar quoteChar, Scanner::State state)
{
    QChar ch = m_src.peek();

    if (ch == '#' && m_src.peek(1) == '{') {
        if (m_src.length()) {
            saveState(state, quoteChar);
            return Token(tokenKindFor(quoteChar, state), m_src.anchor(), m_src.length());
        }
        return readInStringToken();
    }

    if (isLineFeed(ch)) {
        m_src.move();
        ch = m_src.peek();
        m_src.setAnchor();
        m_line++;
    }

    QChar startQuoteChar = translateDelimiter(quoteChar);
    bool quoteCharHasPair = delimiterHasPair(quoteChar);
    int bracketCount = 0;

    forever {
        ch = m_src.peek();
        if (isLineFeed(ch) || ch.isNull()) {
            saveState(state, quoteChar);
            break;
        }

        if (ch == quoteChar && bracketCount == 0)
            break;

        // handles %r{{}}
        if (quoteCharHasPair) {
            if (ch == startQuoteChar) {
                bracketCount++;
                m_src.move();
                continue;
            } else if (ch == quoteChar) {
                bracketCount--;
                m_src.move();
                continue;
            }
        }

        if (ch == '\\') {
            m_src.move();
            ch = m_src.peek();
            m_src.move();
            if (isLineFeed(ch) || ch.isNull()) {
                saveState(state, quoteChar);
                break;
            }
        } else if (quoteChar != '\'' && ch == '#' && m_src.peek(1) == '{') {
            saveState(state, quoteChar);
            break;
        } else if (isLineFeed(ch) || ch.isNull()) {
            saveState(state, quoteChar);
            break;
        } else {
            m_src.move();
        }
    }

    if (ch == quoteChar) {
        m_src.move();
        if (state == State_Regexp)
            consumeRegexpModifiers();
        clearState();
    }

    return Token(tokenKindFor(quoteChar, state), m_src.anchor(), m_src.length());
}

Token Scanner::readInStringToken()
{
    m_src.move();
    m_src.move();
    QChar ch = m_src.peek();
    while (ch != '}' && !ch.isNull()) {
        m_src.move();
        ch = m_src.peek();
    }
    m_src.move();
    return Token(Token::InStringCode, m_src.anchor(), m_src.length());
}

Token Scanner::readRegexp()
{
    const QChar slash = '/';
    const QChar backSlash = '\\';
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
    consumeRegexpModifiers();

    return Token(Token::Regexp, m_src.anchor(), m_src.length());
}

void Scanner::consumeRegexpModifiers()
{
    QChar ch = m_src.peek();
    while (ch.isLetter() && ch.isLower()) {
        m_src.move();
        ch = m_src.peek();
    }
}

/**
  reads identifier and classifies it
  */
Token Scanner::readIdentifier()
{
    QChar ch = m_src.peek();
    while (ch.isLetterOrNumber() || ch == '_' || ch == '?' || ch == '!') {
        m_src.move();
        ch = m_src.peek();
    }
    QStringRef value = m_src.value();

    Token::Kind kind = Token::Identifier;
    if (m_src.peek() == ':' && m_src.peek(1) != ':') {
        m_src.move();
        kind = Token::SymbolHashKey;
    } else if (value.at(0) == '@') {
        kind = Token::ClassField;
    } else if (value.length() > 1 && value.at(0) == ':') {
        kind = Token::Symbol;
    } else if (value.at(0) == '$') {
        kind = Token::Global;
    } else if (value.at(0).isUpper()) {
        kind = Token::Constant;
        if (m_hasContextRecognition && m_contextPattern.match(m_tokenSequence).hasMatch()) {
            m_context << value.toString();
            m_contextDepths << m_indentDepth;
        }
    // TODO: Use gperf for this keywords hash
    } else if (value == "end") {
        kind = Token::KeywordEnd;
        m_indentDepth--;
        if (!m_contextDepths.empty() && m_indentDepth < m_contextDepths.last()) {
            m_context.pop_back();
            m_contextDepths.pop_back();
        }
    } else if (value == "self") {
        kind = Token::KeywordSelf;
    } else if (value == "def") {
        kind = Token::KeywordDef;
        m_indentDepth++;
    } else if (value == "module") {
        kind = Token::KeywordModule;
        m_indentDepth++;
    } else if (value == "class") {
        kind = Token::KeywordClass;
        m_indentDepth++;
    } else if (value == "if" || value == "unless") {
        kind = Token::KeywordFlowControl;
        if (m_controlFlowShouldIncIndentPattern.match(m_tokenSequence).hasMatch())
            m_indentDepth++;
    } else if (value == "while" || value == "until") {
        kind = Token::KeywordLoop;
        m_indentDepth++;
    } else if (value == "do" || value == "begin" || value == "case") {
        kind = Token::KeywordBlockStarter;
        m_indentDepth++;
    } else if (value == "else"
               || value == "elsif"
               || value == "ensure"
               || value == "rescue") {
        kind = Token::KeywordElseElsIfRescueEnsure;
    } else if (value == "protected" || value == "private" || value == "public") {
        kind = Token::KeywordVisibility;
    } else if (std::find(&RUBY_KEYWORDS[0], &RUBY_KEYWORDS[N_KEYWORDS], value.toUtf8()) != &RUBY_KEYWORDS[N_KEYWORDS]) {
        kind = Token::Keyword;
    } else if (m_methodPattern.match(m_tokenSequence).hasMatch()) {
        QChar ch = m_src.peek();
        while (!ch.isNull() && !ch.isSpace() && ch != '(' &&  ch != '#') {
            m_src.move();
            ch = m_src.peek();
        }
        kind = Token::Method;
    } else if (m_parameterPattern.match(m_tokenSequence).hasMatch()) {
        kind = Token::Parameter;
    }

    return Token(kind, m_src.anchor(), m_src.length());
}

inline static bool isHexDigit(QChar ch)
{
    return (ch.isDigit()
            || (ch >= 'a' && ch <= 'f')
            || (ch >= 'A' && ch <= 'F'));
}

inline static bool isOctalDigit(QChar ch)
{
    return (ch.isDigit() && ch != '8' && ch != '9');
}

inline static bool isBinaryDigit(QChar ch)
{
    return (ch == '0' || ch == '1');
}

inline static bool isValidIntegerSuffix(QChar ch)
{
    return (ch == 'l' || ch == 'L');
}

Token Scanner::readNumber()
{
    if (!m_src.isEnd()) {
        QChar ch = m_src.peek();
        if (ch.toLower() == 'b') {
            m_src.move();
            while (isBinaryDigit(m_src.peek()))
                m_src.move();
        } else if (ch.toLower() == 'o') {
            m_src.move();
            while (isOctalDigit(m_src.peek()))
                m_src.move();
        } else if (ch.toLower() == 'x') {
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
            if (ch == '.' && m_src.peek(1).isDigit() && !hadFraction) {
                m_src.move();
                hadFraction = true;
                state = State_FRACTION;
            } else if (!ch.isDigit()) {
                break;
            }
        } else if (state == State_FRACTION) {
            if (ch == 'e' || ch == 'E') {
                QChar next = m_src.peek(1);
                QChar next2 = m_src.peek(2);
                bool isExp = next.isDigit()
                        || (((next == '-') || (next == '+')) && next2.isDigit());
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
    static const QString operators = "<=>+-/*%!";
    static const QString colon = ":";
    const QString &acceptedChars = first == ':' ? colon : operators;
    QChar ch = m_src.peek();

    while (acceptedChars.contains(ch)) {
        m_src.move();
        ch = m_src.peek();
    }
    return Token(Token::Operator, m_src.anchor(), m_src.length());
}

Token Scanner::readPercentageNotation()
{
    QChar ch = m_src.peek();
    if (ch.isSpace() || ch.isDigit())
        return Token(Token::Operator, m_src.anchor(), m_src.length());

    State state = State_String;
    if (ch.isLetter()) {
        if (ch == 'r')
            state = State_Regexp;
        if (ch == 'i')
            state = State_Symbols;
        m_src.move();
    }
    QChar delimiter = translateDelimiter(m_src.peek());
    m_src.move();
    return readStringLiteral(delimiter, state);
}

Token Scanner::readMethodDefinition()
{
    consumeUntil(".(#", "!?");
    QStringRef value = m_src.value();
    if (value == "self")
        return Token(Token::KeywordSelf, m_src.anchor(), m_src.length());

    if (!std::strchr("!?", m_src.peek(-1).toLatin1()))
        consumeUntil("(#", "!?");
    return Token(Token::Method, m_src.anchor(), m_src.length());
}

void Scanner::consumeUntil(const char* stopAt, const char* stopAfter)
{
    QChar ch = m_src.peek();
    while (!ch.isNull() && !ch.isSpace() && !std::strchr(stopAt, ch.toLatin1())) {
        m_src.move();
        ch = m_src.peek();
        if (stopAfter && !ch.isNull() && std::strchr(stopAfter, ch.toLatin1())) {
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
