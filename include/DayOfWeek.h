#ifndef DAY_OF_WEEK_H
#define DAY_OF_WEEK_H
// MEMBER 2.
#include <string>

enum class DayOfWeek { MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURDAY, SUNDAY };

inline const char* dayName(DayOfWeek d) {
    switch (d) {
        case DayOfWeek::MONDAY: return "Monday";
        case DayOfWeek::TUESDAY: return "Tuesday";
        case DayOfWeek::WEDNESDAY: return "Wednesday";
        case DayOfWeek::THURSDAY: return "Thursday";
        case DayOfWeek::FRIDAY: return "Friday";
        case DayOfWeek::SATURDAY: return "Saturday";
        case DayOfWeek::SUNDAY: return "Sunday";
    }
    return "?";
}

// Used by Storage when reloading TimeSlots from a data file.
inline DayOfWeek dayFromString(const std::string& s) {
    if (s == "Monday") return DayOfWeek::MONDAY;
    if (s == "Tuesday") return DayOfWeek::TUESDAY;
    if (s == "Wednesday") return DayOfWeek::WEDNESDAY;
    if (s == "Thursday") return DayOfWeek::THURSDAY;
    if (s == "Friday") return DayOfWeek::FRIDAY;
    if (s == "Saturday") return DayOfWeek::SATURDAY;
    return DayOfWeek::SUNDAY;
}
#endif
