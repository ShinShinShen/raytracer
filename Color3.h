
#ifndef RAYTRACER_COLOR3_H
#define RAYTRACER_COLOR3_H

#include "Vector3.h"

using Color3 = Vector3;

inline Color3 operator*(float t, const Color3& c) {
    return Color3(t * c.x(), t * c.y(), t * c.z());
}
#endif //RAYTRACER_COLOR3_H