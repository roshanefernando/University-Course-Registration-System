#ifndef TIME_SLOT_H
#define TIME_SLOT_H
// MEMBER 2 (FR4.1/FR4.4).
#include "DayOfWeek.h"
#include <string>
#include <iosfwd>

class TimeSlot {
private:
    DayOfWeek day;
    int startTime; // minutes since midnight
    int endTime;
    std::string location;
public:
    TimeSlot(DayOfWeek day, int startTime, int endTime, const std::string& location);
    DayOfWeek getDay() const;
    int getStartTime() const;
    int getEndTime() const;
    const std::string& getLocation() const;

    // Clash rule (stated explicitly per spec): same day AND
    // start1 < end2 AND start2 < end1 (half-open interval overlap).
    // Touching boundaries (one ends exactly when the other starts) are
    // NOT a clash under this rule.
    bool operator==(const TimeSlot& other) const;

    friend std::ostream& operator<<(std::ostream& os, const TimeSlot& t);
};
#endif
