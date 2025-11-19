#include "json_highlighter.h"

JsonHighlighter::JsonHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{

    HighlightingRule jsonValueRule;

    jsonValueFormat.setForeground(Qt::darkCyan);
    jsonValueRule.pattern = QRegularExpression(QStringLiteral(":\\s*\".*\""));
    jsonValueRule.format = jsonValueFormat;
    highlightingRules.append(jsonValueRule);

    HighlightingRule jsonVarRule;

    jsonVarFormat.setForeground(Qt::blue);
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
