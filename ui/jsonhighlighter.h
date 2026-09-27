#ifndef JSONHIGHLIGHTER_H
#define JSONHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>

class JsonHighlighter : public QSyntaxHighlighter
{

    Q_OBJECT

public:
    explicit JsonHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    // One big pattern that matches:
    //  - strings   `"foo"`
    //  - numbers   `123` or `45.6`
    //  - booleans  `true|false`
    //  - null      `null`
    //  - braces/brackets

    QRegularExpression tokenPattern;
};

#endif // JSONHIGHLIGHTER_H
