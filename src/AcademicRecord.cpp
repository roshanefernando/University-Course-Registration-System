#include "AcademicRecord.h"
#include "Course.h"
#include <algorithm>
double AcademicRecord::getAttendance() const { return attendance; }
int AcademicRecord::getSemester() const { return semester; }
void AcademicRecord::setAttendance(double a) { attendance = a; }
void AcademicRecord::setSemester(int s) { semester = s; }
bool AcademicRecord::hasCompleted(Course* c) const {
    return std::any_of(completedCourses.begin(), completedCourses.end(),
        [&](Course* done) { return done->getCode() == c->getCode(); });
}
void AcademicRecord::addCompletedCourse(Course* c) {
    completedCourses.push_back(c);
}
std::vector<std::string> AcademicRecord::getCompletedCourseCodes() const {
    std::vector<std::string> codes;
    for (Course* c : completedCourses) codes.push_back(c->getCode());
    return codes;
}
