#include <stdio.h>
#include "vector.h"
#include "heap.h"
#include "graph.h"
#include "hnsw.h"
#include "utils.h"

#include <assert.h>
#include <math.h>

int main(void) {
    printf("=== Testing Module 1: Vector ADT ===\n\n");

    // 1. Create a vector of dimension 4
    printf("--- Test 1: Create, Set, and Get ---\n");
    Vector *v1 = vector_create(4);
    v1->id = 101;
    vector_set(v1, 0, 1.25f);
    vector_set(v1, 1, 2.50f);
    vector_set(v1, 2, 3.75f);
    vector_set(v1, 3, 5.00f);

    vector_print(v1);
    printf("Value at index 2: %f (expected: 3.750000)\n\n", vector_get(v1, 2));

    // 2. Create and fill a vector with random values
    printf("--- Test 2: Random Vector ---\n");
    Vector *v2 = vector_create(5);
    v2->id = 102;
    vector_fill_random(v2);
    vector_print(v2);
    printf("\n");

    // 3. Free memory
    printf("--- Test 3: Free Memory ---\n");
    vector_free(v1);
    vector_free(v2);
    printf("All vectors successfully freed!\n\n");

    printf("=== Testing Module 2: Distance Functions ===\n\n");

    float a[] = {1.0f, 0.0f};
    float b[] = {0.0f, 1.0f};
    float c[] = {1.0f, 0.0f};
    float opposite[] = {-1.0f, 0.0f};
    float zero[] = {0.0f, 0.0f};

    // --- Squared Euclidean Distance ---
    printf("--- Test 4: Squared Euclidean Distance (dist_l2_sq) ---\n");
    float d_same = dist_l2_sq(a, a, 2);
    float d_equal = dist_l2_sq(a, c, 2);
    float d_diff = dist_l2_sq(a, b, 2);
    printf("dist_l2_sq(a, a) = %f (expected 0.000000)\n", d_same);
    printf("dist_l2_sq(a, c) = %f (expected 0.000000)\n", d_equal);
    printf("dist_l2_sq(a, b) = %f (expected 2.000000)\n", d_diff);

    assert(d_same == 0.0f);
    assert(d_equal == 0.0f);
    assert(fabsf(d_diff - 2.0f) < 1e-6f);

    // 3D Euclidean distance test: (4-1)^2 + (6-2)^2 + (3-3)^2 = 9 + 16 = 25
    float p1[] = {1.0f, 2.0f, 3.0f};
    float p2[] = {4.0f, 6.0f, 3.0f};
    float d_3d = dist_l2_sq(p1, p2, 3);
    printf("dist_l2_sq(3D)   = %f (expected 25.000000)\n\n", d_3d);
    assert(fabsf(d_3d - 25.0f) < 1e-6f);

    // --- Dot Product ---
    printf("--- Test 5: Dot Product (dist_dot) ---\n");
    float dot_ident = dist_dot(a, c, 2);
    float dot_ortho = dist_dot(a, b, 2);
    printf("dist_dot(a, c)   = %f (expected 1.000000)\n", dot_ident);
    printf("dist_dot(a, b)   = %f (expected 0.000000)\n\n", dot_ortho);
    assert(fabsf(dot_ident - 1.0f) < 1e-6f);
    assert(fabsf(dot_ortho - 0.0f) < 1e-6f);

    // --- Cosine Distance ---
    printf("--- Test 6: Cosine Distance (dist_cosine) ---\n");
    float cos_same = dist_cosine(a, c, 2);
    float cos_ortho = dist_cosine(a, b, 2);
    float cos_opp = dist_cosine(a, opposite, 2);
    float cos_zero = dist_cosine(a, zero, 2);
    printf("dist_cosine(same dir) = %f (expected ~0.0)\n", cos_same);
    printf("dist_cosine(90 deg)   = %f (expected ~1.0)\n", cos_ortho);
    printf("dist_cosine(opposite) = %f (expected ~2.0)\n", cos_opp);
    printf("dist_cosine(zero-vec) = %f (expected 1.0)\n\n", cos_zero);

    assert(fabsf(cos_same - 0.0f) < 1e-5f);
    assert(fabsf(cos_ortho - 1.0f) < 1e-5f);
    assert(fabsf(cos_opp - 2.0f) < 1e-5f);
    assert(cos_zero == 1.0f);

    printf("=== All Module 1 & Module 2 tests passed! ===\n");
    return 0;
}
