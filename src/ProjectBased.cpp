#include "ProjectBased.h"
double ProjectBased::finalGrade(double marks) const { return marks * 0.7 + 30.0 * 0.3; } 
std::string ProjectBased::getCourseType() const { return "ProjectBased"; }
