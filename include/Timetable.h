#ifndef TIMETABLE_H
#define TIMETABLE_H
// MEMBER 2 (FR4.1). Composition: Timetable owns its TimeSlots via pointer.
#include "TimeSlot.h"
#include <vector>
#include <iosfwd>

class Timetable {
private:
    std::vector<TimeSlot*> timeSlots;
public:
    Timetable() = default;
    // Rule of Five #2 (deep copy) -- distinct style from Lecturer's
    // deleted-copy example: this class demonstrates the OTHER approach,
    // a fully correct deep copy of a dynamically-owned resource.
    Timetable(const Timetable& other);
    Timetable& operator=(const Timetable& other);
    Timetable(Timetable&& other) noexcept;
    Timetable& operator=(Timetable&& other) noexcept;
    ~Timetable();

    void addTimeSlot(const TimeSlot& ts);
    bool removeTimeSlot(const TimeSlot& ts);
    bool hasClash(const std::vector<TimeSlot*>& currentSlots) const;
    const std::vector<TimeSlot*>& getSlots() const;

    friend std::ostream& operator<<(std::ostream& os, const Timetable& t);
};
#endif
