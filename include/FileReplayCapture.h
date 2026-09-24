#ifndef FILE_REPLAY_CAPTURE_H
#define FILE_REPLAY_CAPTURE_H
// MEMBER 2 (FR7.7a) -- mandatory for all groups.
#include "AttendanceCapture.h"
#include <fstream>
#include <string>

class FileReplayCapture : public AttendanceCapture {
private:
    std::string logFilePath;
    std::ifstream stream;
    int lineNumber = 0;
public:
    explicit FileReplayCapture(const std::string& path);
    void beginSession(const std::string& courseCode) override;
    // Throws CorruptDataException (Member 3's type) on a malformed line --
    // a line is malformed if it is empty or contains whitespace/pipe
    // characters (reserved for storage records elsewhere in the system).
    std::string captureNext() override;
    void endSession() override;
};
#endif
