#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
    int id;
    int dim;
    float data[];
} Vector;

Vector *vector_create(int dim);
void    vector_free(Vector *v);
void    vector_set(Vector *v, int i, float val);
float   vector_get(const Vector *v, int i);
void    vector_fill_random(Vector *v);    // fill with random floats in [0, 1]
void    vector_print(const Vector *v);

float dist_l2_sq(const float *a, const float *b, int dim);
float dist_dot(const float *a, const float *b, int dim);
float dist_cosine(const float *a, const float *b, int dim);

#endif // VECTOR_H
