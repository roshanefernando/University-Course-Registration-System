#include "Storage.h"
#include "User.h"
#include "Student.h"
#include "Lecturer.h"
#include "Administrator.h"
#include "Course.h"
#include "LectureBased.h"
#include "LabBased.h"
#include "ProjectBased.h"
#include "StorageException.h"
#include "AttendanceRegister.h"
#include "AttendanceSession.h"
#include "AttendanceRecord.h"
#include "CorrectionRecord.h"
#include <fstream>
#include <sstream>

Storage::Storage(const std::string& a,const std::string& b,const std::string& c):studentFilePath(a),courseFilePath(b),attendanceFilePath(c){}
static std::vector<std::string> split(const std::string& line,char d){std::vector<std::string> v;std::stringstream ss(line);std::string x;while(std::getline(ss,x,d))v.push_back(x);return v;}

bool Storage::saveAllData(const std::vector<User*>& users,const std::vector<Course*>& courses){
 std::ofstream uf(studentFilePath); if(!uf)throw FileNotFoundException(studentFilePath);
 for(User* u:users){if(!u)continue; if(auto*s=dynamic_cast<Student*>(u)){std::string a,b;for(auto&x:s->getAcademicRecord().getCompletedCourseCodes()){if(!a.empty())a+=';';a+=x;}for(auto*c:s->getEnrolledCourses()){if(!c)continue;if(!b.empty())b+=';';b+=c->getCode();}uf<<"Student|"<<s->getId()<<"|"<<s->getUsername()<<"|"<<s->getPassword()<<"|"<<a<<"|"<<b<<"\n";}else if(dynamic_cast<Lecturer*>(u))uf<<"Lecturer|"<<u->getId()<<"|"<<u->getUsername()<<"|"<<u->getPassword()<<"\n";else if(dynamic_cast<Administrator*>(u))uf<<"Administrator|"<<u->getId()<<"|"<<u->getUsername()<<"|"<<u->getPassword()<<"\n";}
 std::ofstream cf(courseFilePath);if(!cf)throw FileNotFoundException(courseFilePath);
 for(Course*c:courses){
     if(!c)continue;
     std::string p;
     for(Course*x:c->getPrerequisites()){if(!x)continue;if(!p.empty())p+=';';p+=x->getCode();}
     std::string lid=c->getAssignedLecturer()?c->getAssignedLecturer()->getId():"";
     // Optional field 7 stores the course timetable.  The format is:
     // Day~startMinutes~endMinutes~location;Day~startMinutes~endMinutes~location
     // Existing 7-field course files remain valid because timetable data is optional.
     std::string slots;
     for(TimeSlot* ts:c->getTimetable().getSlots()){
         if(!ts)continue;
         if(!slots.empty())slots+=';';
         std::string location=ts->getLocation();
         for(char& ch:location) if(ch=='|'||ch=='~'||ch==';') ch=' ';
         slots += std::string(dayName(ts->getDay())) + "~" +
                  std::to_string(ts->getStartTime()) + "~" +
                  std::to_string(ts->getEndTime()) + "~" + location;
     }
     cf<<c->getCourseType()<<"|"<<c->getCode()<<"|"<<c->getTitle()<<"|"<<c->getCreditValue()<<"|"<<c->getCapacity()<<"|"<<lid<<"|"<<p<<"|"<<slots<<"\n";
 }
 std::ofstream af(attendanceFilePath);if(!af)throw FileNotFoundException(attendanceFilePath);
 for(Course*c:courses)for(AttendanceSession*s:c->getRegister().getSessions()){af<<"SESSION|"<<c->getCode()<<"|"<<s->getSessionId()<<"|"<<s->getDate()<<"|"<<s->getStartTime()<<"|"<<s->getEndTime()<<"|"<<s->getLocation()<<"|"<<s->getDurationSeconds()<<"|"<<(s->isOpen()?1:0)<<"\n";for(AttendanceRecord*r:s->getRecords())if(r&&r->getStudent())af<<"RECORD|"<<c->getCode()<<"|"<<s->getSessionId()<<"|"<<r->getRecordId()<<"|"<<r->getStudent()->getId()<<"|"<<r->getTimestamp()<<"|"<<r->getStatus()<<"|"<<r->getMethod()<<"\n";for(CorrectionRecord*cr:c->getRegister().getCorrections())if(cr&&cr->getOriginalRecord()&&cr->getActingLecturer()&&cr->getOriginalRecord()->getSession()==s)af<<"CORRECTION|"<<c->getCode()<<"|"<<s->getSessionId()<<"|"<<cr->getCorrectionId()<<"|"<<cr->getOriginalRecord()->getRecordId()<<"|"<<cr->getActingLecturer()->getId()<<"|"<<cr->getTimestamp()<<"|"<<cr->getReason()<<"\n";}
 return true;}

