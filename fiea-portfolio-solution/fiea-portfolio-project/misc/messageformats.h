#pragma once
#include <string>

namespace AWE {
    /// <summary>
    /// Flexible class which stores a success message and a failure message. Makes logging convenient. Used primarily in the LOV load.
    /// </summary>
    class MessageFormats {
    private:
        std::string _success;
        std::string _failure;
        std::string _other;
        bool _isSuccess;

    public:
        /// <summary>
        /// Default constructor. Sets all strings to the empty string.
        /// </summary>
        MessageFormats();
        MessageFormats(std::string success, std::string failure, std::string other = "");

        /// <returns>Success message.</returns>
        std::string success();
        /// <returns>Failure message.</returns>
        std::string failure();
        /// <returns>Message which either provides additional information or does not pertain to either success or failure. Context-dependent.</returns>
        std::string other();
        /// <returns>Indicates whether the success message should be shown.</returns>
        bool isSuccess() const;

        /// <summary>
        /// Unlike most setters in this project, this setter returns the new value rather than the old value.
        /// </summary>
        /// <param name="">New value for success message.</param>
        /// <returns>New value for success message.</returns>
        std::string success(std::string);
        /// <summary>
        /// Unlike most setters in this project, this setter returns the new value rather than the old value.
        /// </summary>
        /// <param name="">New value for failure message.</param>
        /// <returns>New value for failure message.</returns>
        std::string failure(std::string);
        /// <summary>
        /// Unlike most setters in this project, this setter returns the new value rather than the old value.
        /// </summary>
        /// <param name="">New value for context-dependent message.</param>
        /// <returns>New value for context-dependent message.</returns>
        std::string other(std::string);
        /// <param name="">New value for success flag.</param>
        /// <returns>Old value for success flag.</returns>
        bool isSuccess(bool);
    };
}
