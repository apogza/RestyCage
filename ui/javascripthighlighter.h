#ifndef JAVASCRIPTHIGHLIGHTER_H
#define JAVASCRIPTHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QTextCharFormat>

class JavascriptHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit JavascriptHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    QVector<HighlightingRule> highlightingRules;

    QTextCharFormat keywordFormat;
    QTextCharFormat builtinFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
    QTextCharFormat numberFormat;
    QTextCharFormat operatorFormat;
    QTextCharFormat stringFormat;
    QTextCharFormat regexFormat;
    QTextCharFormat commentFormat;
    QTextCharFormat preprocessorFormat;

    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;
    QRegularExpression functionStartExpression;
    QRegularExpression functionEndExpression;
    QRegularExpression stringStartExpression;
    QRegularExpression stringEndExpression;
    QRegularExpression regexStartExpression;
    QRegularExpression regexEndExpression;
    QRegularExpression numberExpression;
    QRegularExpression operatorExpression;
    QRegularExpression keywordExpression;
    QRegularExpression builtinExpression;
    QRegularExpression classExpression;
    QRegularExpression singleLineCommentExpression;

    void setupHighlightingRules();
    void setupCommentExpressions();
    void setupPatternExpressions();
    bool isCommentBlock(const QString &text, int position);
    bool isStringBlock(const QString &text, int position);
    bool isRegexBlock(const QString &text, int position);
};

#endif // JAVASCRIPTHIGHLIGHTER_H
