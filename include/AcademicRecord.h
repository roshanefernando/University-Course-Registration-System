#ifndef ACADEMIC_RECORD_H
#define ACADEMIC_RECORD_H
// MEMBER 1.
#include <vector>
#include <string>

class Course; // M2's type; only ever used as a pointer here

class AcademicRecord {
private:
    double attendance = 0.0;
    int semester = 1;
    std::vector<Course*> completedCourses;
public:
    AcademicRecord() = default;
    double getAttendance() const;
    int getSemester() const;
    void setAttendance(double a);
    void setSemester(int s);
    bool hasCompleted(Course* c) const; // compares by course code, not pointer identity
    void addCompletedCourse(Course* c);
    // Small addition: needed by Storage to persist completed-course codes.
    std::vector<std::string> getCompletedCourseCodes() const;
};
#endif
