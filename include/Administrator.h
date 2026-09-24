#ifndef ADMINISTRATOR_H
#define ADMINISTRATOR_H
// MEMBER 1.
// ASSUMPTION (flagged per instructions): the UML's Administrator box has
// NO attributes, so it cannot hold a pointer back to a master user/course
// list. Every method that mutates such a list therefore takes that
// collection as an explicit parameter -- System (Member 3) owns the real
// storage and calls into Administrator for the business-rule checks.
#include "User.h"
#include <vector>
#include <string>
#include <iosfwd>

class Course;
class Lecturer;

class Administrator : public User {
public:
    Administrator(const std::string& username, const std::string& password);
    Administrator(const std::string& id, const std::string& username, const std::string& password, bool isReload);

    void displayDashboard() const override;
    std::string getRole() const override;
    void runInteractiveSession(System& sys) override;

    // FR1.2
    bool addUser(User* u, std::vector<User*>& allUsers);
    bool updateUser(const std::string& id, const std::string& newUsername, std::vector<User*>& allUsers);
    bool removeUser(const std::string& id, std::vector<User*>& allUsers);

    // FR2.1
    bool addCourse(Course* c, std::vector<Course*>& allCourses);
    bool editCourse(const std::string& code, int newCapacity, std::vector<Course*>& allCourses);
    bool removeCourse(const std::string& code, std::vector<Course*>& allCourses);
    bool assignLecturer(Course& c, Lecturer& l);

    // FR6.1
    void generateEnrollmentReport(const std::vector<Course*>& allCourses, std::ostream& os) const;
};
#endif
