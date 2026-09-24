#include "TimeSlot.h"
#include <ostream>

TimeSlot::TimeSlot(DayOfWeek day, int startTime, int endTime, const std::string& location)
    : day(day), startTime(startTime), endTime(endTime), location(location) {}

DayOfWeek TimeSlot::getDay() const { return day; }
int TimeSlot::getStartTime() const { return startTime; }
int TimeSlot::getEndTime() const { return endTime; }
const std::string& TimeSlot::getLocation() const { return location; }

bool TimeSlot::operator==(const TimeSlot& other) const {
    if (day != other.day) return false;
    return startTime < other.endTime && other.startTime < endTime;
}

std::ostream& operator<<(std::ostream& os, const TimeSlot& t) {
    os << dayName(t.day) << " "
       << t.startTime / 60 << ":" << (t.startTime % 60 < 10 ? "0" : "") << t.startTime % 60
       << "-"
       << t.endTime / 60 << ":" << (t.endTime % 60 < 10 ? "0" : "") << t.endTime % 60
       << " @ " << t.location;
    return os;
}
