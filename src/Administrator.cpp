#include "Administrator.h"
#include "Course.h"
#include "Lecturer.h"
#include "Student.h"
#include "LectureBased.h"
#include "LabBased.h"
#include "ProjectBased.h"
#include "System.h"
#include <algorithm>
#include <iostream>
#include <ostream>
#include <limits>
#include <cctype>
Administrator::Administrator(const std::string& username, const std::string& password)
    : User(username, password) {}

Administrator::Administrator(const std::string& id, const std::string& username, const std::string& password, bool isReload)
    : User(id, username, password, isReload) {}

void Administrator::displayDashboard() const {
    std::cout << "-- Administrator Dashboard (" << username << ") --\n"
              << "  1) Manage users  2) Manage courses  3) Assign lecturer  4) Reports\n";
}
std::string Administrator::getRole() const {
     return "Administrator"; 
    }
bool Administrator::addUser(User* u, std::vector<User*>& allUsers) {
    allUsers.push_back(u);
    return true;
}
bool Administrator::updateUser(const std::string& id, const std::string& newUsername, std::vector<User*>& allUsers) {
    for (User* u : allUsers) {
        if (u && u->getId() == id) {
            u->setUsername(newUsername);
            return true;
        }
    }
    return false;
}
bool Administrator::removeUser(const std::string& id, std::vector<User*>& allUsers) {
    auto it = std::find_if(allUsers.begin(), allUsers.end(),
        [&](User* u) { return u && u->getId() == id; });
    if (it == allUsers.end() || *it == nullptr) return false;
    if (auto* student = dynamic_cast<Student*>(*it)) {
        if (!student->getEnrolledCourses().empty()) return false;
    }
    if (auto* lecturer = dynamic_cast<Lecturer*>(*it)) {
        (void)lecturer;
    }
    delete *it;
    allUsers.erase(it);
    return true;
}
bool Administrator::addCourse(Course* c, std::vector<Course*>& allCourses) {
    allCourses.push_back(c);
    return true;
}
bool Administrator::editCourse(const std::string& code, int newCapacity, std::vector<Course*>& allCourses) {
    for (Course* c : allCourses) {
        if (c && c->getCode() == code) return c->setCapacity(newCapacity);
    }
    return false;
}
bool Administrator::removeCourse(const std::string& code, std::vector<Course*>& allCourses) {
    auto it = std::find_if(allCourses.begin(), allCourses.end(),
        [&](Course* c) { return c && c->getCode() == code; });
    if (it == allCourses.end() || *it == nullptr) return false;
    Course* target = *it;
    if (!target->getEnrolledStudents().empty()) return false;
    for (Course* c : allCourses) {
        if (c && std::find(c->getPrerequisites().begin(), c->getPrerequisites().end(), target) != c->getPrerequisites().end()) return false;
    }
    delete target;
    allCourses.erase(it);
    return true;
}
bool Administrator::assignLecturer(Course& c, Lecturer& l) {
    c.setAssignedLecturer(&l);
    return true;
}
void Administrator::generateEnrollmentReport(const std::vector<Course*>& allCourses, std::ostream& os) const {
    os << "=== Enrollment Report ===\n";
    for (Course* c : allCourses) {
        os << *c << "\n";
        for (Student* s : c->getEnrolledStudents()) {
            os << "    - " << *s << "\n";
        }
    }
}
void Administrator::runInteractiveSession(System& sys) {
    while (true) {
        displayDashboard();
        std::cout << "  0) Logout\n> ";
        std::string choice;
        if (!std::getline(std::cin, choice) || choice == "0") return;
        if (choice == "1") {
            std::cout << "  a) Add user   u) Update user   r) Remove user\n> ";
            std::string sub;
            std::getline(std::cin, sub);
            if (sub == "a") {
                std::cout << "Role (Student/Lecturer/Administrator): ";
                std::string role, username, password;
                std::getline(std::cin, role);
                std::cout << "Username: "; std::getline(std::cin, username);
                std::cout << "Password: "; std::getline(std::cin, password);
                User* newUser = nullptr;
                if (role == "Student") newUser = new Student(username, password);
                else if (role == "Lecturer") newUser = new Lecturer(username, password);
                else if (role == "Administrator") newUser = new Administrator(username, password);
                else { std::cout << "Unknown role.\n"; continue; }
                addUser(newUser, sys.getUsersMutable());
                std::cout << "Added " << role << " " << username << " (id " << newUser->getId() << ")\n";
            } else if (sub == "u") {
                std::cout << "User id to update: ";
                std::string id, name;
                std::getline(std::cin, id);
                std::cout << "New username: ";
                std::getline(std::cin, name);
                std::cout << (updateUser(id, name, sys.getUsersMutable()) ? "Updated.\n" : "Not found.\n");
            } else if (sub == "r") {
                std::cout << "User id to remove: ";
                std::string id; std::getline(std::cin, id);
                std::cout << (removeUser(id, sys.getUsersMutable()) ? "Removed.\n" : "Not found.\n");
            }
        } else if (choice == "2") {
            std::cout << "  a) Add course   e) Edit capacity   r) Remove course\n> ";
            std::string sub;
            std::getline(std::cin, sub);
            if (sub == "a") {
                std::cout << "Type (Lecture/Lab/Project): ";
                std::string type, code, title, lecturerId;
                int credit, capacity;
                std::getline(std::cin, type);
                std::cout << "Code: "; std::getline(std::cin, code);
                std::cout << "Title: "; std::getline(std::cin, title);
                std::cout << "Credit value: "; std::cin >> credit;
                std::cout << "Capacity: "; std::cin >> capacity;
                std::cin.ignore();
                std::cout << "Lecturer id (blank for none): "; std::getline(std::cin, lecturerId);
                Lecturer* lecturer = dynamic_cast<Lecturer*>(sys.findUser(lecturerId));
                Course* newCourse = nullptr;
                if (type == "Lecture") newCourse = new LectureBased(code, title, credit, capacity, lecturer);
                else if (type == "Lab") newCourse = new LabBased(code, title, credit, capacity, lecturer);
                else if (type == "Project") newCourse = new ProjectBased(code, title, credit, capacity, lecturer);
                else { std::cout << "Unknown type.\n"; continue; }
                // Add the course first, then let the administrator enter its
                // weekly timetable slots.  The existing Course/Timetable design
                // is unchanged; we simply populate the course's Timetable here.
                addCourse(newCourse, sys.getCoursesMutable());

                std::cout << "Number of weekly time slots (0 for none): ";
                std::string slotCountText;
                std::getline(std::cin, slotCountText);
                int slotCount = 0;
                try {
                    slotCount = std::stoi(slotCountText);
                    if (slotCount < 0) throw std::invalid_argument("negative");
                } catch (...) {
                    std::cout << "Invalid slot count. Course added without time slots.\n";
                    slotCount = 0;
                }

                auto parseDay = [](const std::string& text, DayOfWeek& day) -> bool {
                    std::string d = text;
                    for (char& ch : d) ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
                    if (d == "monday" || d == "mon") { day = DayOfWeek::MONDAY; return true; }
                    if (d == "tuesday" || d == "tue") { day = DayOfWeek::TUESDAY; return true; }
                    if (d == "wednesday" || d == "wed") { day = DayOfWeek::WEDNESDAY; return true; }
                    if (d == "thursday" || d == "thu") { day = DayOfWeek::THURSDAY; return true; }
                    if (d == "friday" || d == "fri") { day = DayOfWeek::FRIDAY; return true; }
                    if (d == "saturday" || d == "sat") { day = DayOfWeek::SATURDAY; return true; }
                    if (d == "sunday" || d == "sun") { day = DayOfWeek::SUNDAY; return true; }
                    return false;
                };

                auto parseTime = [](const std::string& text, int& minutes) -> bool {
                    const auto colon = text.find(':');
                    if (colon == std::string::npos) return false;
                    try {
                        int hour = std::stoi(text.substr(0, colon));
                        int minute = std::stoi(text.substr(colon + 1));
                        if (hour < 0 || hour > 23 || minute < 0 || minute > 59) return false;
                        minutes = hour * 60 + minute;
                        return true;
                    } catch (...) {
                        return false;
                    }
                };

                for (int i = 0; i < slotCount; ++i) {
                    while (true) {
                        std::string dayText, startText, endText, location;
                        std::cout << "  Slot " << (i + 1) << " day (Monday-Sunday): ";
                        std::getline(std::cin, dayText);
                        std::cout << "  Start time (HH:MM): ";
                        std::getline(std::cin, startText);
                        std::cout << "  End time (HH:MM): ";
                        std::getline(std::cin, endText);
                        std::cout << "  Location: ";
                        std::getline(std::cin, location);

                        DayOfWeek day;
                        int start = 0, end = 0;
                        if (!parseDay(dayText, day) || !parseTime(startText, start) ||
                            !parseTime(endText, end) || start >= end || location.empty()) {
                            std::cout << "  Invalid time slot. Please enter a valid day, time range and location.\n";
                            continue;
                        }

                        // Reject a clash inside the new course itself.
                        TimeSlot candidate(day, start, end, location);
                        bool duplicateClash = false;
                        for (TimeSlot* existing : newCourse->getTimetable().getSlots()) {
                            if (existing && *existing == candidate) {
                                duplicateClash = true;
                                break;
                            }
                        }
                        if (duplicateClash) {
                            std::cout << "  This slot clashes with another slot of the same course. Try again.\n";
                            continue;
                        }

                        newCourse->getTimetable().addTimeSlot(candidate);
                        std::cout << "  Added slot: " << candidate << "\n";
                        break;
                    }
                }

                std::cout << "Added " << code << " with "
                          << newCourse->getTimetable().getSlots().size()
                          << " time slot(s).\n";
            } else if (sub == "e") {
                std::string code, capText;
                std::cout << "Course code to edit: "; std::getline(std::cin, code);
                std::cout << "New capacity: "; std::getline(std::cin, capText);
                try {
                    std::cout << (editCourse(code, std::stoi(capText), sys.getCoursesMutable()) ? "Updated.\n" : "Update rejected.\n");
                } catch (const std::exception&) { std::cout << "Invalid capacity.\n"; }
            } else if (sub == "r") {
                std::cout << "Course code to remove: ";
                std::string code; std::getline(std::cin, code);
                std::cout << (removeCourse(code, sys.getCoursesMutable()) ? "Removed.\n" : "Not found.\n");
            }
        } else if (choice == "3") {
            std::cout << "Course code: ";
            std::string code; std::getline(std::cin, code);
            std::cout << "Lecturer id: ";
            std::string lecturerId; std::getline(std::cin, lecturerId);
            Course* c = sys.findCourse(code);
            Lecturer* l = dynamic_cast<Lecturer*>(sys.findUser(lecturerId));
            if (c && l) { assignLecturer(*c, *l); std::cout << "Assigned.\n"; }
            else std::cout << "Course or lecturer not found.\n";
        } else if (choice == "4") {
            generateEnrollmentReport(sys.getCourses(), std::cout);
        } else {
            std::cout << "Unknown option.\n";
        }
    }
}
