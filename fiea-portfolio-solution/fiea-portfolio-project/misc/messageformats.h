#pragma once
#include <string>

namespace AWE {
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
