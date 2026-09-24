#ifndef COURSE_H
#define COURSE_H
// MEMBER 2 (FR2.1/FR2.2).
#include "Timetable.h"
#include <string>
#include <vector>
#include <iosfwd>

class Lecturer;          // M1, aggregation (pointer)
class Student;            // M1, aggregation (pointer)
class AcademicRecord;     // M1, reference param only
class AttendanceRegister; // M2, composition via pointer

class Course {
protected:
    std::string code;
    std::string title;
    int creditValue;
    int capacity;
    Lecturer* assignedLecturer;             // AGGREGATION
    std::vector<Student*> enrolledStudents; // AGGREGATION
    std::vector<Course*> prerequisites;     // AGGREGATION (self)
    Timetable timetable;                    // COMPOSITION (by value)
    AttendanceRegister* attendanceRegister; // COMPOSITION (owns via pointer)
public:
    Course(const std::string& code, const std::string& title, int creditValue,
           int capacity, Lecturer* lecturer);
    Course(const Course&) = delete;   // abstract + owns AttendanceRegister by pointer: non-copyable
    Course& operator=(const Course&) = delete;
    virtual ~Course();

    void enrolStudent(Student& s);  // FR3.1-3.4/FR4.2, throws EnrolmentException subtypes
    void dropStudent(Student& s);   // FR3.1
    void restoreStudent(Student& s); // persistence only: restores an already-saved relationship
    bool checkEligibility(AcademicRecord& record) const; // predicate helper (FR3.4)
    bool verifyNoClash(const std::vector<TimeSlot*>& currentSlots) const; // predicate helper (FR4.2/4.4)

    virtual double finalGrade(double marks) const = 0; // FR2.2
    virtual std::string getCourseType() const = 0;     // small addition: needed for persistence tagging

    std::string getCode() const;
    std::string getTitle() const;
    int getCreditValue() const;
    int getCapacity() const;
    Lecturer* getAssignedLecturer() const;
    void setAssignedLecturer(Lecturer* l); // small addition: needed for Administrator::assignLecturer to actually work
    bool setCapacity(int newCapacity);
    void setTitle(const std::string& newTitle);
    const std::vector<Student*>& getEnrolledStudents() const;
    const std::vector<Course*>& getPrerequisites() const;
    void addPrerequisite(Course* prereq);

    Timetable& getTimetable();
    const Timetable& getTimetable() const;
    AttendanceRegister& getRegister();

    friend std::ostream& operator<<(std::ostream& os, const Course& c); // FR6.2
};

// Free function required by Repository<T>'s genericity contract (see
// Repository.h) -- found via ADL when Repository<Course*> is instantiated,
// since Course is declared in the global namespace, same as Course*.
inline std::string getRecordId(Course* const& c) { return c->getCode(); }

#endif
