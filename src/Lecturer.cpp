#include "Lecturer.h"
#include "Course.h"
#include "Student.h"
#include "AttendanceCapture.h"
#include "SessionCodeCapture.h"
#include "AttendanceRegister.h"
#include "AttendanceSession.h"
#include "System.h"
#include "AttendanceException.h"
#include <iostream>
#include <ctime>

Lecturer::Lecturer(const std::string& username, const std::string& password)
    : User(username, password) {}

Lecturer::Lecturer(const std::string& id, const std::string& username, const std::string& password, bool isReload)
    : User(id, username, password, isReload) {}

Lecturer::Lecturer(Lecturer&& other) noexcept
    : User(std::move(other)), activeCapture(other.activeCapture) {
    other.activeCapture = nullptr;
}

Lecturer& Lecturer::operator=(Lecturer&& other) noexcept {
    if (this == &other) return *this;
    delete activeCapture;
    activeCapture = other.activeCapture;
    other.activeCapture = nullptr;
    return *this;
}

Lecturer::~Lecturer() {
    delete activeCapture;
}

void Lecturer::displayDashboard() const {
    std::cout << "-- Lecturer Dashboard (" << username << ") --\n"
              << "  1) View enrolled students  2) Open attendance  3) Mark attendance  4) Close attendance  5) Attendance report\n";
}

std::string Lecturer::getRole() const { return "Lecturer"; }

bool Lecturer::viewEnrolledStudents(Course& c) const {
    // FR2.3: own courses only.
    if (c.getAssignedLecturer() != this) {
        std::cout << "Access denied: you do not teach " << c.getCode() << "\n";
        return false;
    }
    std::cout << "Enrolled in " << c.getCode() << ":\n";
    for (Student* s : c.getEnrolledStudents()) {
        std::cout << "  " << *s << "\n";
    }
    return true;
}

void Lecturer::openAttendanceSession(Course& c) {
    if (c.getAssignedLecturer() != this) throw AttendanceException("Lecturer is not assigned to " + c.getCode());
    int start = 0, end = 60;
    const auto& slots = c.getTimetable().getSlots();
    if (!slots.empty()) {
        start = slots.front()->getStartTime();
        end = slots.front()->getEndTime();
    }
    std::time_t now = std::time(nullptr);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", std::localtime(&now));
    AttendanceSession* session = c.getRegister().createSession(buf, start, end, "Lecture Hall");
    if (activeCapture) {
        activeCapture->beginSession(c.getCode());
    }
    std::cout << "Opened session " << session->getSessionId() << " for " << c.getCode() << "\n";
}

void Lecturer::closeAttendanceSession(Course& c) {
    AttendanceSession* active = c.getRegister().getActiveSession();
    if (active) {
        active->close();
        if (activeCapture) activeCapture->endSession();
        std::cout << "Closed session " << active->getSessionId() << "\n";
    } else {
        std::cout << "No active session for " << c.getCode() << "\n";
    }
}


void Lecturer::markAttendance(Course& c) {
    if (c.getAssignedLecturer() != this) {
        throw AttendanceException("Lecturer is not assigned to " + c.getCode());
    }

    AttendanceSession* session = c.getRegister().getActiveSession();
    if (!session) {
        throw SessionClosedException(c.getCode());
    }

    std::cout << "Students enrolled in " << c.getCode() << ":\n";
    for (Student* s : c.getEnrolledStudents()) {
        if (s) {
            std::cout << "  " << s->getId() << " - " << s->getUsername() << "\n";
        }
    }

    std::cout << "Enter student ID: ";
    std::string studentId;
    std::getline(std::cin, studentId);

    Student* student = nullptr;
    for (Student* s : c.getEnrolledStudents()) {
        if (s && s->getId() == studentId) {
            student = s;
            break;
        }
    }

    if (!student) {
        throw NotEnrolledException(session->getSessionId(), studentId);
    }

    std::cout << "Status (1=Present, 2=Absent): ";
    std::string statusChoice;
    std::getline(std::cin, statusChoice);
    std::string status;
    if (statusChoice == "1") {
        status = "Present";
    } else if (statusChoice == "2") {
        status = "Absent";
    } else {
        std::cout << "Invalid attendance status. Use 1 for Present or 2 for Absent.\n";
        return;
    }

    c.getRegister().recordAttendance(session->getSessionId(), student, status, "Manual");
    std::cout << student->getUsername() << " marked " << status << ".\n";
}

void Lecturer::generateAttendanceReport(Course& c) const {
    for (AttendanceSession* s : c.getRegister().getSessions()) {
        c.getRegister().generateReport(s->getSessionId(), std::cout);
    }
}

void Lecturer::setActiveCapture(AttendanceCapture* capture) {
    delete activeCapture;
    activeCapture = capture;
}

AttendanceCapture* Lecturer::getActiveCapture() const { return activeCapture; }

void Lecturer::runInteractiveSession(System& sys) {
    if (!activeCapture) {
        setActiveCapture(new SessionCodeCapture());
    }

    while (true) {
        displayDashboard();
        std::cout << "  0) Logout\n> ";
        std::string choice;
        if (!std::getline(std::cin, choice) || choice == "0") return;

        std::cout << "Your courses:\n";
        for (Course* c : sys.getCourses()) {
            if (c->getAssignedLecturer() == this) std::cout << "  " << *c << "\n";
        }
        std::cout << "Enter course code: ";
        std::string code;
        std::getline(std::cin, code);
        Course* c = sys.findCourse(code);
        if (!c || c->getAssignedLecturer() != this) {
            std::cout << "No such course of yours.\n";
            continue;
        }

        try {
            if (choice == "1") {
                viewEnrolledStudents(*c);
            } else if (choice == "2") {
                openAttendanceSession(*c);
            } else if (choice == "3") {
                markAttendance(*c);
            } else if (choice == "4") {
                closeAttendanceSession(*c);
            } else if (choice == "5") {
                generateAttendanceReport(*c);
            } else {
                std::cout << "Unknown option.\n";
            }
        } catch (const AttendanceException& e) {
            std::cout << "Attendance action failed: " << e.what() << "\n";
        } catch (const std::exception& e) {
            std::cout << "Action failed: " << e.what() << "\n";
        }
    }
}
