#ifndef LECTURER_H
#define LECTURER_H
// MEMBER 1.
#include "User.h"

class AttendanceCapture; // M2's abstract type, pointer only
class Course;             // M2's type, reference/pointer only

class Lecturer : public User {
private:
    AttendanceCapture* activeCapture = nullptr; // COMPOSITION 0..1
public:
    Lecturer(const std::string& username, const std::string& password);
    Lecturer(const std::string& id, const std::string& username, const std::string& password, bool isReload);

    // --- Rule of Five #1: DELETED COPY (distinct style from Timetable's
    // deep copy) ------------------------------------------------------
    // AttendanceCapture has no clone(); there is no safe way to deep-copy
    // an arbitrary concrete capture through the base pointer, so copying
    // a Lecturer is explicitly disallowed rather than silently shallow-
    // copying (which would double-delete). Moving just transfers
    // ownership, which is safe and is implemented normally.
    Lecturer(const Lecturer&) = delete;
    Lecturer& operator=(const Lecturer&) = delete;
    Lecturer(Lecturer&& other) noexcept;
    Lecturer& operator=(Lecturer&& other) noexcept;
    ~Lecturer() override;
    // -------------------------------------------------------------------

    void displayDashboard() const override;
    std::string getRole() const override;
    void runInteractiveSession(System& sys) override;

    bool viewEnrolledStudents(Course& c) const;    // FR2.3 (own courses only)
    void openAttendanceSession(Course& c);          // FR7.1
    void closeAttendanceSession(Course& c);         // FR7.1
    void markAttendance(Course& c);                  // FR7.2 / interactive manual marking
    void generateAttendanceReport(Course& c) const; // FR7.4/FR6.1

    void setActiveCapture(AttendanceCapture* capture); // FR7.8, takes ownership
    AttendanceCapture* getActiveCapture() const;
};
#endif
