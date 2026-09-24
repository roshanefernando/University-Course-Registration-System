#ifndef EXCEPTION_H
#define EXCEPTION_H
// MEMBER 1 -- root of the entire custom exception hierarchy.
#include <exception>
#include <string>

class Exception : public std::exception {
protected:
    std::string errorMessage;
public:
    explicit Exception(const std::string& msg) : errorMessage(msg) {}
    const char* what() const noexcept override { return errorMessage.c_str(); }
    virtual ~Exception() = default;
};
#endif
