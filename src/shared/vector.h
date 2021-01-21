#ifndef VECTOR_h
#define VECTOR_h

typedef struct vector vector;

struct vector
{
    double x, y;
};

vector vector_new(double x, double y);
vector vector_sum(vector v1, vector v2);
vector vector_sub(vector v1, vector v2);
vector vector_mul(vector v, double d);
vector vector_div(vector v, double d);
double vector_get_arg(vector *v);
double vector_get_module(vector *v);
vector vector_set_arg(vector v, double arg);
vector vector_normalize(vector v);
void vector_increment(vector * v1, vector v2);
void vector_decrement(vector * v1, vector* v2);
vector vector_zero();

#endif