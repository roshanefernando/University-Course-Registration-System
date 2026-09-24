#ifndef ATTENDANCE_SESSION_H
#define ATTENDANCE_SESSION_H
#include <string>
#include <vector>
#include <chrono>
class Course; class AttendanceRecord;
class AttendanceSession {
private:
    std::string sessionId; Course* course; std::string date; int startTime, endTime; std::string location;
    bool open_ = true; int durationSeconds = 300; std::chrono::steady_clock::time_point openedAt;
    std::vector<AttendanceRecord*> records;
public:
    AttendanceSession(const std::string& id, Course* course, const std::string& date, int startTime, int endTime, const std::string& location, int durationSeconds=300);
    ~AttendanceSession(); AttendanceSession(const AttendanceSession&)=delete; AttendanceSession& operator=(const AttendanceSession&)=delete;
    void open(); void close(); bool isOpen() const;
    const std::string& getSessionId() const; Course* getCourse() const; const std::string& getDate() const; int getStartTime() const; int getEndTime() const; const std::string& getLocation() const; int getDurationSeconds() const;
    void addRecord(AttendanceRecord* r); const std::vector<AttendanceRecord*>& getRecords() const;
};
#endif
