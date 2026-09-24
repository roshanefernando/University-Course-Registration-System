#include "Student.h"
#include "Course.h"
#include "AttendanceRegister.h"
#include "System.h"
#include "EnrolmentException.h"
#include <algorithm>
#include <iostream>
#include <ostream>

Student::Student(const std::string& username, const std::string& password)
    : User(username, password) {}

Student::Student(const std::string& id, const std::string& username, const std::string& password, bool isReload)
    : User(id, username, password, isReload) {}

void Student::displayDashboard() const {
    std::cout << "-- Student Dashboard (" << username << ") --\n"
              << "  1) Register course  2) Drop course  3) View timetable  4) View attendance\n";
}

std::string Student::getRole() const { return "Student"; }

Student& Student::operator+=(Course& c) {
    c.enrolStudent(*this);
    for (TimeSlot* slot : c.getTimetable().getSlots()) {
        timetable.addTimeSlot(*slot);
    }
    return *this;
}

void Student::registerCourse(Course& c) {
    *this += c;
}

void Student::dropCourse(Course& c) {
    c.dropStudent(*this);
    timetable = Timetable();
    for (Course* remaining : enrolledCourses) {
        for (TimeSlot* slot : remaining->getTimetable().getSlots()) {
            timetable.addTimeSlot(*slot);
        }
    }
}

const Timetable& Student::viewTimetable() const {
    return timetable;
}

void Student::viewAttendance(Course& c) const {
    if (std::find(enrolledCourses.begin(), enrolledCourses.end(), &c) == enrolledCourses.end()) {
        throw EnrolmentException("Student is not enrolled in " + c.getCode());
    }
    double pct = c.getRegister().attendancePercentage(this);
    std::cout << "  " << c.getCode() << ": " << pct << "% attendance\n";
}

const std::vector<Course*>& Student::getEnrolledCourses() const { return enrolledCourses; }
AcademicRecord& Student::getAcademicRecord() { return academicRecord; }
const Timetable& Student::getTimetableConst() const { return timetable; }
void Student::rebuildTimetable() {
    timetable = Timetable();
    for (Course* c : enrolledCourses) {
        if (!c) continue;
        for (TimeSlot* slot : c->getTimetable().getSlots()) timetable.addTimeSlot(*slot);
    }
}

void Student::addCourseInternal(Course* c) { enrolledCourses.push_back(c); }
void Student::removeCourseInternal(Course* c) {
    auto it = std::find(enrolledCourses.begin(), enrolledCourses.end(), c);
    if (it != enrolledCourses.end()) enrolledCourses.erase(it);
}

std::ostream& operator<<(std::ostream& os, const Student& s) {
    os << "[" << s.getId() << "] " << s.getUsername()
       << " (Student) -- enrolled in " << s.enrolledCourses.size() << " course(s)";
    return os;
}

void Student::runInteractiveSession(System& sys) {
    while (true) {
        displayDashboard();
        std::cout << "  0) Logout\n> ";
        std::string choice;
        if (!std::getline(std::cin, choice) || choice == "0") return;

        try {
            if (choice == "1") {
                std::cout << "Available courses:\n";
                for (Course* c : sys.getCourses()) std::cout << "  " << *c << "\n";
                std::cout << "Enter course code to register: ";
                std::string code;
                std::getline(std::cin, code);
                Course* c = sys.findCourse(code);
                if (!c) { std::cout << "No such course.\n"; continue; }
                registerCourse(*c);
                std::cout << "Registered in " << code << ".\n";
            } else if (choice == "2") {
                std::cout << "Your courses:\n";
                for (Course* c : enrolledCourses) std::cout << "  " << *c << "\n";
                std::cout << "Enter course code to drop: ";
                std::string code;
                std::getline(std::cin, code);
                Course* c = sys.findCourse(code);
                if (!c) { std::cout << "No such course.\n"; continue; }
                dropCourse(*c);
                std::cout << "Dropped " << code << ".\n";
            } else if (choice == "3") {
                std::cout << "Your timetable:\n" << viewTimetable();
            } else if (choice == "4") {
                std::cout << "Your courses:\n";
                for (Course* c : enrolledCourses) std::cout << "  " << *c << "\n";
                std::cout << "Enter course code to view attendance for: ";
                std::string code;
                std::getline(std::cin, code);
                Course* c = sys.findCourse(code);
                if (!c) { std::cout << "No such course.\n"; continue; }
                viewAttendance(*c);
            } else {
                std::cout << "Unknown option.\n";
            }
        } catch (const EnrolmentException& e) {
            std::cout << "Could not complete that action: " << e.what() << "\n";
        }
    }
}
