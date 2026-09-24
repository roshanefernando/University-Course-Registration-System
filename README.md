# University Course Registration, Timetable & Attendance Management System

Built from the CO2203 spec PDF and the confirmed final UML. All 35 UML
classes are implemented — nothing is a stub, nothing prints "implement
later." Verified: `g++ -std=c++17 -Wall` gives **zero warnings**;
AddressSanitizer + UndefinedBehaviorSanitizer give **zero errors, zero
leaks**.

## Build & run

```
make run
```
This compiles all 20 `.cpp` files and runs the program. It first walks
through an automated demo exercising every functional requirement
(login, enrolment + all 3 violation exceptions, 5 timetable clash
boundary cases, both attendance capture mechanisms, corrections,
persistence round-trip), then drops into an interactive login prompt.

`make clean` removes the binary and the `data/` folder (the program
regenerates `data/users.txt`, `data/courses.txt`, `data/attendance.txt`,
and `data/checkins.txt` on its own — nothing needs to exist beforehand).

### Windows (no `make`)
```
g++ -std=c++17 -Wall -Iinclude src/*.cpp -o UniversitySystem.exe
```
If your terminal doesn't expand `*.cpp` (some `cmd` setups don't), list
every file in `src/` explicitly instead.

## Test login credentials

| Username | Password | Role |
|---|---|---|
| `admin` | `adminpass` | Administrator |
| `amara` | `lecpass` | Lecturer |
| `alice` | `pw1` | Student |
| `bob` | `pw2` | Student |
| `carol` | `pw3` | Student |

## File structure

```
include/    27 headers, one per class (+ DayOfWeek.h for the enum)
src/        20 .cpp files + main.cpp
data/       generated on first run; checkins_sample.txt included as a
            worked example of FileReplayCapture's expected format
Makefile
README.md   (this file)
```

## Interactive menus (after login)

Each role gets a real, working menu — not just a printed list. Options
call the actual domain methods directly, and exceptions are caught right
at this menu layer (never inside `Course`/`AttendanceRegister`/etc.):

**Student:** register a course (lists all courses, prompts for a code),
drop a course, view your personal timetable, view attendance for a
course. Enrolment violations (full course, clash, missing prerequisite)
print a friendly message instead of crashing.

**Lecturer:** view enrolled students, open/mark/close an attendance
session, and generate an attendance report — each restricted to courses
actually assigned to you (FR2.3). When an attendance session is open,
choose **3) Mark attendance**, enter the enrolled student's ID, then
choose `1` for Present or `2` for Absent. A `SessionCodeCapture` is
auto-armed on first use if you haven't set one (FR7.7b's chosen
interactive mechanism).

**Administrator:** add/remove users (prompts for role, username,
password), add/remove courses (prompts for type, code, title, credit,
capacity, lecturer id), assign a lecturer to a course, and generate the
enrollment report.

This is implemented as a new virtual `User::runInteractiveSession()`,
overridden by each subclass — same polymorphic-dispatch pattern as
`displayDashboard()`, so there is still no role-string branching
anywhere, even though the system is now genuinely interactive rather
than just printing a static menu.

## Two logged conflicts between the instruction template and the actual PDF/UML

1. **`Storage::saveAllData`/`loadAllData`** take `vector<User*>&`, not
   `vector<Student*>&` as literally drawn — a `vector<Student*>` cannot
   hold a `Lecturer*` or `Administrator*`, and FR5.1 requires *all*
   users to persist. Signature widened, nothing removed.
2. **`Administrator` has no attributes** (per the UML, explicitly). Every
   method that mutates a master list takes that list as a parameter;
   `System` owns the real storage.

Full reasoning for both is in the class headers at the point of change.

## Necessary additions beyond the UML (all flagged in-code)

- `User::getPassword()` — the UML only exposes `login()`, but FR5.1
  can't persist credentials without a way to read them back out.
- `User(id, username, password, isReload)` — a second constructor on
  every `User` subclass. Without it, reloading from disk would silently
  assign *new* auto-generated IDs, breaking every ID-based relationship
  (login, `Course→Lecturer`, `AttendanceRecord→Student`).
- `Course::setAssignedLecturer()` — needed for
  `Administrator::assignLecturer()` (an explicit UML method) to actually
  do something.
- `Course::getCourseType()` and a matching tag in each save file line —
  needed so `Storage` can reconstruct the correct concrete subclass
  (`LectureBased`/`LabBased`/`ProjectBased`) on reload.
- `getRecordId(Course* const&)` — a free function `Repository<T>`
  requires via ADL for its generic `find()`/`remove()`.
- `User::runInteractiveSession(System&)` — a new pure virtual, overridden
  by each role. Without it, login would succeed but nothing would
  actually be *doable* afterward (the UML's `displayDashboard()` alone
  only prints a menu, it doesn't handle a chosen option).
- `System::getUsersMutable()`/`getCoursesMutable()` — needed so
  Administrator's interactive menu can call its own existing
  `addUser()`/`removeUser()`/etc. methods, which take the target list as
  a mutable reference (per the "Administrator has no attributes"
  resolution above).

## Corrected implementation notes

- `Administrator::updateUser()` now changes the username through `User::setUsername()`.
- `Administrator::editCourse()` now changes capacity and rejects values below the current enrolment count.
- Attendance sessions, records, timestamps, capture methods, and correction records are persisted and restored.
- Saved enrolments are restored directly instead of being re-validated as new registrations.
- Existing saved data is loaded on subsequent runs without reseeding duplicate demo users/courses.
- Attendance session and session-code expiry are checked against wall-clock elapsed time.
- Lecturer attendance opening is restricted to courses assigned to that lecturer.
- Attendance recording validates null/non-enrolled students before dereferencing them.
- Student attendance viewing validates that the student is enrolled in the selected course.
- Course deletion is refused when the course still has enrolled students or is used as a prerequisite, preventing dangling course references.

Login is by **username**, then `User::login()` validates the password.

## File format (persistence)

Pipe-delimited fields, semicolon-delimited sub-lists. See the comment
block at the top of `src/Storage.cpp` for the exact per-line format for
users, courses, and attendance records.

## Windows CMD build/run

From the `project_final` directory: 

```cmd
g++ -std=c++17 -Wall -Iinclude src\*.cpp -o UniversitySystem.exe
UniversitySystem.exe
```

The program creates the `data` directory automatically using C++17
`std::filesystem`; no Unix `mkdir -p` command is required.

After an interactive login session ends, changes are automatically saved.
The final program exit also performs a save.
