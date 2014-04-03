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

#include <QString>
#include <QSet>

namespace RubyEditor {

static const char* const RUBY_KEYWORDS[] = {
    "BEGIN",
    "END",
    "__ENCODING__",
    "__END__",
    "__FILE__",
    "__LINE__",
    "alias",
    "and",
    "begin",
    "break",
    "case",
    "class",
    "def",
    "defined?",
    "do",
    "else",
    "elsif",
    "end",
    "ensure",
    "false",
    "for",
    "if",
    "in",
    "module",
    "next",
    "nil",
    "not",
    "or",
    "redo",
    "rescue",
    "retry",
    "return",
    "self",
    "super",
    "then",
    "true",
    "undef",
    "unless",
    "until",
    "when",
    "while",
    "yield"
};

/// Copies identifiers from array to QSet
static void copyIdentifiers(const char * const words[], size_t bytesCount, QSet<QString> &result)
{
    const size_t count = bytesCount / sizeof(const char * const);
    for (size_t i = 0; i < count; ++i)
        result.insert(QLatin1String(words[i]));
}

QSet<QString> RubyScanner::m_keywords;
QSet<QString> RubyScanner::m_magics;
QSet<QString> RubyScanner::m_builtins;

RubyScanner::RubyScanner(const QChar* text, const int length)
    : m_src(text, length)
    , m_state(0)
{
    if (m_keywords.empty())
        copyIdentifiers(RUBY_KEYWORDS, sizeof(RUBY_KEYWORDS), m_keywords);
}

void RubyScanner::setState(int state)
{
    m_state = state;
}

int RubyScanner::state() const
{
    return m_state;
}

RubyToken RubyScanner::read()
{
    m_src.setAnchor();
    if (m_src.isEnd())
        return { RubyToken::EndOfBlock, m_src.anchor(), 0 };

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

QString RubyScanner::value(const RubyToken& tk) const
{
    return m_src.value(tk.position, tk.length);
}

RubyToken RubyScanner::onDefaultState()
{
    QChar first = m_src.peek();
    m_src.move();

    if (first == QLatin1Char('\\') && m_src.peek() == QLatin1Char('\n')) {
        m_src.move();
        return { RubyToken::Whitespace, m_src.anchor(), 2 };
    }

    if (first == QLatin1Char('.') && m_src.peek().isDigit())
        return readFloatNumber();

    if (first == QLatin1Char('\'') || first == QLatin1Char('\"'))
        return readStringLiteral(first);

    if (first.isLetter() || (first == QLatin1Char('_')))
        return readIdentifier();

    if (first.isDigit())
        return readNumber();

    if (first == QLatin1Char('#')) {
        if (m_src.peek() == QLatin1Char('#'))
            return readDoxygenComment();
        return readComment();
    }

    if (first.isSpace())
        return readWhiteSpace();

    return readOperator();
}

/**
 * @brief Lexer::passEscapeCharacter
 * @return returns true if escape sequence doesn't end with newline
 */
void RubyScanner::checkEscapeSequence(QChar quoteChar)
{
    if (m_src.peek() == QLatin1Char('\\')) {
        m_src.move();
        QChar ch = m_src.peek();
        if (ch == QLatin1Char('\n') || ch.isNull())
            saveState(State_String, quoteChar);
    }
}

/**
  reads single-line string literal, surrounded by ' or " quotes
  */
RubyToken RubyScanner::readStringLiteral(QChar quoteChar)
{
    QChar ch = m_src.peek();
    if (ch == quoteChar && m_src.peek(1) == quoteChar) {
        saveState(State_MultiLineString, quoteChar);
        return readMultiLineStringLiteral(quoteChar);
    }

    while (ch != quoteChar && !ch.isNull()) {
        checkEscapeSequence(quoteChar);
        m_src.move();
        ch = m_src.peek();
    }
    if (ch == quoteChar)
        clearState();
    m_src.move();
    return { RubyToken::String, m_src.anchor(), m_src.length() };
}

/**
  reads multi-line string literal, surrounded by ''' or """ sequencies
  */
RubyToken RubyScanner::readMultiLineStringLiteral(QChar quoteChar)
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

    return { RubyToken::String, m_src.anchor(), m_src.length() };
}

/**
  reads identifier and classifies it
  */
RubyToken RubyScanner::readIdentifier()
{
    QChar ch = m_src.peek();
    while (ch.isLetterOrNumber() || (ch == QLatin1Char('_'))) {
        m_src.move();
        ch = m_src.peek();
    }
    QString value = m_src.value();

    RubyToken::Kind kind = RubyToken::Identifier;
    if (value == QLatin1String("self"))
        kind = RubyToken::ClassField;
    else if (m_builtins.contains(value))
        kind = RubyToken::Type;
    else if (m_magics.contains(value))
        kind = RubyToken::MagicAttr;
    else if (m_keywords.contains(value))
        kind = RubyToken::Keyword;

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

RubyToken RubyScanner::readNumber()
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
    return { RubyToken::Number, m_src.anchor(), m_src.length() };
}

RubyToken RubyScanner::readFloatNumber()
{
    enum
    {
        State_INTEGER,
        State_FRACTION,
        State_EXPONENT
    } state;
    state = (m_src.peek(-1) == QLatin1Char('.')) ? State_FRACTION : State_INTEGER;

    for (;;) {
        QChar ch = m_src.peek();
        if (ch.isNull())
            break;

        if (state == State_INTEGER) {
            if (ch == QLatin1Char('.'))
                state = State_FRACTION;
            else if (!ch.isDigit())
                break;
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

    QChar ch = m_src.peek();
    if ((state == State_INTEGER && (ch == QLatin1Char('l') || ch == QLatin1Char('L')))
            || (ch == QLatin1Char('j') || ch == QLatin1Char('J')))
        m_src.move();

    return { RubyToken::Number, m_src.anchor(), m_src.length() };
}

/**
  reads single-line python comment, started with "#"
  */
RubyToken RubyScanner::readComment()
{
    QChar ch = m_src.peek();
    while (ch != QLatin1Char('\n') && !ch.isNull()) {
        m_src.move();
        ch = m_src.peek();
    }
    return { RubyToken::Comment, m_src.anchor(), m_src.length() };
}

/**
  reads single-line python doxygen comment, started with "##"
  */
RubyToken RubyScanner::readDoxygenComment()
{
    QChar ch = m_src.peek();
    while (ch != QLatin1Char('\n') && !ch.isNull()) {
        m_src.move();
        ch = m_src.peek();
    }
    return { RubyToken::Doxygen, m_src.anchor(), m_src.length() };
}

/**
  reads whitespace
  */
RubyToken RubyScanner::readWhiteSpace()
{
    while (m_src.peek().isSpace())
        m_src.move();
    return { RubyToken::Whitespace, m_src.anchor(), m_src.length() };
}

/**
  reads punctuation symbols, excluding some special
  */
RubyToken RubyScanner::readOperator()
{
    const QString EXCLUDED_CHARS = QLatin1String("\'\"_#");
    QChar ch = m_src.peek();
    while (ch.isPunct() && !EXCLUDED_CHARS.contains(ch)) {
        m_src.move();
        ch = m_src.peek();
    }
    return { RubyToken::Operator, m_src.anchor(), m_src.length() };
}

void RubyScanner::clearState()
{
    m_state = 0;
}

void RubyScanner::saveState(State state, QChar savedData)
{
    m_state = (state << 16) | static_cast<int>(savedData.unicode());
}

void RubyScanner::parseState(State &state, QChar &savedData) const
{
    state = static_cast<State>(m_state >> 16);
    savedData = static_cast<ushort>(m_state);
}

}
