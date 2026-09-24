#include "vector.h"
#include <stdlib.h>
#include <stdio.h>


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