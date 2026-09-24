#ifndef LECTURE_BASED_H
#define LECTURE_BASED_H
#include "Course.h"
class LectureBased : public Course {
public:
    using Course::Course;
    double finalGrade(double marks) const override;
    std::string getCourseType() const override;
};
#endif
