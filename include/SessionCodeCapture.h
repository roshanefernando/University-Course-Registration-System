#ifndef SESSION_CODE_CAPTURE_H
#define SESSION_CODE_CAPTURE_H
// MEMBER 2 (FR7.7b) -- chosen interactive mechanism (Option A: rotating
// session code).
#include "AttendanceCapture.h"
#include <string>
#include <vector>
#include <chrono>

class SessionCodeCapture : public AttendanceCapture {
private:
    std::string currentCode;
    int expiryTime = 300; // seconds; illustrative, not wall-clock enforced in this console demo
    bool sessionOpen = false;
    std::chrono::steady_clock::time_point startedAt;
    std::vector<std::string> pendingIds;
public:
    SessionCodeCapture() = default;
    void beginSession(const std::string& courseCode) override;
    std::string captureNext() override;
    void endSession() override;

    // Called by the student-facing menu (System/UI layer) when a student
    // types in the code they were shown.
    bool submitCode(const std::string& studentId, const std::string& code);
    const std::string& getCurrentCode() const;
private:
    std::string generateRandomCode();
};
#endif
