#ifndef JSON_HIGHLIGHTER_H
#define JSON_HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QObject>
#include <QRegularExpression>

class JsonHighlighter : public QSyntaxHighlighter
{

    Q_OBJECT

public:
    JsonHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    QTextCharFormat keyFormat;
    QTextCharFormat stringFormat;
    QTextCharFormat numberFormat;
    QTextCharFormat keywordFormat;
    QTextCharFormat braceFormat;

};


#endif // JSON_HIGHLIGHTER_H
