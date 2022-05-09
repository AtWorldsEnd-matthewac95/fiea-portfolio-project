#pragma once
#include <string>

namespace AWE {
    static const std::string STR_WHITESPACE = " \n\r\t\f\v";

    std::string string_ltrim(const std::string& str, const std::string& trimthis = STR_WHITESPACE);
    std::string string_rtrim(const std::string& str, const std::string& trimthis = STR_WHITESPACE);
    std::string string_trim(const std::string& str, const std::string& trimthis = STR_WHITESPACE);
}
