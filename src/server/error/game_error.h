#pragma once
#include <stdexcept>

namespace error{
class PlayerJoinException : public std::runtime_error{
    public:
        explicit PlayerJoinException(const std::string& msg) : std::runtime_error(msg) {}
};

class InGameException : public std::runtime_error{
    public:
        explicit InGameException(const std::string& msg) : std::runtime_error(msg) {}
};
}