bool Storage::loadAllData(std::vector<User*>& users,std::vector<Course*>& courses){
 for (User* u : users) delete u;
 users.clear();
 for (Course* c : courses) delete c;
 courses.clear();
 std::vector<std::string> ul,cl;std::ifstream uf(studentFilePath);if(!uf)throw FileNotFoundException(studentFilePath);std::string line;
 while(std::getline(uf,line)){if(line.empty())continue;auto f=split(line,'|');if(f.size()<4)throw CorruptDataException(studentFilePath,"invalid user line");ul.push_back(line);if(f[0]=="Student")users.push_back(new Student(f[1],f[2],f[3],true));else if(f[0]=="Lecturer")users.push_back(new Lecturer(f[1],f[2],f[3],true));else if(f[0]=="Administrator")users.push_back(new Administrator(f[1],f[2],f[3],true));else throw CorruptDataException(studentFilePath,"unknown user type");}
 std::ifstream cf(courseFilePath);if(!cf)throw FileNotFoundException(courseFilePath);
 while(std::getline(cf,line)){if(line.empty())continue;auto f=split(line,'|');if(f.size()<6)throw CorruptDataException(courseFilePath,"invalid course line");cl.push_back(line);int cr,cap;try{cr=std::stoi(f[3]);cap=std::stoi(f[4]);}catch(...){throw CorruptDataException(courseFilePath,"invalid numeric course data");}Lecturer*l=nullptr;if(!f[5].empty())for(User*u:users)if(u->getId()==f[5]){l=dynamic_cast<Lecturer*>(u);break;}if(!f[5].empty()&&!l)throw CorruptDataException(courseFilePath,"unknown lecturer");Course*c=nullptr;if(f[0]=="LectureBased")c=new LectureBased(f[1],f[2],cr,cap,l);else if(f[0]=="LabBased")c=new LabBased(f[1],f[2],cr,cap,l);else if(f[0]=="ProjectBased")c=new ProjectBased(f[1],f[2],cr,cap,l);else throw CorruptDataException(courseFilePath,"unknown course type");courses.push_back(c);}
 for(auto&raw:cl){
     auto f=split(raw,'|');
     Course*c=nullptr;
     for(Course*x:courses)if(x->getCode()==f[1]){c=x;break;}
     if(!c)continue;
     if(f.size()>6&&!f[6].empty())
         for(auto&p:split(f[6],';'))
             for(Course*x:courses)if(x->getCode()==p){c->addPrerequisite(x);break;}

     // Timetable field was added as an optional 8th field so older saved
     // course files still load normally.
     if(f.size()>7&&!f[7].empty()){
         for(const auto& slotText:split(f[7],';')){
             auto sf=split(slotText,'~');
             if(sf.size()!=4)throw CorruptDataException(courseFilePath,"invalid timetable slot");
             try{
                 int start=std::stoi(sf[1]);
                 int end=std::stoi(sf[2]);
                 if(start<0||start>=24*60||end<=start||end>24*60)throw std::runtime_error("bad time");
                 c->getTimetable().addTimeSlot(TimeSlot(dayFromString(sf[0]),start,end,sf[3]));
             }catch(...){throw CorruptDataException(courseFilePath,"invalid timetable slot values");}
         }
     }
 }
 for(auto&raw:ul){auto f=split(raw,'|');if(f[0]!="Student"||f.size()<6)continue;Student*s=nullptr;for(User*u:users)if(u->getId()==f[1]){s=dynamic_cast<Student*>(u);break;}if(!s)continue;if(!f[4].empty())for(auto&code:split(f[4],';'))for(Course*c:courses)if(c->getCode()==code){s->getAcademicRecord().addCompletedCourse(c);break;}}
 for(auto&raw:ul){auto f=split(raw,'|');if(f[0]!="Student"||f.size()<6||f[5].empty())continue;Student*s=nullptr;for(User*u:users)if(u->getId()==f[1]){s=dynamic_cast<Student*>(u);break;}if(!s)continue;for(auto&code:split(f[5],';'))for(Course*c:courses)if(c->getCode()==code){c->restoreStudent(*s);break;}s->rebuildTimetable();}
 std::ifstream af(attendanceFilePath);if(!af)return true;std::vector<std::string> al;while(std::getline(af,line))if(!line.empty())al.push_back(line);
 for(auto&raw:al){auto f=split(raw,'|');if(f[0]!="SESSION")continue;if(f.size()!=9)throw CorruptDataException(attendanceFilePath,"bad SESSION");Course*c=nullptr;for(Course*x:courses)if(x->getCode()==f[1]){c=x;break;}if(!c)throw CorruptDataException(attendanceFilePath,"unknown course");try{c->getRegister().restoreSession(f[2],f[3],std::stoi(f[4]),std::stoi(f[5]),f[6],std::stoi(f[7]),f[8]=="1");}catch(...){throw CorruptDataException(attendanceFilePath,"bad SESSION values");}}
 for(auto&raw:al){auto f=split(raw,'|');if(f[0]!="RECORD")continue;if(f.size()!=8)throw CorruptDataException(attendanceFilePath,"bad RECORD");Course*c=nullptr;Student*st=nullptr;for(Course*x:courses)if(x->getCode()==f[1]){c=x;break;}for(User*u:users)if(u->getId()==f[4]){st=dynamic_cast<Student*>(u);break;}if(!c||!st)throw CorruptDataException(attendanceFilePath,"unknown course/student");auto*s=c->getRegister().getSession(f[2]);if(!s)throw CorruptDataException(attendanceFilePath,"unknown session");c->getRegister().restoreRecord(s,st,f[5],f[6],f[7],f[3]);}
 for(auto&raw:al){auto f=split(raw,'|');if(f[0]!="CORRECTION")continue;if(f.size()!=8)throw CorruptDataException(attendanceFilePath,"bad CORRECTION");Course*c=nullptr;Lecturer*l=nullptr;for(Course*x:courses)if(x->getCode()==f[1]){c=x;break;}for(User*u:users)if(u->getId()==f[5]){l=dynamic_cast<Lecturer*>(u);break;}auto*s=c?c->getRegister().getSession(f[2]):nullptr;AttendanceRecord*r=nullptr;if(s)for(auto*x:s->getRecords())if(x->getRecordId()==f[4]){r=x;break;}if(!c||!l||!r)throw CorruptDataException(attendanceFilePath,"invalid correction references");c->getRegister().restoreCorrection(r,l,f[7],f[6],f[3]);}
 return true;}
