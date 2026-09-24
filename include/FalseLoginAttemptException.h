#ifndef FALSE_LOGIN_ATTEMPT_EXCEPTION_H
#define FALSE_LOGIN_ATTEMPT_EXCEPTION_H
// MEMBER 1 -- thrown by User::login() on a bad password (FR1.1).
#include "Exception.h"

class FalseLoginAttemptException : public Exception {
public:
    explicit FalseLoginAttemptException(const std::string& username)
        : Exception("Login failed for user: " + username) {}
};
#endif
