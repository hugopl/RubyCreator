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

#ifndef RubyScanner_h
#define RubyScanner_h

#include "SourceCodeStream.h"

#include <QString>
#include <QSet>

namespace Ruby {

class Token {
public:
    enum Kind
    {
        Number = 0,
        String,
        Keyword,
        Type,
        ClassField,
        Operator,
        Comment,
        Doxygen,
        Identifier,
        Whitespace,
        Constant,
        Global,
        Regexp,
        Symbol,

        EndOfBlock
    };

    Kind kind;
    int position;
    int length;
};

class Scanner
{
public:
    enum State {
        State_Default,
        State_String,
        State_MultiLineString
    };

    Scanner(const QChar* text, const int length);

    void setState(int state);
    int state() const;
    Token read();
    QString value(const Token& tk) const;

private:
    Token onDefaultState();

    bool checkEscapeSequence();
    Token readStringLiteral(QChar quoteChar);
    Token readRegexp();
    Token readMultiLineStringLiteral(QChar quoteChar);
    Token readIdentifier();
    Token readNumber();
    Token readFloatNumber();
    Token readComment();
    Token readDoxygenComment();
    Token readWhiteSpace();
    Token readOperator();

    void clearState();
    void saveState(State state, QChar savedData);
    void parseState(State& state, QChar& savedData) const;

    SourceCodeStream m_src;
    int m_state;
    static QSet<QString> m_keywords;

    Scanner(const Scanner&) = delete;
};

}

#endif
