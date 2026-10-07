// xmlsyntaxhighlighter.cpp
#include "xmlhighlighter.h"

XMLHighlighter::XMLHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    // Define keyword format (tags)
    keywordFormat.setForeground(Qt::darkBlue);

    // Define XML tag format
    xmlTagFormat.setForeground(Qt::darkGreen);

    // Define attribute name format
    attributeNameFormat.setForeground(Qt::darkMagenta);

    // Define attribute value format
    attributeValueFormat.setForeground(Qt::darkRed);
    attributeValueFormat.setFontItalic(true);

    // Define comment format
    commentFormat.setForeground(Qt::gray);
    commentFormat.setFontItalic(true);

    // Define string format
    stringFormat.setForeground(Qt::darkRed);

    // Highlighting rules for XML elements
    HighlightingRule rule;

    // XML tags (both opening and closing)
    rule.pattern = QRegularExpression(QStringLiteral("<(/?\\w+)"));
    rule.format = xmlTagFormat;
    highlightingRules.append(rule);

    // XML tags (closing tags)
    rule.pattern = QRegularExpression(QStringLiteral("</\\w+>"));
    rule.format = xmlTagFormat;
    highlightingRules.append(rule);

    // Attribute names
    rule.pattern = QRegularExpression(QStringLiteral("(\\w+)\\s*="));
    rule.format = attributeNameFormat;
    highlightingRules.append(rule);

    // Attribute values (quoted strings)
    rule.pattern = QRegularExpression(QStringLiteral("=\\s*\"([^\"]*)\""));
    rule.format = attributeValueFormat;
    highlightingRules.append(rule);

    // Attribute values (single quoted strings)
    rule.pattern = QRegularExpression(QStringLiteral("=\\s*'([^']*)'"));
    rule.format = attributeValueFormat;
    highlightingRules.append(rule);

    // XML declarations
    rule.pattern = QRegularExpression(QStringLiteral("<\\?xml[^>]*>"));
    rule.format = keywordFormat;
    highlightingRules.append(rule);

    // Processing instructions
    rule.pattern = QRegularExpression(QStringLiteral("<\\?[^>]*\\?>"));
    rule.format = keywordFormat;
    highlightingRules.append(rule);

    // DOCTYPE declarations
    rule.pattern = QRegularExpression(QStringLiteral("<!DOCTYPE[^>]*>"));
    rule.format = keywordFormat;
    highlightingRules.append(rule);

    // Comments
    commentStartExpression = QRegularExpression(QStringLiteral("<!--"));
    commentEndExpression = QRegularExpression(QStringLiteral("-->"));
}

void XMLHighlighter::highlightBlock(const QString &text)
{
    // Apply highlighting rules
    for (const HighlightingRule &rule : std::as_const(highlightingRules)) {
        QRegularExpressionMatchIterator iterator = rule.pattern.globalMatch(text);
        while (iterator.hasNext()) {
            QRegularExpressionMatch match = iterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    // Handle multi-line comments
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);

    while (startIndex >= 0) {
        QRegularExpressionMatch match = commentStartExpression.match(text, startIndex);
        int endIndex = text.indexOf(commentEndExpression, startIndex);
        int commentLength;

        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex + match.capturedLength();
        }

        setFormat(startIndex, commentLength, commentFormat);
        startIndex = text.indexOf(commentEndExpression, startIndex + commentLength);
    }

    // Highlight strings (quoted attributes)
    QRegularExpression stringExpression(QStringLiteral("(=\\s*[\"'])[^\"']*([\"'])"));
    QRegularExpressionMatchIterator stringIterator = stringExpression.globalMatch(text);
    while (stringIterator.hasNext()) {
        QRegularExpressionMatch match = stringIterator.next();
        // Highlight only the quoted part, excluding the equals sign and quotes
        int valueStart = match.capturedStart(1) + 1; // Start after ="
        int valueLength = match.capturedLength(1) - 2; // Length without quotes

        if (valueLength > 0) {
            setFormat(valueStart, valueLength, attributeValueFormat);
        }
    }
}
