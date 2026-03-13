#pragma once

#include <string>
#include <vector>

namespace TextUtils
{
    std::string toLower(const std::string& s);

    std::vector<std::string> tokenize(const std::string& text);
}