#include "Course.h"
#include "Student.h"
#include "AcademicRecord.h"
#include "AttendanceRegister.h"
#include "EnrolmentException.h"
#include <algorithm>
#include <ostream>

Course::Course(const std::string& code, const std::string& title, int creditValue,
               int capacity, Lecturer* lecturer)
    : code(code), title(title), creditValue(creditValue), capacity(capacity),
      assignedLecturer(lecturer) {
    attendanceRegister = new AttendanceRegister(this);
}

Course::~Course() {
    delete attendanceRegister;
}

void Course::enrolStudent(Student& s) {
    if (std::find(enrolledStudents.begin(), enrolledStudents.end(), &s) != enrolledStudents.end()) {
        throw EnrolmentException("Student is already enrolled in " + code);
    }
    if (static_cast<int>(enrolledStudents.size()) >= capacity) {
        throw CourseFullException(code);
    }
    if (!checkEligibility(s.getAcademicRecord())) {
        throw PrerequisitesNotMetException(code);
    }
    if (!verifyNoClash(s.getTimetableConst().getSlots())) {
        throw TimeClashException(code);
    }
    enrolledStudents.push_back(&s);
    s.addCourseInternal(this);
}

void Course::restoreStudent(Student& s) {
    if (std::find(enrolledStudents.begin(), enrolledStudents.end(), &s) != enrolledStudents.end()) return;
    enrolledStudents.push_back(&s);
    s.addCourseInternal(this);
}

void Course::dropStudent(Student& s) {
    auto it = std::find(enrolledStudents.begin(), enrolledStudents.end(), &s);
    if (it != enrolledStudents.end()) {
        enrolledStudents.erase(it);
        s.removeCourseInternal(this);
    }
}

bool Course::checkEligibility(AcademicRecord& record) const {
    for (Course* prereq : prerequisites) {
        if (!record.hasCompleted(prereq)) return false;
    }
    return true;
}

bool Course::verifyNoClash(const std::vector<TimeSlot*>& currentSlots) const {
    return !timetable.hasClash(currentSlots);
}

std::string Course::getCode() const { return code; }
std::string Course::getTitle() const { return title; }
int Course::getCreditValue() const { return creditValue; }
int Course::getCapacity() const { return capacity; }
Lecturer* Course::getAssignedLecturer() const { return assignedLecturer; }
void Course::setAssignedLecturer(Lecturer* l) { assignedLecturer = l; }
bool Course::setCapacity(int newCapacity) {
    if (newCapacity < static_cast<int>(enrolledStudents.size()) || newCapacity <= 0) return false;
    capacity = newCapacity;
    return true;
}
void Course::setTitle(const std::string& newTitle) { title = newTitle; }
const std::vector<Student*>& Course::getEnrolledStudents() const { return enrolledStudents; }
const std::vector<Course*>& Course::getPrerequisites() const { return prerequisites; }
void Course::addPrerequisite(Course* prereq) { prerequisites.push_back(prereq); }

Timetable& Course::getTimetable() { return timetable; }
const Timetable& Course::getTimetable() const { return timetable; }
AttendanceRegister& Course::getRegister() { return *attendanceRegister; }

std::ostream& operator<<(std::ostream& os, const Course& c) {
    os << "[" << c.code << "] " << c.title << " (" << c.creditValue << " credits, "
       << c.enrolledStudents.size() << "/" << c.capacity << " enrolled, "
       << c.getCourseType() << ")";
    return os;
}
