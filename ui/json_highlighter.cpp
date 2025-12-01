#include "json_highlighter.h"

JsonHighlighter::JsonHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{

    HighlightingRule jsonValueRule;

    jsonValueFormat.setForeground(QBrush(QColor(qRgb(60, 121, 107))));
    jsonValueRule.pattern = QRegularExpression(QStringLiteral(":\\s*\".*\""));
    jsonValueRule.format = jsonValueFormat;
    highlightingRules.append(jsonValueRule);

    HighlightingRule jsonVarRule;

    jsonVarFormat.setForeground(QBrush(QColor(qRgb(79, 85, 118))));
    jsonVarRule.pattern = QRegularExpression(QStringLiteral("\".*\"\\s*:"));
    jsonVarRule.format = jsonVarFormat;
    highlightingRules.append(jsonVarRule);
}

void JsonHighlighter::highlightBlock(const QString &text)
{
    for (const HighlightingRule &rule : std::as_const(highlightingRules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}
