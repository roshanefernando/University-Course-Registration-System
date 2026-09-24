#include "AttendanceSession.h"
#include "AttendanceRecord.h"
AttendanceSession::AttendanceSession(const std::string& id, Course* course, const std::string& date,
                                         int startTime, int endTime, const std::string& location, 
                                         int durationSeconds) : sessionId(id),course(course),date(date),
                                         startTime(startTime),endTime(endTime),location(location),
                                         durationSeconds(durationSeconds>0?durationSeconds:300),
                                         openedAt(std::chrono::steady_clock::now()) {}

AttendanceSession::~AttendanceSession(){ 
    for(auto* r:records) delete r; 
}
void AttendanceSession::open(){
    openedAt=std::chrono::steady_clock::now();open_=true;
}
void AttendanceSession::close(){
    open_=false;
}
bool AttendanceSession::isOpen() const {
     if(!open_) return false; 
     return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now()-openedAt).count()<durationSeconds; 
}
const std::string& AttendanceSession::getSessionId() const{
    return sessionId;
}
Course* AttendanceSession::getCourse() const{
    return course;
}
const std::string& AttendanceSession::getDate() const{
    return date;
}
int AttendanceSession::getStartTime() const{
    return startTime;
}
int AttendanceSession::getEndTime() const{
    return endTime;
}
const std::string& AttendanceSession::getLocation() const{
    return location;
}
int AttendanceSession::getDurationSeconds() const{
    return durationSeconds;
}
void AttendanceSession::addRecord(AttendanceRecord* r){
    if(r)records.push_back(r);
}
const std::vector<AttendanceRecord*>& AttendanceSession::getRecords() const{
    return records;
}
