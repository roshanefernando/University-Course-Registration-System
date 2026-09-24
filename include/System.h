#ifndef SYSTEM_H
#define SYSTEM_H
// MEMBER 3 -- integration / composition root.
// NOTE: the UML shows System with NO operations at all. login()/run()/
// addUser()/addCourse() are necessary additions -- something has to
// actually drive the program, and nothing else in the design owns the
// master user/course lists (see Administrator's "no attributes"
// constraint, resolved in Administrator.h).
#include "Storage.h"
#include <vector>
#include <string>

class User;
class Course;

class System {
private:
    std::vector<User*> users;     // COMPOSITION -- System deletes these
    std::vector<Course*> courses; // COMPOSITION
    Storage storage;
public:
    System(const std::string& studentFile, const std::string& courseFile, const std::string& attendanceFile);
    ~System();
    System(const System&) = delete;
    System& operator=(const System&) = delete;

    User* login(const std::string& username, const std::string& password) const; // FR1.1, throws
    void addUser(User* u);
    void addCourse(Course* c);
    void saveAll();
    void loadAll();
    void run(); // main interactive menu loop

    const std::vector<User*>& getUsers() const;
    const std::vector<Course*>& getCourses() const;
    // ADDED: non-const access, used only by Administrator's interactive
    // menu to call its existing addUser()/removeUser()/addCourse()/
    // removeCourse() methods, which take the target list as a mutable
    // reference parameter (see Administrator.h's "no attributes" note).
    std::vector<User*>& getUsersMutable();
    std::vector<Course*>& getCoursesMutable();
    Course* findCourse(const std::string& code) const;
    User* findUser(const std::string& id) const;
};
#endif
