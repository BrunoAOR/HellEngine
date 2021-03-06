#ifndef __H_PHYSICS_FUNCTIONS__
#define __H_PHYSICS_FUNCTIONS__

class GameObject;
#include "MathGeoLib/src/Geometry/LineSegment.h"

GameObject* CalculateRaycast(const LineSegment& lineSegment);

#endif // !__H_PHYSICS_FUNCTIONS__
