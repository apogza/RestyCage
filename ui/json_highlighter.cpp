#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QTextCharFormat>
#include "json_highlighter.h"


JsonHighlighter::JsonHighlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent)
{
    keyFormat.setForeground(QColor(220,0 ,0));
    valueFormat.setForeground(QColor(50, 0, 200));
    braceFormat.setForeground(QColor(220,0 ,0));
}

void JsonHighlighter::highlightBlock(const QString& text)
{
    QTextBlock block = currentBlock();
    if (!block.isVisible())
    {
        return;
    }

    int textSize = text.length();

    int beginString = -1;
    int beginValue = -1;

    for (int i = 0; i < textSize; i++)
    {
        const QString &currentChar = text[i];

        if (currentChar == " ")
        {
            continue;
        }
        else if (currentChar == "{" || currentChar == "}"
            || currentChar == "[" || currentChar== "]")
        {
            if (currentChar == "{")
            {
                isInKey = true;
                isInArray = false;
            }

            if (currentChar == "[")
            {
                isInArray = true;
            }

            if (currentChar == "]")
            {
                isInArray = false;
            }

            setFormat(i, 1, braceFormat);
        }

        else if (currentChar == ":" && beginString == -1)
        {
            isInKey = false;
            setFormat(i, 1, braceFormat);
        }

        else if (currentChar == ",")
        {
            if (!isInArray)
            {
                isInKey = true;
            }

            setFormat(i, 1, valueFormat);

            if (beginValue != -1)
            {
                setFormat(beginValue, i - beginValue + 1, valueFormat);
                beginValue = -1;
            }
        }        

        else if (currentChar == "\"")
        {
            if (beginString == -1)
            {
                beginString = i;
            }
            else
            {
                setFormat(beginString, i - beginString + 1, isInKey ? keyFormat : valueFormat);
                beginString = -1;
            }
        }
        else
        {
            if (!isInKey && beginString == -1 && beginValue == -1)
            {
                beginValue = i;
            }

            if (!isInKey && i == textSize - 1)
            {
                if (beginValue != -1)
                {
                    setFormat(beginValue, i - beginValue + 1, valueFormat);
                    beginValue = -1;
                }
            }
        }
    }
}