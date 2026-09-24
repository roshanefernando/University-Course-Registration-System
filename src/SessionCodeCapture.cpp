#include "SessionCodeCapture.h"
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <chrono>

void SessionCodeCapture::beginSession(const std::string& courseCode) {
    (void)courseCode;
    currentCode = generateRandomCode();
    sessionOpen = true;
    startedAt = std::chrono::steady_clock::now();
    pendingIds.clear();
    std::cout << "Attendance code: " << currentCode << " (students, enter this now)\n";
}

std::string SessionCodeCapture::generateRandomCode() {
    static const char digits[] = "0123456789";
    std::string code;
    for (int i = 0; i < 4; ++i) code += digits[std::rand() % 10];
    return code;
}

bool SessionCodeCapture::submitCode(const std::string& studentId, const std::string& code) {
    if (sessionOpen && std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - startedAt).count() >= expiryTime) endSession();
    if (!sessionOpen || code != currentCode) return false;
    if (std::find(pendingIds.begin(), pendingIds.end(), studentId) == pendingIds.end()) {
        pendingIds.push_back(studentId);
    }
    return true;
}

std::string SessionCodeCapture::captureNext() {
    if (sessionOpen && std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - startedAt).count() >= expiryTime) endSession();
    if (pendingIds.empty()) return "";
    std::string id = pendingIds.front();
    pendingIds.erase(pendingIds.begin());
    return id;
}

void SessionCodeCapture::endSession() {
    sessionOpen = false;
    currentCode.clear();
    pendingIds.clear();
}

const std::string& SessionCodeCapture::getCurrentCode() const { return currentCode; }
