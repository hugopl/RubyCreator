/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#include "RubyScanner.h"

#include <QRegExp>
#include <QString>
#include <QSet>
#include <QDebug>

namespace Ruby {

static const char* const RUBY_KEYWORDS[] = {
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
    "else",
    "elsif",
    "ensure",
    "false",
    "in",
    "next",
    "nil",
    "not",
    "or",
    "redo",
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

Scanner::Scanner(const QString* text)
    : m_src(text)
    , m_state(0)
    , m_hasContextRecognition(false)
    , m_line(1)
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
        return { Token::EndOfBlock, m_src.anchor(), 0 };

    State state;
    QChar saved;
    parseState(state, saved);
    switch (state) {
    case State_String:
        return readStringLiteral(saved);
    case State_MultiLineString:
        return readMultiLineStringLiteral(saved);
    default:
        return onDefaultState();
    }
}

QString Scanner::contextName() const
{
    return m_context.join("::");
}

static int numMatches(const QRegExp& regExp, const QString& str)
{
    int n = 0;
    int pos = 0;
    while ((pos = regExp.indexIn(str, pos)) != -1) {
        ++n;
        pos += regExp.matchedLength();
    }
    return n;
}

int Scanner::indentLevel() const
{
    //                                                              if        ;if         a=if
    static QRegExp indentInc("(" CLASS_MODULE_PATTERN "|" METHOD_PATTERN "|^(2_)?21_|26_(2_)?21_|25_(2_)?21_|22_|23_)");
    int indent = numMatches(indentInc, m_tokenSequence);

    static QRegExp indentDec("_24");
    indent -= numMatches(indentDec, m_tokenSequence);

    return indent;
}

Token Scanner::onDefaultState()
{
    QChar first = m_src.peek();
    m_src.move();

    while(first == QLatin1Char('\n')) {
        m_line++;
        first = m_src.peek();
        m_src.move();
    }

    Token token;

    if (first.isDigit()) {
        token = readFloatNumber();
    } else if (first == QLatin1Char('\'') || first == QLatin1Char('\"')) {
        token = readStringLiteral(first);
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
        token = { Token::OperatorComma, m_src.anchor(), m_src.length() };
    } else if (first == QLatin1Char('.')) {
        token = { Token::OperatorDot, m_src.anchor(), m_src.length() };
    } else if (first == QLatin1Char('=') && m_src.peek() != QLatin1Char('=')) {
        token = { Token::OperatorAssign, m_src.anchor(), m_src.length() };
    } else if (first == QLatin1Char(';')) {
        token = { Token::OperatorSemiColon, m_src.anchor(), m_src.length() };
    } else {
        token = readOperator(first);
    }

    m_tokenSequence += QString::number(token.kind);
    m_tokenSequence += QLatin1Char('_');

    return token;
}

bool Scanner::checkEscapeSequence()
{
    if (m_src.peek() == QLatin1Char('\\')) {
        m_src.move();
        QChar ch = m_src.peek();
        if (ch == QLatin1Char('\n') || ch.isNull())
            return true;
    }
    return false;
}

/**
  reads single-line string literal, surrounded by ' or " quotes
  */
Token Scanner::readStringLiteral(QChar quoteChar)
{
    QChar ch = m_src.peek();
    if (ch == quoteChar && m_src.peek(1) == quoteChar) {
        saveState(State_MultiLineString, quoteChar);
        return readMultiLineStringLiteral(quoteChar);
    }

    while (ch != quoteChar && !ch.isNull()) {
        if (checkEscapeSequence())
            saveState(State_String, quoteChar);
        m_src.move();
        ch = m_src.peek();
    }
    if (ch == quoteChar)
        clearState();
    m_src.move();
    return { Token::String, m_src.anchor(), m_src.length() };
}

Token Scanner::readRegexp()
{
    QLatin1Char slash('/');
    QChar ch = m_src.peek();

    while (ch != slash && !ch.isNull()) {
        checkEscapeSequence();
        m_src.move();
        ch = m_src.peek();
    }
    m_src.move();
    return { Token::Regexp, m_src.anchor(), m_src.length() };
}

/**
  reads multi-line string literal, surrounded by ''' or """ sequencies
  */
Token Scanner::readMultiLineStringLiteral(QChar quoteChar)
{
    for (;;) {
        QChar ch = m_src.peek();
        if (ch.isNull())
            break;
        if (ch == quoteChar
                && (m_src.peek(1) == quoteChar)
                && (m_src.peek(2) == quoteChar)) {
            clearState();
            m_src.move();
            m_src.move();
            m_src.move();
            break;
        }
        m_src.move();
    }

    return { Token::String, m_src.anchor(), m_src.length() };
}

/**
  reads identifier and classifies it
  */
Token Scanner::readIdentifier()
{
    static QRegExp methodPattern(METHOD_PATTERN "$");
    //                                              METHOD  (          &        parameter,         &
    static QRegExp parameterPattern(METHOD_PATTERN "8_(2_)?(3_)?((2_)?(3_)?(2_)?9_(2_)?(17_)?(2_)?(3_)?(2_)?)*$");
    static QRegExp contextPattern(CLASS_MODULE_PATTERN "$");


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
        if (m_hasContextRecognition && contextPattern.indexIn(m_tokenSequence) != -1)
            m_context << value.toString();
    // TODO: Use gperf for this keywords hash
    } else if (value == QLatin1String("end")) {
        kind = Token::KeywordEnd;
    } else if (value == QLatin1String("self")) {
        kind = Token::KeywordSelf;
    } else if (value == QLatin1String("def")) {
        kind = Token::KeywordDef;
    } else if (value == QLatin1String("module")) {
        kind = Token::KeywordModule;
    } else if (value == QLatin1String("class")) {
        kind = Token::KeywordClass;
    } else if (value == QLatin1String("if") || value == QLatin1String("unless")) {
        kind = Token::KeywordFlowControl;
    } else if (value == QLatin1String("while")
               || value == QLatin1String("until")
               ) {
        kind = Token::KeywordLoop;
    } else if (value == QLatin1String("do")
               || value == QLatin1String("begin")
               || value == QLatin1String("case")) {
        kind = Token::KeywordBlockStarter;
    } else if (std::find(&RUBY_KEYWORDS[0], &RUBY_KEYWORDS[N_KEYWORDS], value) != &RUBY_KEYWORDS[N_KEYWORDS]) {
        kind = Token::Keyword;
    } else if (methodPattern.indexIn(m_tokenSequence) != -1) {
        kind = Token::Method;
    } else if (parameterPattern.indexIn(m_tokenSequence) != -1) {
        kind = Token::Parameter;
    }

