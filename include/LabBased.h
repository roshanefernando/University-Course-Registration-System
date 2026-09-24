#ifndef LAB_BASED_H
#define LAB_BASED_H
#include "Course.h"
class LabBased : public Course {
public:
    using Course::Course;
    double finalGrade(double marks) const override;
    std::string getCourseType() const override;
};
#endif
