#include "TextUtils.h"

#include <cctype>
#include <unordered_set>

static const std::unordered_set<std::string> STOP_WORDS = {
"is","the","a","an","and","or","but","in","on","at","to","for",
"of","with","by","from","as","be","have","has","had","do","does"
};

namespace TextUtils
{

std::string toLower(const std::string& s)
{
    std::string r;

    for (unsigned char c : s)
        r.push_back(std::tolower(c));

    return r;
}

std::vector<std::string> tokenize(const std::string& text)
{
    std::vector<std::string> tokens;

    std::string word;

    for (unsigned char c : text)
    {
        if (std::isalpha(c) || std::isdigit(c))
            word.push_back(std::tolower(c));
        else
        {
            if (!word.empty())
            {
                if (!STOP_WORDS.count(word))
                    tokens.push_back(word);

                word.clear();
            }
        }
    }

    if (!word.empty() && !STOP_WORDS.count(word))
        tokens.push_back(word);

    return tokens;
}

}