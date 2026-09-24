#include "System.h"
#include "User.h"
#include "Student.h"
#include "Lecturer.h"
#include "Administrator.h"
#include "LectureBased.h"
#include "LabBased.h"
#include "ProjectBased.h"
#include "TimeSlot.h"
#include "Timetable.h"
#include "FileReplayCapture.h"
#include "SessionCodeCapture.h"
#include "AttendanceRegister.h"
#include "AttendanceSession.h"
#include "AttendanceRecord.h"
#include "EnrolmentException.h"
#include "AttendanceException.h"
#include "StorageException.h"
#include "FalseLoginAttemptException.h"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <filesystem>
using namespace std;

static void section(const std::string& title) {
    std::cout << "\n=== " << title << " ===\n";
}

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    std::error_code ec;
    std::filesystem::create_directories("data", ec);
    if (ec) {
        std::cerr << "Could not create data directory: " << ec.message() << "\n";
        return 1;
    }

    System uni("data/users.txt", "data/courses.txt", "data/attendance.txt");

    section("STORAGE: startup load (FR5.3 -- no files exist yet, handled gracefully)");
    bool loadedExistingData = false;
    try { uni.loadAll(); loadedExistingData = !uni.getUsers().empty() && !uni.getCourses().empty(); }
    catch (const StorageException& e) { std::cout << "  startup load: " << e.what() << "\n"; }
    if (loadedExistingData) {
        std::cout << "Existing saved data loaded. Skipping demo seeding to avoid duplicates.\n";
        uni.run();
        return 0;
    }

    section("Seeding sample data");
    auto* admin = new Administrator("admin", "adminpass");
    uni.addUser(admin);

    auto* amara = new Lecturer("amara", "lecpass");
    uni.addUser(amara);

    auto* alice = new Student("alice", "pw1");
    auto* bob = new Student("bob", "pw2");
    auto* carol = new Student("carol", "pw3");
    uni.addUser(alice); uni.addUser(bob); uni.addUser(carol);

    std::vector<User*> allUsers(uni.getUsers().begin(), uni.getUsers().end());
    auto* is2301 = new LectureBased("IS2301", "Engineering Mathematics III", 3, 1, amara);
    auto* co2206 = new LectureBased("CO2206", "Operating Systems", 2, 5, amara);
    auto* ee2206 = new LectureBased("EE2206", "Circuit Theory", 2, 5, amara);
    auto* co2201 = new LabBased("CO2201", "DSA", 2, 20, amara);
    auto* co2105 = new ProjectBased("CO2105", "Field Visit", 4, 20, amara);
    co2201->addPrerequisite(co2206);
    for (Course* c : {(Course*)is2301, (Course*)co2206, (Course*)ee2206, (Course*)co2201, (Course*)co2105}) uni.addCourse(c);

    is2301->getTimetable().addTimeSlot(TimeSlot(DayOfWeek::MONDAY, 9 * 60, 10 * 60, "Room A"));
    co2206->getTimetable().addTimeSlot(TimeSlot(DayOfWeek::MONDAY, 9 * 60 + 30, 10 * 60 + 30, "Room B")); // overlaps IS2301
    co2201->getTimetable().addTimeSlot(TimeSlot(DayOfWeek::TUESDAY, 9 * 60, 10 * 60, "Room C"));

    section("FR1.3: polymorphic dashboards via vector<User*>, no if/else on role");
    for (User* u : uni.getUsers()) u->displayDashboard();

    section("FR2.2: three course types, differing grading, called polymorphically");
    for (Course* c : uni.getCourses()) {
        std::cout << "  " << c->getCode() << " (" << c->getCourseType() << ") finalGrade(75) = "
                  << c->finalGrade(75.0) << "\n";
    }

    section("FR3.1: successful registration");
    alice->registerCourse(*is2301);
    std::cout << "  " << *alice << "\n";

    section("Exception 1/4: CourseFullException (IS2301 capacity is 1)");
    try {
        *bob += *is2301;
        std::cout << "  ERROR: expected CourseFullException\n";
    } catch (const CourseFullException& e) {
        std::cout << "  caught as expected: " << e.what() << "\n";
    }

    section("Exception 2/4: TimeClashException (CO2206 overlaps IS2301)");
    try {
        *alice += *co2206;
        std::cout << "  ERROR: expected TimeClashException\n";
    } catch (const TimeClashException& e) {
        std::cout << "  caught as expected: " << e.what() << "\n";
    }

    section("Exception 3/4: PrerequisitesNotMetException, then a successful retry (FR3.4)");
    try {
        *carol += *co2201;
        std::cout << "  ERROR: expected PrerequisitesNotMetException\n";
    } catch (const PrerequisitesNotMetException& e) {
        std::cout << "  caught as expected: " << e.what() << "\n";
    }
    carol->getAcademicRecord().addCompletedCourse(co2206);
    *carol += *co2201;
    std::cout << "  registered after completing CO2206\n";

    section("FR4.3: student's own composite timetable");
    std::cout << carol->viewTimetable();

    section("FR4.4: TimeSlot::operator== clash-detection boundary cases");
    TimeSlot base(DayOfWeek::MONDAY, 9 * 60, 10 * 60, "Room A");
    TimeSlot noClash(DayOfWeek::MONDAY, 11 * 60, 12 * 60, "Room A");
    TimeSlot partialOverlap(DayOfWeek::MONDAY, 9 * 60 + 30, 10 * 60 + 30, "Room A");
    TimeSlot completeOverlap(DayOfWeek::MONDAY, 9 * 60, 10 * 60, "Room A");
    TimeSlot sameBoundary(DayOfWeek::MONDAY, 10 * 60, 11 * 60, "Room A");
    TimeSlot differentDay(DayOfWeek::TUESDAY, 9 * 60, 10 * 60, "Room A");
    std::cout << "  1) No clash:          " << (base == noClash ? "CLASH (WRONG)" : "no clash (correct)") << "\n";
    std::cout << "  2) Partial overlap:   " << (base == partialOverlap ? "clash (correct)" : "NO CLASH (WRONG)") << "\n";
    std::cout << "  3) Complete overlap:  " << (base == completeOverlap ? "clash (correct)" : "NO CLASH (WRONG)") << "\n";
    std::cout << "  4) Same boundary:     " << (base == sameBoundary ? "CLASH (WRONG)" : "no clash (correct: touching, not overlapping)") << "\n";
    std::cout << "  5) Different days:    " << (base == differentDay ? "CLASH (WRONG)" : "no clash (correct)") << "\n";

    section("Attendance: FileReplayCapture (mandatory) with valid + malformed events");
    {
        std::ofstream f("data/checkins.txt");
        f << alice->getId() << "\n";
        f << bob->getId() << "\n";
        f << "bad id\n";          
        f << alice->getId() << "\n"; 
    }
    amara->setActiveCapture(new FileReplayCapture("data/checkins.txt"));
    amara->openAttendanceSession(*is2301);
    AttendanceSession* session1 = is2301->getRegister().getActiveSession();
    {
        auto* capture = dynamic_cast<FileReplayCapture*>(amara->getActiveCapture());
        int exceptionsSeen = 0;
        while (true) {
            std::string id;
            try {
                id = capture->captureNext();
            } catch (const CorruptDataException& e) {
                std::cout << "  caught as expected (malformed line): " << e.what() << "\n";
                ++exceptionsSeen;
                continue;
            }
            if (id.empty()) break;
            try {
                is2301->getRegister().recordAttendance(session1->getSessionId(),
                    dynamic_cast<Student*>(uni.findUser(id)), "Present", "FileReplay");
                std::cout << "  marked present: " << id << "\n";
            } catch (const NotEnrolledException& e) {
                std::cout << "  caught as expected (not enrolled): " << e.what() << "\n";
                ++exceptionsSeen;
            } catch (const DuplicateAttendanceException& e) {
                std::cout << "  caught as expected (duplicate): " << e.what() << "\n";
                ++exceptionsSeen;
            }
        }
        std::cout << "  total exceptions correctly raised from this file: " << exceptionsSeen << " (expected 3)\n";
    }
    amara->closeAttendanceSession(*is2301);

    section("Exception 4/4 context: SessionClosedException (marking after close)");
    try {
        is2301->getRegister().recordAttendance(session1->getSessionId(), alice, "Present", "Manual");
        std::cout << "  ERROR: expected SessionClosedException\n";
    } catch (const SessionClosedException& e) {
        std::cout << "  caught as expected: " << e.what() << "\n";
    }

    section("Attendance: SessionCodeCapture (chosen interactive mechanism, Option A)");
    alice->getAcademicRecord().addCompletedCourse(co2206);
    alice->registerCourse(*co2201);
    amara->setActiveCapture(new SessionCodeCapture());
    amara->openAttendanceSession(*co2201);
    AttendanceSession* session2 = co2201->getRegister().getActiveSession();
    {
        auto* rotating = dynamic_cast<SessionCodeCapture*>(amara->getActiveCapture());
        rotating->submitCode(alice->getId(), rotating->getCurrentCode());
        std::string id = rotating->captureNext();
        if (!id.empty()) {
            co2201->getRegister().recordAttendance(session2->getSessionId(),
                dynamic_cast<Student*>(uni.findUser(id)), "Present", "SessionCode");
            std::cout << "  marked present via SessionCodeCapture: " << id << "\n";
        }
    }
    amara->closeAttendanceSession(*co2201);
    std::cout << "  Same recordAttendance()/Lecturer code path handled two different capture\n"
                 "  mechanisms without any change -- that's FR7.8's runtime interchangeability.\n";

    section("FR7.3: correction record (appended, never edits the original)");
    {
        auto records = is2301->getRegister().getRecords(session1->getSessionId());
        if (!records.empty()) {
            is2301->getRegister().addCorrection(records.front(), amara, "Manual review: confirmed present");
            std::cout << "  correction appended for record " << records.front()->getRecordId() << "\n";
        }
    }

    section("FR7.4: attendance percentage + report");
    std::cout << "  Alice's attendance in IS2301: " << is2301->getRegister().attendancePercentage(alice) << "%\n";
    amara->generateAttendanceReport(*is2301);

    section("FR2.3: lecturer views own enrolment only");
    amara->viewEnrolledStudents(*is2301);

    section("FR6.1: administrator enrollment report");
    admin->generateEnrollmentReport(uni.getCourses(), std::cout);

    section("STORAGE: persistence round-trip (FR5.1/FR5.2/FR7.5)");
    uni.saveAll();
    std::cout << "  saved. Loading into a FRESH System instance...\n";
    {
        System reloaded("data/users.txt", "data/courses.txt", "data/attendance.txt");
        reloaded.loadAll();
        std::cout << "  reloaded " << reloaded.getUsers().size() << " user(s), "
                  << reloaded.getCourses().size() << " course(s)\n";
        for (User* u : reloaded.getUsers()) {
            if (auto* s = dynamic_cast<Student*>(u)) {
                std::cout << "  " << *s << " (enrolment relationships restored)\n";
            }
        }
    }

    section("LOGIN: valid, invalid password, unknown user");
    try {
        User* u = uni.login("alice", "pw1");
        std::cout << "  valid login OK: " << u->getUsername() << "\n";
    } catch (const FalseLoginAttemptException& e) {
        std::cout << "  ERROR: valid login should not throw: " << e.what() << "\n";
    }
    try {
        uni.login("alice", "wrongpassword");
        std::cout << "  ERROR: expected FalseLoginAttemptException\n";
    } catch (const FalseLoginAttemptException& e) {
        std::cout << "  caught as expected (bad password): " << e.what() << "\n";
    }
    try {
        uni.login("nobody", "whatever");
        std::cout << "  ERROR: expected FalseLoginAttemptException\n";
    } catch (const FalseLoginAttemptException& e) {
        std::cout << "  caught as expected (unknown user): " << e.what() << "\n";
    }

    std::cout << "\nAll automated demo scenarios completed without crashing.\n";
    cout << "************************************************************\n";
    cout << "*                                                          *\n";
    cout << "*   *****   *   *  *   *  *********  *     *              *\n";
    cout << "*  *        *   *  **  *      *       *   *               *\n";
    cout << "*   *****    * *   * * *      *        * *                *\n";
    cout << "*        *    *    *  **      *         *                 *\n";
    cout << "*        *    *    *   *      *        * *                *\n";
    cout << "*   *****     *    *   *      *       *   *               *\n";
    cout << "*                                                          *\n";
    cout << "************************************************************\n";
    cout << "        University Course Management System\n";
    cout << "                  Group : SyntX\n\n";
    uni.run();
    return 0;
}
