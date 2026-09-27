#include "jsonhighlighter.h"

#include <QRegularExpression>
#include <QRegularExpressionMatch>


JsonHighlighter::JsonHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    tokenPattern = QRegularExpression({R"((\"[^"]*\":)|(\"[^"]*\")|(\b\d+(\.\d+)?\b)|\b(true|false|null)\b|([{}[\],:]))"});
}

void JsonHighlighter::highlightBlock(const QString &text)
{
    QRegularExpressionMatchIterator it = tokenPattern.globalMatch(text);

    while (it.hasNext()) {
        const QRegularExpressionMatch match = it.next();
        int start   = match.capturedStart();      // where the token begins
        int length  = match.capturedLength();     // how many chars

        QTextCharFormat fmt;                      // colour of the token

        if (match.hasCaptured(1)) {         // key
            fmt.setForeground(Qt::red);
        } else if (match.hasCaptured(2)) { // value
            fmt.setForeground(Qt::blue);
        } else if (match.hasCaptured(2)) {  // number
            fmt.setForeground(Qt::darkGreen);
        } else if (match.hasCaptured(3)) {  // boolean
            fmt.setForeground(Qt::magenta);
        } else if (match.hasCaptured(4)) {  // null
            fmt.setForeground(Qt::gray);
        } else if (match.captured().length() == 1) {   // braces/brackets
            fmt.setForeground(Qt::darkBlue);
        }

        setFormat(start, length, fmt);             // paint the token!
    }
}
