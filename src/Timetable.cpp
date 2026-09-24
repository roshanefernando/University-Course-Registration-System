#include "Timetable.h"
#include <ostream>
#include <algorithm>

Timetable::Timetable(const Timetable& other) {
    for (TimeSlot* ts : other.timeSlots) {
        timeSlots.push_back(new TimeSlot(*ts)); // deep copy
    }
}

Timetable& Timetable::operator=(const Timetable& other) {
    if (this == &other) return *this;
    for (TimeSlot* ts : timeSlots) delete ts;
    timeSlots.clear();
    for (TimeSlot* ts : other.timeSlots) {
        timeSlots.push_back(new TimeSlot(*ts));
    }
    return *this;
}

Timetable::Timetable(Timetable&& other) noexcept : timeSlots(std::move(other.timeSlots)) {
    other.timeSlots.clear();
}

Timetable& Timetable::operator=(Timetable&& other) noexcept {
    if (this == &other) return *this;
    for (TimeSlot* ts : timeSlots) delete ts;
    timeSlots = std::move(other.timeSlots);
    other.timeSlots.clear();
    return *this;
}

Timetable::~Timetable() {
    for (TimeSlot* ts : timeSlots) delete ts;
}

void Timetable::addTimeSlot(const TimeSlot& ts) {
    timeSlots.push_back(new TimeSlot(ts));
}

bool Timetable::removeTimeSlot(const TimeSlot& ts) {
    auto it = std::find_if(timeSlots.begin(), timeSlots.end(),
        [&](TimeSlot* existing) { return *existing == ts; });
    if (it == timeSlots.end()) return false;
    delete *it;
    timeSlots.erase(it);
    return true;
}

bool Timetable::hasClash(const std::vector<TimeSlot*>& currentSlots) const {
    for (TimeSlot* mine : timeSlots) {
        for (TimeSlot* theirs : currentSlots) {
            if (*mine == *theirs) return true;
        }
    }
    return false;
}

const std::vector<TimeSlot*>& Timetable::getSlots() const { return timeSlots; }

std::ostream& operator<<(std::ostream& os, const Timetable& t) {
    if (t.timeSlots.empty()) {
        os << "  (no classes scheduled)\n";
    }
    for (TimeSlot* ts : t.timeSlots) {
        os << "  " << *ts << "\n";
    }
    return os;
}
