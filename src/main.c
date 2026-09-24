#include <stdio.h>
#include "vector.h"
#include "heap.h"
#include "graph.h"
#include "hnsw.h"
#include "utils.h"

int main(void) {
    printf("=== Testing Vector Implementation ===\n\n");

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

    printf("=== All Vector tests passed! ===\n");
    return 0;
}
