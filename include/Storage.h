#ifndef STORAGE_H
#define STORAGE_H
// MEMBER 3 (FR5.1/FR5.2/FR5.3).
#include <string>
#include <vector>

class User;
class Course;

class Storage {
private:
    std::string studentFilePath;   // reused for ALL users, not just students -- see README
    std::string courseFilePath;
    std::string attendanceFilePath;
public:
    Storage(const std::string& studentFile, const std::string& courseFile, const std::string& attendanceFile);

    // NOTE (logged deviation): the UML types this vector<Student*>&, but
    // FR5.1 requires ALL users (Student/Lecturer/Administrator) to
    // persist -- a vector<Student*> cannot hold a Lecturer*, so this is
    // widened to vector<User*>&. Signed off as a signature-only change
    // per the spec's own conformance rule (Section 8.3).
    bool loadAllData(std::vector<User*>& users, std::vector<Course*>& courses);
    bool saveAllData(const std::vector<User*>& users, const std::vector<Course*>& courses);
};
#endif
