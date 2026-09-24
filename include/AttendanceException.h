#ifndef ATTENDANCE_EXCEPTION_H
#define ATTENDANCE_EXCEPTION_H
// MEMBER 2 (FR7.2) -- thrown by AttendanceRegister::recordAttendance().
#include "Exception.h"

class AttendanceException : public Exception {
public:
    explicit AttendanceException(const std::string& msg) : Exception(msg) {}
};
class NotEnrolledException : public AttendanceException {
public:
    NotEnrolledException(const std::string& sessionId, const std::string& studentId)
        : AttendanceException("Session " + sessionId + ": student " + studentId + " is not enrolled") {}
};
class DuplicateAttendanceException : public AttendanceException {
public:
    DuplicateAttendanceException(const std::string& sessionId, const std::string& studentId)
        : AttendanceException("Session " + sessionId + ": student " + studentId + " already marked") {}
};
class SessionClosedException : public AttendanceException {
public:
    explicit SessionClosedException(const std::string& sessionId)
        : AttendanceException("Session " + sessionId + ": session is closed") {}
};
#endif
