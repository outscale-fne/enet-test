
#include "transform.h"
#include "vector.h"

Transform transform_new(double x, double y, double rotation)
{
    Transform t;
    t.position.x = x;
    t.position.y = y;
    t.rotation = rotation;
    t.angular_velocity = 0;
    t.velocity = vector_zero();
    return t;
}
void transform_update(Transform *t, double dt)
{
    t->rotation -= dt * t->angular_velocity;
    t->velocity = vector_set_arg(t->velocity, t->rotation);
    vector increment = vector_mul(t->velocity, dt);
    vector_increment(&t->position, increment);
}