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
        MessageFormats();
        MessageFormats(std::string success, std::string failure, std::string other = "");

        std::string success();
        std::string failure();
        std::string other();
        bool isSuccess() const;

        std::string success(std::string);
        std::string failure(std::string);
        std::string other(std::string);
        bool isSuccess(bool);
    };
}
