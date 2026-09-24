#ifndef ATTENDANCE_RECORD_H
#define ATTENDANCE_RECORD_H
// MEMBER 2 (FR7.3) -- immutable: no setters anywhere, ever.
#include <string>
#include <iosfwd>

class AttendanceSession; // pointer only
class Student;            // pointer only

class AttendanceRecord {
private:
    std::string recordId;
    AttendanceSession* session;
    Student* student;
    std::string timestamp;
    std::string status;   // "Present" / "Late"
    std::string method;   // how it was captured, e.g. "SessionCode" / "FileReplay"
public:
    AttendanceRecord(const std::string& id, AttendanceSession* session, Student* student,
                      const std::string& timestamp, const std::string& status, const std::string& method);
    Student* getStudent() const;
    AttendanceSession* getSession() const;
    const std::string& getStatus() const;
    const std::string& getMethod() const;
    const std::string& getTimestamp() const;
    const std::string& getRecordId() const;
    friend std::ostream& operator<<(std::ostream& os, const AttendanceRecord& r);
};
#endif
