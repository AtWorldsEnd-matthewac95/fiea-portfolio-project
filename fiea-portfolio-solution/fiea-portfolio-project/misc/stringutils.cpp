#include "stringutils.h"

namespace AWE {
    std::string string_rtrim(const std::string& str, const std::string& trimthis) {
        size_t start = str.find_first_not_of(trimthis);
        return (start == std::string::npos) ? "" : str.substr(start);
    }

    std::string string_ltrim(const std::string& str, const std::string& trimthis) {
        size_t end = str.find_last_not_of(trimthis);
        return (end == std::string::npos) ? "" : str.substr(0, end + 1);
    }

    std::string string_trim(const std::string& str, const std::string& trimthis) {
        return string_rtrim(string_ltrim(str, trimthis));
    }
}
