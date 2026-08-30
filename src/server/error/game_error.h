#pragma once
#include <stdexcept>

namespace error{
class InitializationException : public std::runtime_error{
    public:
        explicit InitializationException(const std::string& msg) : std::runtime_error(msg) {}
};

class InGameException : public std::runtime_error{
    public:
        explicit InGameException(const std::string& msg) : std::runtime_error(msg) {}
};
}