#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QTextCharFormat>
#include "json_highlighter.h"


JsonHighlighter::JsonHighlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent)
{
    keyFormat.setForeground(QColor(220,0 ,0));
    stringFormat.setForeground(QColor(50, 0, 200));
    numberFormat.setForeground(QColor(50, 0, 200));
    keywordFormat.setForeground(QColor(50, 0, 200));
    braceFormat.setForeground(QColor(220,0 ,0));
}

void JsonHighlighter::highlightBlock(const QString& text)
{
    //
    // Keys: "name":
    //
    static const QRegularExpression keyRegex(
        R"("([^"\\]|\\.)*"(?=\s*:))");

    auto keyMatches = keyRegex.globalMatch(text);
    while (keyMatches.hasNext())
    {
        auto match = keyMatches.next();
        setFormat(match.capturedStart(),
                  match.capturedLength(),
                  keyFormat);
    }

    //
    // Strings (excluding keys already colored)
    //
    static const QRegularExpression stringRegex(
        R"("([^"\\]|\\.)*")");

    auto stringMatches = stringRegex.globalMatch(text);
    while (stringMatches.hasNext())
    {
        auto match = stringMatches.next();
        setFormat(match.capturedStart(),
                  match.capturedLength(),
                  stringFormat);
    }

    //
    // Numbers
    //
    static const QRegularExpression numberRegex(
        R"(\b-?(0|[1-9]\d*)(\.\d+)?([eE][+-]?\d+)?\b)");

    auto numberMatches = numberRegex.globalMatch(text);
    while (numberMatches.hasNext())
    {
        auto match = numberMatches.next();
        setFormat(match.capturedStart(),
                  match.capturedLength(),
                  numberFormat);
    }

    //
    // true, false, null
    //
    static const QRegularExpression keywordRegex(
        R"(\b(true|false|null)\b)");

    auto keywordMatches = keywordRegex.globalMatch(text);
    while (keywordMatches.hasNext())
    {
        auto match = keywordMatches.next();
        setFormat(match.capturedStart(),
                  match.capturedLength(),
                  keywordFormat);
    }

    //
    // Braces and brackets
    //
    static const QRegularExpression braceRegex(
        R"([{}\[\]])");

    auto braceMatches = braceRegex.globalMatch(text);
    while (braceMatches.hasNext())
    {
        auto match = braceMatches.next();
        setFormat(match.capturedStart(),
                  match.capturedLength(),
                  braceFormat);
    }

    //
    // Re-apply key format so keys win over string format.
    //
    keyMatches = keyRegex.globalMatch(text);
    while (keyMatches.hasNext())
    {
        auto match = keyMatches.next();
        setFormat(match.capturedStart(),
                  match.capturedLength(),
                  keyFormat);
    }
}