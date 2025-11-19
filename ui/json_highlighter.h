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
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    QList<HighlightingRule> highlightingRules;

    QTextCharFormat jsonValueFormat;
    QTextCharFormat jsonVarFormat;
};

#endif // JSON_HIGHLIGHTER_H
