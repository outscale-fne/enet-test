#ifndef TRANSFORM_h
#define TRANSFORM_h

#include "vector.h"
typedef struct Transform Transform;
struct Transform
{
    vector position, velocity;
    double rotation, angular_velocity;
};
Transform transform_new(double x, double y, double rotation);
void transform_update(Transform * t, double dt);
#endif