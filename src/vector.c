#include "vector.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define SQUARE(x) ((x) * (x))

Vector *vector_create(int dimensions){
    //dimensions is a dynamic member,just allotting for vector will only allocate memory for id and dim
    Vector *vector = malloc(sizeof(Vector) + sizeof(float) * dimensions); 
    if (vector == NULL)
    {
        printf("Memory allocation for Vector failed.\n");
        exit(EXIT_FAILURE);
    }
    vector -> dim = dimensions;
    vector -> id = -1;
    return vector;
}

void vector_free(Vector *v){
    free(v);
}

void vector_set(Vector *v, int i, float val){
    if (i < 0 || i >= v->dim){
        printf("Given index is out of bounds.\n");
        exit(EXIT_FAILURE);
    }
    v->data[i] = val;
}

float vector_get(const Vector *v, int i){
    if (i < 0 || i >= v->dim){
        printf("Given index is out of bounds.\n");
        exit(EXIT_FAILURE);
    }
    return v->data[i];
}

void vector_fill_random(Vector *v){
    for (int i = 0; i < v->dim; i++){
        v->data[i] = (float) rand() / (float)RAND_MAX;
    }
    printf("Vector initialized with random values.\n");
}

void vector_print(const Vector *v){
    printf("Vector ID: %d\n", v->id);
    printf("Vector Dimensions: %d\n", v->dim);
    printf("Printing Vector...\n");
    printf("[");
    for (int i =0; i < v-> dim; i++){
        printf("%f, ", v->data[i]);
    }
    printf("]\n");
}

float dist_l2_sq(const float *a, const float *b, int dim){
    float dist = 0;
    for (int i =0; i < dim; i++){
        dist += SQUARE(a[i] - b[i]);
    }
    return dist;
}

float dist_dot(const float *a, const float *b, int dim){
    float dot = 0;
    for (int i = 0; i < dim; i++){
        dot += a[i] * b[i];
    }
    return dot;
}

float dist_cosine(const float *a, const float *b, int dim){
    float norm_a = 0.0f;
    float norm_b = 0.0f;
    for (int i = 0; i <dim; i++){
        norm_a += SQUARE(a[i]);
        norm_b += SQUARE((b[i]));
    }
    norm_a = sqrtf(norm_a);
    norm_b = sqrtf(norm_b);
    if (norm_a  == 0.0f || norm_b == 0.0f){
        return 1.0f;
    }
    return (1 - (dist_dot(a, b, dim) / (norm_a * norm_b)));
}