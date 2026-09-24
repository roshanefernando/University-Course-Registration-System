#ifndef CORRECTION_RECORD_H
#define CORRECTION_RECORD_H
// MEMBER 2 (FR7.3) -- corrections are APPENDED, never edit/delete the original.
#include <string>
#include <iosfwd>

class AttendanceRecord; // pointer only
class Lecturer;          // pointer only

class CorrectionRecord {
private:
    std::string correctionId;
    AttendanceRecord* originalRecord;
    Lecturer* actingLecturer;
    std::string reason;
    std::string timestamp;
public:
    CorrectionRecord(const std::string& id, AttendanceRecord* original, Lecturer* lecturer,
                      const std::string& reason, const std::string& timestamp);
    const std::string& getReason() const;
    AttendanceRecord* getOriginalRecord() const;
    const std::string& getCorrectionId() const;
    Lecturer* getActingLecturer() const;
    const std::string& getTimestamp() const;
    friend std::ostream& operator<<(std::ostream& os, const CorrectionRecord& c);
};
#endif
