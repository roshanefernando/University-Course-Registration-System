#include "CorrectionRecord.h"
#include "AttendanceRecord.h"
#include "Lecturer.h"
#include <ostream>

CorrectionRecord::CorrectionRecord(const std::string& id, AttendanceRecord* original, Lecturer* lecturer,
                                    const std::string& reason, const std::string& timestamp)
    : correctionId(id), originalRecord(original), actingLecturer(lecturer), reason(reason), timestamp(timestamp) {}

const std::string& CorrectionRecord::getReason() const { return reason; }
AttendanceRecord* CorrectionRecord::getOriginalRecord() const { return originalRecord; }
const std::string& CorrectionRecord::getCorrectionId() const { return correctionId; }
Lecturer* CorrectionRecord::getActingLecturer() const { return actingLecturer; }
const std::string& CorrectionRecord::getTimestamp() const { return timestamp; }

std::ostream& operator<<(std::ostream& os, const CorrectionRecord& c) {
    os << "  Correction " << c.correctionId << " by " << c.actingLecturer->getUsername()
       << ": " << c.reason << " (original record: " << c.originalRecord->getRecordId() << ")";
    return os;
}
