#include "System.h"
#include "User.h"
#include "Student.h"
#include "Lecturer.h"
#include "Administrator.h"
#include "Course.h"
#include "FalseLoginAttemptException.h"
#include "StorageException.h"
#include <algorithm>
#include <iostream>

System::System(const std::string& studentFile, const std::string& courseFile, const std::string& attendanceFile)
    : storage(studentFile, courseFile, attendanceFile) {}

System::~System() {
    for (User* u : users) delete u;
    for (Course* c : courses) delete c;
}

User* System::login(const std::string& username, const std::string& password) const {
    for (User* u : users) {
        if (u->getUsername() == username) {
            u->login(password);
            return u;
        }
    }
    throw FalseLoginAttemptException(username);
}

void System::addUser(User* u) { users.push_back(u); }
void System::addCourse(Course* c) { courses.push_back(c); }

void System::saveAll() { storage.saveAllData(users, courses); }
void System::loadAll() { storage.loadAllData(users, courses); }

const std::vector<User*>& System::getUsers() const { return users; }
const std::vector<Course*>& System::getCourses() const { return courses; }
std::vector<User*>& System::getUsersMutable() { return users; }
std::vector<Course*>& System::getCoursesMutable() { return courses; }

Course* System::findCourse(const std::string& code) const {
    for (Course* c : courses) if (c->getCode() == code) return c;
    return nullptr;
}

User* System::findUser(const std::string& id) const {
    for (User* u : users) if (u->getId() == id) return u;
    return nullptr;
}

void System::run() {
    std::cout << "\n=== University System ===\n";
    std::cout << "Sample logins: alice/pw1 (Student), amara/lecpass (Lecturer), admin/adminpass (Administrator)\n";
    std::cout << "Type a username to log in, or 'quit' to exit.\n";

    std::string username;
    while (true) {
        std::cout << "\nUsername: ";
        if (!std::getline(std::cin, username) || username == "quit") break;
        std::cout << "Password: ";
        std::string password;
        std::getline(std::cin, password);

        try {
            User* u = login(username, password);
            u->runInteractiveSession(*this); 
            saveAll();
            std::cout << "Changes saved successfully.\n";
        } catch (const FalseLoginAttemptException& e) {
            std::cout << "Login failed: " << e.what() << "\n";
        } catch (const StorageException& e) {
            std::cout << "Could not save data: " << e.what() << "\n";
        } catch (const std::exception& e) {
            std::cout << "Action failed: " << e.what() << "\n";
        }
    }
    try {
        saveAll();
        std::cout << "Final changes saved.\n";
    } catch (const StorageException& e) {
        std::cout << "Could not save final changes: " << e.what() << "\n";
    }
    std::cout << "Goodbye.\n";
}
