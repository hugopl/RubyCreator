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

#ifndef SourceCodeStream_h
#define SourceCodeStream_h

#include <QString>

namespace Ruby {

class SourceCodeStream
{
public:
    SourceCodeStream(const QString* text)
        : m_text(text)
        , m_textPtr(m_text->data())
        , m_textLength(text->length())
        , m_position(0)
        , m_markedPosition(0)
    {}

    inline void setAnchor()
    {
        m_markedPosition = m_position;
    }

    inline void move()
    {
        ++m_position;
    }

    int position() const
    {
        return m_position;
    }

    inline int length() const
    {
        return m_position - m_markedPosition;
    }

    inline int anchor() const
    {
        return m_markedPosition;
    }

    inline bool isEnd() const
    {
        return m_position >= m_textLength;
    }

    inline QChar peek(int offset = 0) const
    {
        int pos = m_position + offset;
        if (pos >= m_textLength)
            return QChar();
        return m_textPtr[pos];
    }

    inline QStringRef value() const
    {
        return QStringRef(m_text, m_markedPosition, length());
    }

    inline QStringRef value(int begin, int length) const
    {
        return QStringRef(m_text, begin, length);
    }

private:
    const QString* m_text;
    const QChar* m_textPtr;
    const int m_textLength;
    int m_position;
    int m_markedPosition;
};

}

#endif
