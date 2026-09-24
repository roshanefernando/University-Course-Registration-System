#ifndef STUDENT_H
#define STUDENT_H
// MEMBER 1.
#include "User.h"
#include "AcademicRecord.h"
#include "Timetable.h"
#include <vector>
#include <iosfwd>

class Course; // M2's type, aggregation only (pointer)

class Student : public User {
private:
    AcademicRecord academicRecord;        // COMPOSITION (by value)
    std::vector<Course*> enrolledCourses; // AGGREGATION
    Timetable timetable;                  // COMPOSITION (by value)
public:
    Student(const std::string& username, const std::string& password);
    Student(const std::string& id, const std::string& username, const std::string& password, bool isReload);

    void displayDashboard() const override;
    std::string getRole() const override;
    void runInteractiveSession(System& sys) override;

    void registerCourse(Course& c);   // FR3.1, delegates to operator+=
    void dropCourse(Course& c);       // FR3.1
    const Timetable& viewTimetable() const;  // FR4.3
    void viewAttendance(Course& c) const;    // FR7.4 (per-student view)

    Student& operator+=(Course& c);   // FR3.1/FR4.4, throws EnrolmentException subtypes
    friend std::ostream& operator<<(std::ostream& os, const Student& s); // FR6.2

    // Interface exposed to Member 2's Course:
    const std::vector<Course*>& getEnrolledCourses() const;
    AcademicRecord& getAcademicRecord();
    const Timetable& getTimetableConst() const;
    void rebuildTimetable();

    // Friend-restricted linking, called only by Course during
    // enrolStudent()/dropStudent() to keep both sides of the
    // aggregation in sync.
    friend class Course;
private:
    void addCourseInternal(Course* c);
    void removeCourseInternal(Course* c);
};
#endif
