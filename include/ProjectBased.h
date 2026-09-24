#ifndef PROJECT_BASED_H
#define PROJECT_BASED_H
#include "Course.h"
class ProjectBased : public Course {
public:
    using Course::Course;
    double finalGrade(double marks) const override;
    std::string getCourseType() const override;
};
#endif
