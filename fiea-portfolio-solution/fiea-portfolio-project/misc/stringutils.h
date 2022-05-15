#pragma once
#include <string>

namespace AWE {
    /// <summary>
    /// String representing most known whitespace characters.
    /// </summary>
    static const std::string STR_WHITESPACE = " \n\r\t\f\v";

    /// <summary>
    /// Trim the given string from the beginning according to the given regex.
    /// </summary>
    /// <param name="str">The string to trim. Note this is const - this string is not actually modified. A copy with the modifications applied is returned</param>
    /// <param name="trimthis">Any length of the string starting from the beginning which matches this regex will be trimmed.</param>
    /// <returns>A copy of the given string with the modifications applied.</returns>
    std::string string_ltrim(const std::string& str, const std::string& trimthis = STR_WHITESPACE);
    /// <summary>
    /// Trim the given string from the end going backwards according to the given regex.
    /// </summary>
    /// <param name="str">The string to trim. Note this is const - this string is not actually modified. A copy with the modifications applied is returned</param>
    /// <param name="trimthis">Any length of the string starting from the end going backwards which matches this regex will be trimmed.</param>
    /// <returns>A copy of the given string with the modifications applied.</returns>
    std::string string_rtrim(const std::string& str, const std::string& trimthis = STR_WHITESPACE);
    /// <summary>
    /// Trim the given string from the beginning and from the end going backwards according to the given regex.
    /// </summary>
    /// <param name="str">The string to trim. Note this is const - this string is not actually modified. A copy with the modifications applied is returned</param>
    /// <param name="trimthis">Any length of the string starting from the beginning and from the end going backwards which matches this regex will be trimmed.</param>
    /// <returns>A copy of the given string with the modifications applied.</returns>
    std::string string_trim(const std::string& str, const std::string& trimthis = STR_WHITESPACE);
}
