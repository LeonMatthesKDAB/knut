#include "string_utils.h"

#include <QSet>

namespace Core {

static QString nextWordInString(const QString &str, QString::const_iterator &i, Case c)
{
    QString word;

    auto isSeparator = [&i, c]() {
        if (c == Case::CamelCase || c == Case::PascalCase)
            return i->isUpper();
        else if (c == Case::SnakeCase || c == Case::UpperCase)
            return *i == '_';
        else if (c == Case::KebabCase)
            return *i == '-';
        return *i == ' ';
    };

    do {
        word += *i;
        i++;
    } while (!isSeparator() && i != str.end());

    if (i != str.end()
        && (c == Case::SnakeCase || c == Case::KebabCase || c == Case::UpperCase || c == Case::TitleCase))
        i++;

    return word;
}

QString convertCase(const QString &str, Case from, Case to)
{
    if (from == to)
        return str;

    static const QSet<QString> titleCaseExceptions = {"a",  "an", "the", "at", "by",  "by", "for",
                                                      "in", "of", "on",  "to", "and", "as", "or"};

    QString result;

    auto i = str.cbegin();
    while (i < str.cend()) {
        QString w = nextWordInString(str, i, from);
        const bool firstWord = result.isEmpty();

        switch (to) {
        case Case::CamelCase:
            w = w.toLower();
            if (!firstWord)
                w[0] = w.front().toUpper();
            result += w;
            break;
        case Case::PascalCase:
            w = w.toLower();
            w[0] = w.front().toUpper();
            result += w;
            break;
        case Case::SnakeCase:
            if (!firstWord)
                result += '_';
            result += w.toLower();
            break;
        case Case::KebabCase:
            if (!firstWord)
                result += '-';
            result += w.toLower();
            break;
        case Case::UpperCase:
            if (!firstWord)
                result += '_';
            result += w.toUpper();
            break;
        case Case::TitleCase:
            if (!firstWord)
                result += ' ';
            w = w.toLower();
            if (firstWord || !titleCaseExceptions.contains(w))
                w[0] = w.front().toUpper();
            result += w;
        }
    }

    return result;
}

namespace Internal {
    // This function is copied from from Qt Creator.
    QString matchCaseReplacement(const QString &originalText, const QString &replaceText)
    {
        if (originalText.isEmpty() || replaceText.isEmpty())
            return replaceText;

        // Now proceed with actual case matching
        bool firstIsUpperCase = originalText.at(0).isUpper();
        bool firstIsLowerCase = originalText.at(0).isLower();
        bool restIsLowerCase = true; // to be verified
        bool restIsUpperCase = true; // to be verified

        for (int i = 1; i < originalText.length(); ++i) {
            if (originalText.at(i).isUpper())
                restIsLowerCase = false;
            else if (originalText.at(i).isLower())
                restIsUpperCase = false;

            if (!restIsLowerCase && !restIsUpperCase)
                break;
        }

        if (restIsLowerCase) {
            QString res = replaceText.toLower();
            if (firstIsUpperCase)
                res.replace(0, 1, res.at(0).toUpper());
            return res;
        } else if (restIsUpperCase) {
            QString res = replaceText.toUpper();
            if (firstIsLowerCase)
                res.replace(0, 1, res.at(0).toLower());
            return res;
        } else {
            return replaceText; // mixed
        }
    }
}

// This function is copied from from Qt Creator.
QString matchCaseReplacement(const QString &originalText, const QString &replaceText)
{
    if (originalText.isEmpty())
        return replaceText;

    // Find common prefix & suffix: these will be unaffected
    const int replaceTextLen = replaceText.length();
    const int originalTextLen = originalText.length();

    int prefixLen = 0;
    for (; prefixLen < replaceTextLen && prefixLen < originalTextLen; ++prefixLen)
        if (replaceText.at(prefixLen).toLower() != originalText.at(prefixLen).toLower())
            break;

    int suffixLen = 0;
    for (; suffixLen < replaceTextLen - prefixLen && suffixLen < originalTextLen - prefixLen; ++suffixLen)
        if (replaceText.at(replaceTextLen - 1 - suffixLen).toLower()
            != originalText.at(originalTextLen - 1 - suffixLen).toLower())
            break;

    // keep prefix and suffix, and do actual replacement on the 'middle' of the string
    return originalText.left(prefixLen)
        + Internal::matchCaseReplacement(originalText.mid(prefixLen, originalTextLen - prefixLen - suffixLen),
                                         replaceText.mid(prefixLen, replaceTextLen - prefixLen - suffixLen))
        + originalText.right(suffixLen);
}

} // namespace Migration