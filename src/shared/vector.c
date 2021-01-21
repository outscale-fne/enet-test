#include <math.h>
#include "vector.h"

#define PI 3.14159265358979323846264338327950288
vector vector_new(double x, double y){
    struct vector v = {.x = x, .y = y};
    return v;
}
vector vector_sum(vector v1, vector v2)
{
    vector output;
    output.x = v1.x + v2.x;
    output.y = v1.y + v2.y;
    return output;
}
vector vector_sub(vector v1, vector v2)
{
    vector output;
    output.x = v1.x - v2.x;
    output.y = v1.y - v2.y;
    return output;
}
vector vector_mul(vector v, double d)
{
    vector output;
    output.x = v.x * d;
    output.y = v.y * d;
    return output;
}
vector vector_div(vector v, double d)
{
    vector output;
    output.x = v.x / d;
    output.y = v.y / d;
    return output;
}
double vector_get_arg(vector *v)
{
    return 90-atan2(v->x, v->y)* (180.0 / PI);
}
double vector_get_module(vector *v)
{
    return sqrt(pow(v->x, 2) + pow(v->y, 2));
}
vector vector_set_arg(vector v, double arg)
{
    arg = arg / (180.0 / PI);
    vector output;
    double module = vector_get_module(&v);
    output.y = sin(arg) * module;
    output.x = cos(arg) * module;
    return output;
}
vector vector_normalize(vector v)
{
    vector output;
    double arg = vector_get_arg(&v);
    output.y = sin(arg);
    output.x = cos(arg);
    return output;
}
void vector_increment(vector * v1, vector v2){
    v1->x = v1->x + v2.x;
    v1->y = v1->y + v2.y;
}
void vector_decrement(vector * v1, vector* v2){
    v1->x = v1->x - v2->x;
    v1->y = v1->y - v2->y;
}
vector vector_zero(){
    vector v = {0,0};
    return v;
}