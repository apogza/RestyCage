// xmlsyntaxhighlighter.h
#ifndef XMLSYNTAXHIGHLIGHTER_H
#define XMLSYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QTextCharFormat>
#include <QHash>

class XMLHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit XMLHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    QVector<HighlightingRule> highlightingRules;

    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat xmlTagFormat;
    QTextCharFormat attributeNameFormat;
    QTextCharFormat attributeValueFormat;
    QTextCharFormat commentFormat;
    QTextCharFormat stringFormat;
};

#endif // XMLSYNTAXHIGHLIGHTER_H
