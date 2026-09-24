#ifndef ATTENDANCE_REGISTER_H
#define ATTENDANCE_REGISTER_H
// MEMBER 2 -- the attendance core (FR7.1-FR7.5). Depends only on the
// abstract AttendanceCapture interface, never a concrete type (FR7.6).
#include <string>
#include <vector>
#include <iosfwd>

class Course;
class Student;
class Lecturer;
class AttendanceSession;
class AttendanceRecord;
class CorrectionRecord;

class AttendanceRegister {
private:
    Course* course;
    std::vector<AttendanceSession*> sessions;   // COMPOSITION
    std::vector<CorrectionRecord*> corrections; // COMPOSITION
    int recordCounter = 0;
public:
    explicit AttendanceRegister(Course* course);
    ~AttendanceRegister();
    AttendanceRegister(const AttendanceRegister&) = delete;
    AttendanceRegister& operator=(const AttendanceRegister&) = delete;

    AttendanceSession* createSession(const std::string& date, int start, int end, const std::string& location);
    AttendanceSession* getActiveSession();
    AttendanceSession* getSession(const std::string& id);
    const std::vector<AttendanceSession*>& getSessions() const;
    const std::vector<CorrectionRecord*>& getCorrections() const;
    AttendanceSession* restoreSession(const std::string&, const std::string&, int, int, const std::string&, int, bool);
    bool restoreRecord(AttendanceSession*, Student*, const std::string&, const std::string&, const std::string&, const std::string&);
    bool restoreCorrection(AttendanceRecord*, Lecturer*, const std::string&, const std::string&, const std::string&);

    // FR7.2: throws SessionClosedException / NotEnrolledException /
    // DuplicateAttendanceException on the first violation found.
    void recordAttendance(const std::string& sessionId, Student* student,
                           const std::string& status, const std::string& method);

    // FR7.3: appends a CorrectionRecord; never mutates the original.
    void addCorrection(AttendanceRecord* rec, Lecturer* lecturer, const std::string& reason);

    std::vector<AttendanceRecord*> getRecords(const std::string& sessionId);
    double attendancePercentage(const Student* student) const; // FR7.4
    void generateReport(const std::string& sessionId, std::ostream& os) const; // FR6.1/FR7.4
};
#endif
