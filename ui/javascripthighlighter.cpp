#include "javascripthighlighter.h"
#include <QTextDocument>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>

JavascriptHighlighter::JavascriptHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    setupHighlightingRules();
    setupCommentExpressions();
    setupPatternExpressions();
}

void JavascriptHighlighter::setupHighlightingRules()
{
    // Keywords
    keywordFormat.setForeground(Qt::darkBlue);

    QStringList keywordPatterns = {
        "\\babstract\\b", "\\barguments\\b", "\\bawait\\b", "\\bboolean\\b",
        "\\bbreak\\b", "\\bbyte\\b", "\\bcase\\b", "\\bcatch\\b",
        "\\bchar\\b", "\\bclass\\b", "\\bconst\\b", "\\bcontinue\\b",
        "\\bdebugger\\b", "\\bdefault\\b", "\\bdelete\\b", "\\bdo\\b",
        "\\bdouble\\b", "\\belse\\b", "\\benum\\b", "\\beval\\b",
        "\\bexport\\b", "\\bextends\\b", "\\bfalse\\b", "\\bfinal\\b",
        "\\bfinally\\b", "\\bfloat\\b", "\\bfor\\b", "\\bfunction\\b",
        "\\bgoto\\b", "\\bif\\b", "\\bimplements\\b", "\\bimport\\b",
        "\\binstanceof\\b", "\\bint\\b", "\\binterface\\b", "\\blet\\b",
        "\\blink\\b", "\\blong\\b", "\\bnative\\b", "\\bnew\\b",
        "\\bnull\\b", "\\bpackage\\b", "\\bprivate\\b", "\\bprotected\\b",
        "\\bpublic\\b", "\\breturn\\b", "\\bshort\\b", "\\bsizeof\\b",
        "\\bstatic\\b", "\\bstruct\\b", "\\bswitch\\b", "\\bsynchronized\\b",
        "\\bthis\\b", "\\bthrow\\b", "\\bthrows\\b", "\\btransient\\b",
        "\\btrue\\b", "\\btry\\b", "\\btypeof\\b", "\\bvar\\b",
        "\\bvoid\\b", "\\bvolatile\\b", "\\bwhile\\b", "\\bwith\\b",
        "\\byield\\b", "\\basync\\b", "\\bawait\\b"
    };

    for (const QString &pattern : keywordPatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    // Built-in functions and objects
    builtinFormat.setForeground(Qt::darkGreen);

    QStringList builtinPatterns = {
        "\\bArray\\b", "\\bDate\\b", "\\beval\\b", "\\bFunction\\b",
        "\\bMath\\b", "\\bNumber\\b", "\\bObject\\b", "\\bRegExp\\b",
        "\\bString\\b", "\\bdecodeURI\\b", "\\bdecodeURIComponent\\b",
        "\\bencodeURI\\b", "\\bencodeURIComponent\\b", "\\bError\\b",
        "\\bEvalError\\b", "\\bRangeError\\b", "\\bReferenceError\\b",
        "\\bSyntaxError\\b", "\\bTypeError\\b", "\\bURIError\\b",
        "\\bconsole\\b", "\\bdocument\\b", "\\bwindow\\b", "\\bnavigator\\b",
        "\\blocalStorage\\b", "\\bsessionStorage\\b"
    };

    for (const QString &pattern : builtinPatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = builtinFormat;
        highlightingRules.append(rule);
    }

    // Class names
    classFormat.setForeground(Qt::darkMagenta);

    QStringList classPatterns = {
        "\\bclass\\b\\s+([A-Z][a-zA-Z0-9_]*)"
    };

    for (const QString &pattern : classPatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = classFormat;
        highlightingRules.append(rule);
    }

    // Functions
    functionFormat.setForeground(Qt::darkCyan);
    functionFormat.setFontItalic(true);

    QStringList functionPatterns = {
        "\\bfunction\\b\\s+([a-zA-Z_$][a-zA-Z0-9_$]*)"
    };

    for (const QString &pattern : functionPatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = functionFormat;
        highlightingRules.append(rule);
    }

    // Single line comments
    singleLineCommentFormat.setForeground(Qt::gray);
    singleLineCommentFormat.setFontItalic(true);
    singleLineCommentExpression = QRegularExpression("//[^\n]*");

    // Multi-line comments
    multiLineCommentFormat.setForeground(Qt::gray);
    multiLineCommentFormat.setFontItalic(true);

    // String literals
    quotationFormat.setForeground(Qt::darkRed);


    // Numbers
    numberFormat.setForeground(Qt::darkYellow);

    // Operators
    operatorFormat.setForeground(Qt::darkRed);

    // Regular expressions (simple detection)
    regexFormat.setForeground(Qt::darkBlue);

    // Preprocessor directives (for C-like syntax)
    preprocessorFormat.setForeground(Qt::darkBlue);
}

void JavascriptHighlighter::setupCommentExpressions()
{
    commentStartExpression = QRegularExpression("/\\*");
    commentEndExpression = QRegularExpression("\\*/");
}

void JavascriptHighlighter::setupPatternExpressions()
{
    functionStartExpression = QRegularExpression("function\\s+\\w+");
    functionEndExpression = QRegularExpression("\\}");
    stringStartExpression = QRegularExpression("\"[^\"\\\\]*(?:\\\\.[^\"\\\\]*)*\"");
    stringEndExpression = QRegularExpression("'[^'\\\\]*(?:\\\\.[^'\\\\]*)*'");
    regexStartExpression = QRegularExpression("/[^/\\\\]*(?:\\\\.[^/\\\\]*)*/");
    regexEndExpression = QRegularExpression("/");
    numberExpression = QRegularExpression("\\b\\d+\\.?\\d*\\b");
    operatorExpression = QRegularExpression("[=+\\-*/<>!&|%^~]");
    keywordExpression = QRegularExpression("\\b(?:if|else|for|while|do|switch|case|default|break|continue|return|try|catch|finally|throw|with|var|let|const|function|class|extends|import|export|from|as|new|this|super|static|async|await|yield|debugger)\\b");
    builtinExpression = QRegularExpression("\\b(?:Array|Date|eval|Function|Math|Number|Object|RegExp|String|console|document|window|navigator|localStorage|sessionStorage)\\b");
    classExpression = QRegularExpression("\\bclass\\b");
    singleLineCommentExpression = QRegularExpression("//[^\n]*");
}

void JavascriptHighlighter::highlightBlock(const QString &text)
{
    // Highlight comments
    int commentStart = -1;
    int commentEnd = -1;

    // Handle multi-line comments
    commentStart = text.indexOf(commentStartExpression);
    if (commentStart != -1) {
        commentEnd = text.indexOf(commentEndExpression, commentStart);
        if (commentEnd != -1) {
            setFormat(commentStart, commentEnd - commentStart + 2, multiLineCommentFormat);
            // Continue processing remaining text
            int remainingStart = commentEnd + 2;
            if (remainingStart < text.length()) {
                highlightBlock(text.mid(remainingStart));
            }
            return;
        } else {
            // Multi-line comment not closed
            setFormat(commentStart, text.length() - commentStart, multiLineCommentFormat);
            return;
        }
    }

    // Handle single-line comments
    QRegularExpressionMatchIterator iterator = singleLineCommentExpression.globalMatch(text);
    while (iterator.hasNext()) {
        QRegularExpressionMatch match = iterator.next();
        setFormat(match.capturedStart(), match.capturedLength(), singleLineCommentFormat);
    }

    // Highlight strings
    QRegularExpression stringPattern("\"[^\"\\\\]*(?:\\\\.[^\"\\\\]*)*\"");
    QRegularExpressionMatchIterator stringIterator = stringPattern.globalMatch(text);
    while (stringIterator.hasNext()) {
        QRegularExpressionMatch match = stringIterator.next();
        setFormat(match.capturedStart(), match.capturedLength(), quotationFormat);
    }

    // Highlight single quote strings
    QRegularExpression singleQuotePattern("'[^'\\\\]*(?:\\\\.[^'\\\\]*)*'");
    QRegularExpressionMatchIterator singleQuoteIterator = singleQuotePattern.globalMatch(text);
    while (singleQuoteIterator.hasNext()) {
        QRegularExpressionMatch match = singleQuoteIterator.next();
        setFormat(match.capturedStart(), match.capturedLength(), quotationFormat);
    }

    // Highlight regular expressions (simplified - only basic cases)
    QRegularExpression regexPattern("/[^/\\\\]*(?:\\\\.[^/\\\\]*)*/[gimy]*");
    QRegularExpressionMatchIterator regexIterator = regexPattern.globalMatch(text);
    while (regexIterator.hasNext()) {
        QRegularExpressionMatch match = regexIterator.next();
        setFormat(match.capturedStart(), match.capturedLength(), regexFormat);
    }

    // Highlight numbers
    QRegularExpressionMatchIterator numberIterator = numberExpression.globalMatch(text);
    while (numberIterator.hasNext()) {
        QRegularExpressionMatch match = numberIterator.next();
        setFormat(match.capturedStart(), match.capturedLength(), numberFormat);
    }

    // Highlight operators
    QRegularExpressionMatchIterator operatorIterator = operatorExpression.globalMatch(text);
    while (operatorIterator.hasNext()) {
        QRegularExpressionMatch match = operatorIterator.next();
        setFormat(match.capturedStart(), match.capturedLength(), operatorFormat);
    }

    // Highlight keywords and built-ins
    for (const HighlightingRule &rule : highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    // Handle function declarations
    QRegularExpression funcPattern("\\bfunction\\s+([a-zA-Z_$][a-zA-Z0-9_$]*)");
    QRegularExpressionMatchIterator funcIterator = funcPattern.globalMatch(text);
    while (funcIterator.hasNext()) {
        QRegularExpressionMatch match = funcIterator.next();
        if (match.hasMatch()) {
            setFormat(match.capturedStart(1), match.capturedLength(1), functionFormat);
        }
    }
}
