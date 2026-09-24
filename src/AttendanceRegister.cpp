#include "AttendanceRegister.h"
#include "AttendanceSession.h"
#include "AttendanceRecord.h"
#include "CorrectionRecord.h"
#include "AttendanceException.h"
#include "Course.h"
#include "Student.h"
#include <algorithm>
#include <ostream>
#include <ctime>
#include <iomanip>
#include <sstream>

AttendanceRegister::AttendanceRegister(Course* course) : course(course) {}

AttendanceRegister::~AttendanceRegister() {
    for (AttendanceSession* s : sessions) delete s;
    for (CorrectionRecord* c : corrections) delete c;
}

AttendanceSession* AttendanceRegister::createSession(const std::string& date, int start, int end, const std::string& location) {
    std::string id = course->getCode() + "-S" + std::to_string(sessions.size() + 1);
    sessions.push_back(new AttendanceSession(id, course, date, start, end, location));
    return sessions.back();
}

AttendanceSession* AttendanceRegister::getActiveSession() {
    for (auto it = sessions.rbegin(); it != sessions.rend(); ++it) {
        if ((*it)->isOpen()) return *it;
    }
    return nullptr;
}

AttendanceSession* AttendanceRegister::getSession(const std::string& id) {
    for (AttendanceSession* s : sessions) {
        if (s->getSessionId() == id) return s;
    }
    return nullptr;
}

const std::vector<AttendanceSession*>& AttendanceRegister::getSessions() const { 
    return sessions; }
const std::vector<CorrectionRecord*>& AttendanceRegister::getCorrections() const { 
    return corrections; }
AttendanceSession* AttendanceRegister::restoreSession(const std::string& id,const std::string& date,
                                                        int start,int end,const std::string& location,int duration,bool open){
    if(auto* existing=getSession(id)) return existing;
    auto* s=new AttendanceSession(id,course,date,start,end,location,duration); if(!open)s->close(); sessions.push_back(s); return s;
}
bool AttendanceRegister::restoreRecord(AttendanceSession* s,Student* st,const std::string& timestamp,const std::string& status,const std::string& method,const std::string& recordId){
    if (!s || !st) return false;
    for (auto* r : s->getRecords()) {
        if (r->getRecordId() == recordId) return true;
    }
    s->addRecord(new AttendanceRecord(recordId, s, st, timestamp, status, method));
    ++recordCounter;
    return true;
}
bool AttendanceRegister::restoreCorrection(AttendanceRecord* r,Lecturer* l,const std::string& reason,const std::string& timestamp,const std::string& id){
    if (!r || !l) return false;
    corrections.push_back(new CorrectionRecord(id, r, l, reason, timestamp));
    return true;
}

void AttendanceRegister::recordAttendance(const std::string& sessionId, Student* student,
                                           const std::string& status, const std::string& method) {
    AttendanceSession* session = getSession(sessionId);
    if (!session || !session->isOpen()) {
        throw SessionClosedException(sessionId);
    }
    if (!student) throw NotEnrolledException(sessionId, "<null>");
    const auto& enrolled = course->getEnrolledStudents();
    bool isEnrolled = std::find(enrolled.begin(), enrolled.end(), student) != enrolled.end();
    if (!isEnrolled) {
        throw NotEnrolledException(sessionId, student->getId());
    }
    bool alreadyMarked = std::any_of(session->getRecords().begin(), session->getRecords().end(),
        [&](AttendanceRecord* r) { return r->getStudent() == student; });
    if (alreadyMarked) {
        throw DuplicateAttendanceException(sessionId, student->getId());
    }
    std::string recordId = sessionId + "-R" + std::to_string(++recordCounter);
    std::time_t now = std::time(nullptr); std::tm tmNow{};
#if defined(_WIN32)
    localtime_s(&tmNow, &now);
#else
    localtime_r(&now, &tmNow);
#endif
    std::ostringstream ts; ts << std::put_time(&tmNow, "%Y-%m-%d %H:%M:%S");
    session->addRecord(new AttendanceRecord(recordId, session, student, ts.str(), status, method));
}

void AttendanceRegister::addCorrection(AttendanceRecord* rec, Lecturer* lecturer, const std::string& reason) {
    std::string id = "C" + std::to_string(corrections.size() + 1);
    corrections.push_back(new CorrectionRecord(id, rec, lecturer, reason, rec->getTimestamp()));
}

std::vector<AttendanceRecord*> AttendanceRegister::getRecords(const std::string& sessionId) {
    AttendanceSession* session = getSession(sessionId);
    if (!session) return {};
    return session->getRecords();
}

double AttendanceRegister::attendancePercentage(const Student* student) const {
    if (sessions.empty()) return 0.0;
    int attended = 0;
    for (AttendanceSession* s : sessions) {
        for (AttendanceRecord* r : s->getRecords()) {
            if (r->getStudent() == student && r->getStatus() == "Present") {
                ++attended;
                break;
            }
        }
    }
    return 100.0 * static_cast<double>(attended) / static_cast<double>(sessions.size());
}

void AttendanceRegister::generateReport(const std::string& sessionId, std::ostream& os) const {
    for (AttendanceSession* s : sessions) {
        if (s->getSessionId() == sessionId) {
            os << "Report for " << sessionId << " (" << s->getRecords().size() << " record(s)):\n";
            for (AttendanceRecord* r : s->getRecords()) os << *r << "\n";
            return;
        }
    }
}
