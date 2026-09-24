#ifndef STORAGE_EXCEPTION_H
#define STORAGE_EXCEPTION_H
// MEMBER 3 (FR5.3). Placed here early since Member 2's FileReplayCapture
// throws CorruptDataException on a malformed log line.
#include "Exception.h"

class StorageException : public Exception {
public:
    explicit StorageException(const std::string& msg) : Exception(msg) {}
};
class FileNotFoundException : public StorageException {
public:
    explicit FileNotFoundException(const std::string& path)
        : StorageException(path + ": file not found") {}
};
class CorruptDataException : public StorageException {
public:
    CorruptDataException(const std::string& path, const std::string& detail)
        : StorageException(path + ": corrupt data - " + detail) {}
};
#endif
