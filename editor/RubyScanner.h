#ifndef RubyScanner_h
#define RubyScanner_h

#include "SourceCodeStream.h"

#include <QString>
#include <QStringList>
#include <QSet>

namespace Ruby {

class Token
{
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
        KeywordElseElsIfRescueEnsure = 27,
        OpenBraces                   = 28,
        CloseBraces                  = 29,

        Backtick,
        InStringCode,
        KeywordVisibility,
        EndOfBlock
    };

    Token(Kind _kind = EndOfBlock, int _position = 0, int _length = 0) :
        kind(_kind),
        position(_position),
        length(_length)
    {}

    Kind kind;
    int position;
    int length;
};

class Scanner
{
    Q_DISABLE_COPY(Scanner)

public:
    enum State {
        State_Default,
        State_String,
        State_MultiLineString
    };

    Scanner(const QString *text);
    void enableContextRecognition();

    void setState(int state);
    int state() const;
    Token read();
    void readLine();

    static Token tokenAt(const QString* line, int position);

    QString contextName() const;
    int currentLine() const { return m_line; }
    int currentColumn(const Token &token) const { return token.position - m_lineStartOffset; }

    int indentVariation() const;
    // current line has a else, elsif, rescue or ensure.
    bool didBlockInterrupt();
private:
    Token onDefaultState();

    Token readStringLiteral(QChar quoteChar, bool stateRestored = false);
    Token readRegexp();
    Token readIdentifier();
    Token readNumber();
    Token readFloatNumber();
    Token readComment();
    Token readDoxygenComment();
    Token readWhiteSpace();
    Token readOperator(QChar first);
    Token readPercentageNotation();

    void clearState();
    void saveState(State state, QChar savedData);
    void parseState(State &state, QChar &savedData) const;

    SourceCodeStream m_src;
    int m_state;
    bool m_hasContextRecognition;

    QString m_tokenSequence;

    QStringList m_context;
    int m_line;
    int m_lineStartOffset;

    QList<int> m_contextDepths;
    int m_indentDepth;
};

}

#endif
