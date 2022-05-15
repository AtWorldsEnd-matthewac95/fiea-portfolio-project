#include "messageformats.h"

namespace AWE {
    MessageFormats::MessageFormats() : _success(""), _failure(""), _isSuccess(false) {}

    MessageFormats::MessageFormats(std::string success, std::string failure, std::string other) : _success(std::move(success)), _failure(std::move(failure)), _other(std::move(other)), _isSuccess(false) {}

    std::string MessageFormats::success() { return _success; }
    std::string MessageFormats::failure() { return _failure; }
    std::string MessageFormats::other() { return _other; }
    bool MessageFormats::isSuccess() const { return _isSuccess; }

    std::string MessageFormats::success(std::string val) { _success = std::move(val); return _success; }
    std::string MessageFormats::failure(std::string val) { _failure = std::move(val); return _failure; }
    std::string MessageFormats::other(std::string val) { _other = std::move(val); return _other; }
    bool MessageFormats::isSuccess(bool val) { _isSuccess = std::move(val); return _isSuccess; }
}
