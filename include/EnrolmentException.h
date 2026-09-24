#ifndef ENROLMENT_EXCEPTION_H
#define ENROLMENT_EXCEPTION_H
// MEMBER 1 -- thrown by Member 2's Course::enrolStudent() (FR3.2/FR3.3).
#include "Exception.h"

class EnrolmentException : public Exception {
public:
    explicit EnrolmentException(const std::string& msg) : Exception(msg) {}
};
class TimeClashException : public EnrolmentException {
public:
    explicit TimeClashException(const std::string& courseCode)
        : EnrolmentException(courseCode + ": clashes with an existing enrolled course") {}
};
class CourseFullException : public EnrolmentException {
public:
    explicit CourseFullException(const std::string& courseCode)
        : EnrolmentException(courseCode + ": course is at full capacity") {}
};
class PrerequisitesNotMetException : public EnrolmentException {
public:
    explicit PrerequisitesNotMetException(const std::string& courseCode)
        : EnrolmentException(courseCode + ": prerequisite(s) not met") {}
};
#endif
