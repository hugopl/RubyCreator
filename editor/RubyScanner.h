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
#include <QStringList>
#include <QSet>

namespace Ruby {

class Token {
public:
    enum Kind
    {
        // Don't change these numbers until I find a better way to keep track of then in
        // the regexes used against m_tokenSequence
        Number        = 0,
        String        = 1,
        Whitespace    = 2,
        Operator      = 3,
        Comment       = 4,
        Identifier    = 5,
        Regexp        = 6,
        Symbol        = 7,
        Method        = 8,
        Parameter     = 9,
        ClassField    = 11,
        Constant      = 12,
        Global        = 13,
        Keyword       = 14,
        KeywordDef    = 15,
        KeywordSelf   = 16,
        OperatorComma = 17,
        OperatorDot   = 18,
        KeywordClass  = 19,
        KeywordModule = 20,
        KeywordFlowControl  = 21,
        KeywordLoop         = 22,
        KeywordBlockStarter = 23,
        KeywordEnd          = 24,
        OperatorAssign      = 25,
        OperatorSemiColon   = 26,

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

    Scanner(const QString* text);
    void enableContextRecognition();

    void setState(int state);
    int state() const;
    Token read();

    QString contextName() const;
    int currentLine() const { return m_line; }
    int indentLevel() const;
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
    Token readOperator(const QChar& first);

    void clearState();
    void saveState(State state, QChar savedData);
    void parseState(State& state, QChar& savedData) const;

    SourceCodeStream m_src;
    int m_state;
    bool m_hasContextRecognition;

    QString m_tokenSequence;

    QStringList m_context;
    int m_line;

    Scanner(const Scanner&) = delete;
};

}

#endif