    return { kind, m_src.anchor(), m_src.length() };
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
    return { Token::Number, m_src.anchor(), m_src.length() };
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

    return { Token::Number, m_src.anchor(), m_src.length() };
}

/**
  reads single-line python comment, started with "#"
  */
Token Scanner::readComment()
{
    QChar ch = m_src.peek();
    while (ch != QLatin1Char('\n') && !ch.isNull()) {
        m_src.move();
        ch = m_src.peek();
    }
    return { Token::Comment, m_src.anchor(), m_src.length() };
}

/**
  reads whitespace
  */
Token Scanner::readWhiteSpace()
{
    while (m_src.peek().isSpace())
        m_src.move();
    return { Token::Whitespace, m_src.anchor(), m_src.length() };
}

/**
  reads punctuation symbols, excluding some special
  */
Token Scanner::readOperator(const QChar& first)
{
    static const QString singleCharOperators = QStringLiteral("[]{}()");
    if (singleCharOperators.contains(first))
        return { Token::Operator, m_src.anchor(), m_src.length() };

    static const QString operators = QStringLiteral("<=>+-/*%!");
    static const QString colon = QStringLiteral(":");
    const QString& acceptedChars = first == QLatin1Char(':') ? colon : operators;
    QChar ch = m_src.peek();

    while (acceptedChars.contains(ch)) {
        m_src.move();
        ch = m_src.peek();
    }
    return { Token::Operator, m_src.anchor(), m_src.length() };
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
    state = static_cast<State>(m_state >> 16);
    savedData = static_cast<ushort>(m_state);
}

}
