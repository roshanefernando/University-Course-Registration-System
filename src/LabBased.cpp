#include "LabBased.h"
double LabBased::finalGrade(double marks) const { return marks * 0.4 + 60.0 * 0.6; }
std::string LabBased::getCourseType() const { return "LabBased"; }
