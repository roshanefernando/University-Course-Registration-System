#ifndef USER_H
#define USER_H
// MEMBER 1 -- abstract base of the Person/role hierarchy (FR1.1/FR1.3).
#include <string>

class System; // forward declared -- each role's interactive session needs
              // to reach System's data (courses, other users)

class User {
protected:
    std::string id;
    std::string username;
    std::string password;
    static int nextId;
public:
    User(const std::string& username, const std::string& password);
    // Reload constructor: preserves a pre-existing ID instead of
    // generating a new one. Without this, every ID-based relationship
    // (login, Course->Lecturer, AttendanceRecord->Student) would silently
    // break on reload, since IDs would no longer match what was saved.
    User(const std::string& id, const std::string& username, const std::string& password, bool /*isReload*/);
    virtual ~User() = default;

    // A user-declared destructor suppresses the IMPLICIT move
    // constructor/assignment in C++11+ -- declared explicitly here so
    // Lecturer's Rule-of-Five move constructor genuinely moves the base
    // part instead of silently falling back to a copy.
    User(const User&) = default;
    User& operator=(const User&) = default;
    User(User&&) = default;
    User& operator=(User&&) = default;

    std::string getId() const;
    std::string getUsername() const;
    // Small addition: needed by Storage to persist/reload credentials --
    // the UML has no accessor for this (only login() touches password).
    std::string getPassword() const;
    void setUsername(const std::string& newUsername);

    // FR1.1: throws FalseLoginAttemptException on a bad password.
    void login(const std::string& pw) const;

    // FR1.3: polymorphic dashboard -- no if/else on a role string anywhere.
    virtual void displayDashboard() const = 0;
    virtual std::string getRole() const = 0; // small addition: needed for persistence tagging

    // ADDED: the real interactive menu loop for this role. Each
    // subclass owns its own menu and its own dispatch to actual domain
    // methods -- polymorphic dispatch does the "which role does what"
    // job, so there is still no role-string branching anywhere, even
    // though the system is now genuinely interactive. Returns when the
    // user chooses to log out.
    virtual void runInteractiveSession(System& sys) = 0;
};
#endif
