#ifndef ATTENDANCE_CAPTURE_H
#define ATTENDANCE_CAPTURE_H
// MEMBER 2 (FR7.6). Abstract interface -- the attendance core depends
// only on this, never on a concrete capture class (dependency inversion).
#include <string>

class AttendanceCapture {
public:
    virtual void beginSession(const std::string& courseCode) = 0;
    virtual std::string captureNext() = 0; // "" means nothing captured
    virtual void endSession() = 0;
    virtual ~AttendanceCapture() = default;
};
#endif
