#include "htmlhighlighter.h"
#include <QTextDocument>

HtmlHighlighter::HtmlHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent) {
    setupRules();
}

void HtmlHighlighter::setupRules() {
    // Keywords (HTML tags)
    keywordFormat.setForeground(Qt::darkBlue);

    // Precompiled regular expressions for tags
    highlightingRules.append({QRegularExpression(R"(<\/?\w+)", QRegularExpression::CaseInsensitiveOption), keywordFormat});

    // Attribute names
    attributeFormat.setForeground(Qt::darkGreen);
    highlightingRules.append({QRegularExpression(R"(\w+(?=\s*=))", QRegularExpression::CaseInsensitiveOption), attributeFormat});

    // Attribute values
    stringFormat.setForeground(Qt::darkRed);
    highlightingRules.append({QRegularExpression(R"(=("[^"]*"|'[^']*'))", QRegularExpression::CaseInsensitiveOption), stringFormat});

    // Comments
    commentFormat.setForeground(Qt::gray);
    commentStartExpression = QRegularExpression(R"(<!--)");
    commentEndExpression = QRegularExpression(R"(-->)");

    // Strings (quoted attributes)
    stringFormat.setForeground(Qt::darkRed);
    highlightingRules.append({QRegularExpression(R"(["'].*?["'])"), stringFormat});

    // Tag names
    tagFormat.setForeground(Qt::darkBlue);
    highlightingRules.append({QRegularExpression(R"(<\/?(\w+))", QRegularExpression::CaseInsensitiveOption), tagFormat});
}

void HtmlHighlighter::highlightBlock(const QString &text) {
    // Highlight all rules
    for (const HighlightingRule &rule : qAsConst(highlightingRules)) {
        QRegularExpressionMatchIterator iterator = rule.pattern.globalMatch(text);
        while (iterator.hasNext()) {
            QRegularExpressionMatch match = iterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    // Handle comments
    QRegularExpressionMatch commentStartMatch = commentStartExpression.match(text);
    if (commentStartMatch.hasMatch()) {
        int commentStart = commentStartMatch.capturedStart();
        QRegularExpressionMatch commentEndMatch = commentEndExpression.match(text, commentStart);
        if (commentEndMatch.hasMatch()) {
            int commentEnd = commentEndMatch.capturedEnd();
            setFormat(commentStart, commentEnd - commentStart, commentFormat);
        } else {
            setFormat(commentStart, text.length() - commentStart, commentFormat);
        }
    }
}
