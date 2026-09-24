#include "FileReplayCapture.h"
#include "StorageException.h"

FileReplayCapture::FileReplayCapture(const std::string& path) : logFilePath(path) {}

void FileReplayCapture::beginSession(const std::string& courseCode) {
    (void)courseCode;
    stream.open(logFilePath);
    if (!stream.is_open()) {
        throw FileNotFoundException(logFilePath);
    }
    lineNumber = 0;
}
std::string FileReplayCapture::captureNext() {
    std::string line;
    while (std::getline(stream, line)) {
        ++lineNumber;
        if (line.empty()) continue; // skip blank lines silently
        if (line.find(' ') != std::string::npos || line.find('|') != std::string::npos) {
            throw CorruptDataException(logFilePath, "malformed entry at line " + std::to_string(lineNumber));
        }
        return line;
    }
    return "";
}

void FileReplayCapture::endSession() {
    if (stream.is_open()) stream.close();
}
