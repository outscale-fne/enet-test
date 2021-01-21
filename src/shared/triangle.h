#ifndef triangle_h
#define triangle_h

#include "vector.h"
#include "transform.h"

typedef struct triangle triangle;
struct triangle {
    vector p[3];
    // Points with offset and rotation.
    vector _p[3];
};

void triangle_update(triangle *t, Transform * tr);
struct triangle triangle_new(vector p1, vector p2, vector p3);
#endif