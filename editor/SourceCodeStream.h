#ifndef SourceCodeStream_h
#define SourceCodeStream_h

#include <QString>

namespace Ruby {

class SourceCodeStream
{
public:
    SourceCodeStream(const QString *text)
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
    const QString *m_text;
    const QChar *m_textPtr;
    const int m_textLength;
    int m_position;
    int m_markedPosition;
};

}

#endif
