#include "AttendanceRecord.h"
#include "AttendanceSession.h"
#include "Student.h"
#include <ostream>

AttendanceRecord::AttendanceRecord(const std::string& id, AttendanceSession* session, Student* student,
                                    const std::string& timestamp, const std::string& status, const std::string& method)
    : recordId(id), session(session), student(student), timestamp(timestamp), status(status), method(method) {}

Student* AttendanceRecord::getStudent() const { return student; }
AttendanceSession* AttendanceRecord::getSession() const { return session; }
const std::string& AttendanceRecord::getStatus() const { return status; }
const std::string& AttendanceRecord::getMethod() const { return method; }
const std::string& AttendanceRecord::getTimestamp() const { return timestamp; }
const std::string& AttendanceRecord::getRecordId() const { return recordId; }

std::ostream& operator<<(std::ostream& os, const AttendanceRecord& r) {
    os << "  " << r.student->getUsername() << " (" << r.student->getId() << ") - "
       << r.status << " via " << r.method << " [session " << r.session->getSessionId() << "]";
    return os;
}
