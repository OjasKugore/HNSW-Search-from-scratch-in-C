# 🗺️ HNSW Learning Roadmap — Build Order & What You'll Learn

> This is your step-by-step learning plan. Every module builds on the last.
> You don't just write code — you understand **why** each piece exists before writing it.
> Follow this in order. Don't skip ahead.

---

## How to Use This File

Each module has:
- 📚 **Learn first** — concepts to understand before you write a single line
- 🛠️ **Build** — exact file and functions to implement
- ✅ **Verify** — how to know you got it right before moving on
- 🔗 **How it connects** — why the next module needs this one

Mark modules as done by changing `[ ]` → `[x]`.

---

## Progress Tracker

```
[x] Module 0 — Setup & Makefile
[x] Module 1 — Vector ADT
[x] Module 2 — Distance Functions
[ ] Module 3 — Binary Heap (Min)
[ ] Module 4 — Binary Heap (Max) & Unified Heap
[ ] Module 5 — Visited Set
[ ] Module 6 — Graph Node (HNSWNode)
[ ] Module 7 — HNSW Index Skeleton
[ ] Module 8 — Random Level Generator
[ ] Module 9 — search_layer()      ← hardest
[ ] Module 10 — select_neighbors()
[ ] Module 11 — hnsw_insert()
[ ] Module 12 — hnsw_search()
[ ] Module 13 — Recall@K Testing
[ ] Module 14 — Serialization (save/load)
[ ] Module 15 — Real Data Demo
[ ] Module 16 — Report
```

---

---

## Module 0 — Project Setup & Makefile

**Time estimate**: 30 minutes

### 📚 Learn First
- What a `Makefile` is and how `gcc` compiles multiple `.c` files together
- The difference between a header file (`.h`) and a source file (`.c`)
- Why you `#ifndef GUARD_H / #define GUARD_H / #endif` in every header
- What `gcc -Wall -Wextra -g` flags mean

### 🛠️ Build
Create the skeleton directory structure with empty files:

```
hnsw/
├── src/
│   ├── main.c
│   ├── vector.c / vector.h
│   ├── heap.c / heap.h
│   ├── graph.c / graph.h
│   ├── hnsw.c / hnsw.h
│   └── utils.c / utils.h
├── tests/
│   └── test_hnsw.c
├── data/
└── Makefile
```

Write a `Makefile` that:
- Compiles all `.c` files in `src/` into a `hnsw_demo` binary
- Has a `test` target that compiles `tests/test_hnsw.c`
- Has a `clean` target

```makefile
CC      = gcc
CFLAGS  = -Wall -Wextra -g -lm
SRCS    = src/main.c src/vector.c src/heap.c src/graph.c src/hnsw.c src/utils.c
TARGET  = hnsw_demo

all: $(TARGET)
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

test: tests/test_hnsw.c $(filter-out src/main.c, $(SRCS))
	$(CC) $(CFLAGS) -o test_hnsw $^

clean:
	rm -f $(TARGET) test_hnsw *.o
```

### ✅ Verify
`make` runs without errors (even though all `.c` files are empty with just `#include` stubs).

### 🔗 How it connects
Every subsequent module adds code to these files. The Makefile recompiles only what changed.

---

---

## Module 1 — Vector ADT

**Time estimate**: 1 hour
**Syllabus**: Unit I — ADT, Dynamic Memory Allocation
**CO**: CO1, CO2

### 📚 Learn First
- What an **Abstract Data Type (ADT)** is: a type defined by its *operations*, not its implementation
- How `malloc` and `free` work in C — when to use them, what happens if you forget `free`
- The difference between stack memory (local variables) and heap memory (`malloc`)
- What `float` precision means and why vectors use `float` not `double` (memory efficiency)

### 🛠️ Build `src/vector.h` and `src/vector.c`

**`vector.h`** — define the struct and declare functions:
```c
#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
    float *data;   // the float array
    int    dim;    // number of dimensions
    int    id;     // unique integer ID
} Vector;

Vector *vector_create(int dim);
void    vector_free(Vector *v);
void    vector_set(Vector *v, int i, float val);
float   vector_get(const Vector *v, int i);
void    vector_fill_random(Vector *v);    // fill with random floats in [0, 1]
void    vector_print(const Vector *v);

#endif
```

**`vector.c`** — implement each function. Key things to get right:
- `vector_create` must `malloc` both the `Vector` struct AND the `data` array inside it
- `vector_free` must `free` the inner `data` first, then the struct itself
- `vector_fill_random` uses `(float)rand() / RAND_MAX`

### ✅ Verify
In `main.c`, write a quick test:
```c
Vector *v = vector_create(4);
vector_fill_random(v);
vector_print(v);   // should print 4 floats
vector_free(v);
// Run: valgrind ./hnsw_demo → 0 memory leaks
```

### 🔗 How it connects
`HNSWNode` and `HNSWIndex` both store `Vector` pointers. Distance functions (Module 2) operate on `Vector.data`.

---

---

## Module 2 — Distance Functions

**Time estimate**: 1 hour
**Syllabus**: Unit I (apply mathematical operations on data)
**CO**: CO1, CO3

### 📚 Learn First
- What **Euclidean distance (L2)** is geometrically — the straight-line distance between two points in N-dimensional space
- Why you use **squared** L2 (`||a - b||²`) instead of `sqrt()` — it's cheaper to compute and comparisons still work correctly (if `a² < b²` then `a < b` for positives)
- What **cosine similarity** means — the angle between two vectors regardless of magnitude. Useful for text embeddings.
- Why distance functions are passed as **function pointers** in C — allows the same HNSW code to work with any metric

### 🛠️ Add to `src/vector.c`

```c
// Squared Euclidean distance: sum of (a[i] - b[i])^2
// O(dim) time
float dist_l2_sq(const float *a, const float *b, int dim);

// Dot product: sum of a[i] * b[i]
// O(dim) time
float dist_dot(const float *a, const float *b, int dim);

// Cosine distance: 1.0 - (a · b) / (|a| * |b|)
// Returns 0 for identical vectors, 2 for opposite vectors
float dist_cosine(const float *a, const float *b, int dim);
```

Add the declarations to `vector.h`.

### ✅ Verify
```c
float a[] = {1.0f, 0.0f};
float b[] = {0.0f, 1.0f};
float c[] = {1.0f, 0.0f};

assert(dist_l2_sq(a, a, 2) == 0.0f);        // same vector → 0
assert(dist_l2_sq(a, c, 2) == 0.0f);        // equal vectors → 0
assert(dist_l2_sq(a, b, 2) == 2.0f);        // (1-0)² + (0-1)² = 2
assert(dist_cosine(a, c, 2) < 0.0001f);     // same direction → ~0
assert(dist_cosine(a, b, 2) > 0.99f);       // 90° angle → ~1.0
```

### 🔗 How it connects
`HNSWIndex` stores a `float (*dist_fn)(const float*, const float*, int)` function pointer. You pass `dist_l2_sq` when creating the index. Every distance computation in `search_layer` goes through this pointer.

---

---

## Module 3 — Min-Heap (Priority Queue)

**Time estimate**: 3–4 hours
**Syllabus**: Unit III — Priority Queue, Unit V — Binary Heap
**CO**: CO1, CO2

### 📚 Learn First
- What a **binary heap** is: a complete binary tree stored in a flat array where the parent is always smaller (min-heap) than its children
- How **array indexing** encodes the tree: parent of `i` is `(i-1)/2`, children are `2i+1` and `2i+2`
- What **sift-up** does: after inserting at the end, bubble the new element up until heap property holds
- What **sift-down** does: after removing the root, replace with last element, then push it down
- Why `push` and `pop` are both **O(log n)** — the tree height is `log n`
- Why `peek` is **O(1)** — root is always at index 0

Draw this on paper first with 10 elements before writing code.

### 🛠️ Build `src/heap.h` and `src/heap.c`

```c
// heap.h
#ifndef HEAP_H
#define HEAP_H

typedef struct {
    int   node_id;
    float distance;
} HeapItem;

typedef struct {
    HeapItem *items;
    int       size;
    int       capacity;
    int       is_min;   // 1 = min-heap, 0 = max-heap
} Heap;

Heap    *heap_create(int capacity, int is_min);
void     heap_push(Heap *h, int node_id, float dist);
HeapItem heap_pop(Heap *h);
HeapItem heap_peek(const Heap *h);
int      heap_is_empty(const Heap *h);
int      heap_size(const Heap *h);
void     heap_free(Heap *h);

#endif
```

Build **only the min-heap** first (`is_min = 1`). Ignore max-heap until Module 4.

The comparison logic for sift operations:
```c
// For min-heap: parent should be SMALLER than child
// swap if parent > child
static int should_swap(Heap *h, int parent, int child) {
    if (h->is_min)
        return h->items[parent].distance > h->items[child].distance;
    else
        return h->items[parent].distance < h->items[child].distance;
}
```

### ✅ Verify
```c
Heap *h = heap_create(16, 1);  // min-heap
heap_push(h, 0, 5.0f);
heap_push(h, 1, 2.0f);
heap_push(h, 2, 8.0f);
heap_push(h, 3, 1.0f);

// Pops should come out in ascending distance order: 1.0, 2.0, 5.0, 8.0
while (!heap_is_empty(h)) {
    HeapItem item = heap_pop(h);
    printf("id=%d dist=%.1f\n", item.node_id, item.distance);
}
// Assert each dist is >= previous
heap_free(h);
```

### 🔗 How it connects
`search_layer` (Module 9) uses a min-heap as the "candidates" set. You must fully understand the min-heap before adding the max-heap variant.

---

---

## Module 4 — Max-Heap & Unified Heap

**Time estimate**: 1 hour
**Syllabus**: Unit III, Unit V
**CO**: CO1, CO3

### 📚 Learn First
- Why HNSW needs **two heaps simultaneously** during search:
  - **Min-heap (candidates)**: always gives you the *closest unvisited* node to explore next
  - **Max-heap (results)**: always gives you the *farthest* result, so you know when to stop and what to evict if over capacity
- The `should_swap` function you wrote in Module 3 is the only thing that changes between min and max

### 🛠️ Build
No new files — just extend `heap.c`. The `is_min` flag you already have handles both modes.

Write a helper to **convert a max-heap to a sorted array** (for extracting top-K results):
```c
// Pops all items from h into out_array (sorted ascending by distance)
void heap_drain_sorted(Heap *h, HeapItem *out_array, int *out_count);
```

### ✅ Verify
```c
Heap *h = heap_create(4, 0);  // max-heap with capacity 4
// Push 10 items
for (int i = 0; i < 10; i++)
    heap_push(h, i, (float)(rand() % 100));

// Pop should come in descending distance order
float prev = 1e9f;
while (!heap_is_empty(h)) {
    HeapItem item = heap_pop(h);
    assert(item.distance <= prev);
    prev = item.distance;
}
```

Also test: push 10 items into a max-heap of capacity 4, keeping only the 4 smallest (evict max when size > capacity). This is exactly what `search_layer` does.

### 🔗 How it connects
With both heap modes working, you have everything needed to implement `search_layer`. This is the most foundational data structure in the whole project.

---

---

## Module 5 — Visited Set

**Time estimate**: 45 minutes
**Syllabus**: Unit I — memory management, Unit III — efficient data access
**CO**: CO2, CO3

### 📚 Learn First
- Why you need to track visited nodes during graph traversal (avoid infinite loops / re-processing)
- Naive approach: `memset(visited, 0, N)` before every query → **O(N) per query**. Bad when N = 1,000,000.
- **Generation counter trick**: instead of clearing the array, just increment a counter. A node is "visited in the current query" if `gen[node_id] == current_gen`. This makes reset **O(1)**.

### 🛠️ Add to `src/utils.c` and `src/utils.h`

```c
typedef struct {
    int *gen;         // gen[node_id] = generation when last marked
    int  current;     // current generation
    int  capacity;
} VisitedSet;

VisitedSet *visited_create(int capacity);
void        visited_mark(VisitedSet *vs, int node_id);
int         visited_check(const VisitedSet *vs, int node_id);  // 1 = visited
void        visited_reset(VisitedSet *vs);   // just does vs->current++
void        visited_free(VisitedSet *vs);
```

### ✅ Verify
```c
VisitedSet *vs = visited_create(100);
visited_mark(vs, 5);
visited_mark(vs, 42);
assert(visited_check(vs, 5)  == 1);
assert(visited_check(vs, 42) == 1);
assert(visited_check(vs, 7)  == 0);

visited_reset(vs);  // O(1)
assert(visited_check(vs, 5)  == 0);  // cleared without memset!
assert(visited_check(vs, 42) == 0);
visited_free(vs);
```

### 🔗 How it connects
`search_layer` creates a `VisitedSet` and resets it between queries. Without this, you'd revisit nodes in cycles and the search would never terminate.

---

---

## Module 6 — Graph Node (HNSWNode)

**Time estimate**: 2 hours
**Syllabus**: Unit V — Graph, Adjacency List representation
**CO**: CO1, CO2, CO4

### 📚 Learn First
- Adjacency list vs adjacency matrix — understand WHY adjacency list is chosen here (sparse graph, variable degree per node)
- Why HNSW needs **per-layer** adjacency lists — a node exists in multiple layers with different neighbour sets
- How a **2D dynamic array** works in C: `int **neighbors` is an array of `int*` pointers, where `neighbors[layer]` is a dynamically allocated array of neighbour IDs
- What **bidirectional edges** mean: when A connects to B, B must also connect to A

### 🛠️ Build `src/graph.h` and `src/graph.c`

```c
// graph.h
#ifndef GRAPH_H
#define GRAPH_H

typedef struct {
    int    id;
    int    level;               // max layer this node appears in
    int  **neighbors;           // neighbors[layer] = array of neighbor IDs
    int   *n_count;             // n_count[layer] = current number of neighbors
    int   *n_cap;               // n_cap[layer]   = allocated size of neighbors[layer]
} HNSWNode;

// M  = max neighbors at upper layers
// M0 = max neighbors at layer 0 (typically 2*M)
HNSWNode *node_create(int id, int level, int M, int M0);
void      node_free(HNSWNode *node);

// Add neighbor_id to node's list at the given layer
void      node_add_neighbor(HNSWNode *node, int layer, int neighbor_id);

// Remove neighbor_id from node's list at the given layer
void      node_remove_neighbor(HNSWNode *node, int layer, int neighbor_id);

// Print the adjacency list for debugging
void      node_print(const HNSWNode *node);

#endif
```

Key implementation detail in `node_create`:
```c
// Allocate arrays for each layer from 0 to level
node->neighbors = malloc(sizeof(int*) * (level + 1));
node->n_count   = calloc(level + 1, sizeof(int));
node->n_cap     = malloc(sizeof(int) * (level + 1));
for (int l = 0; l <= level; l++) {
    int cap = (l == 0) ? M0 : M;
    node->neighbors[l] = malloc(sizeof(int) * cap);
    node->n_cap[l] = cap;
}
```

### ✅ Verify
```c
HNSWNode *n = node_create(0, 2, 4, 8);  // node 0, max layer 2, M=4, M0=8
node_add_neighbor(n, 0, 3);
node_add_neighbor(n, 0, 7);
node_add_neighbor(n, 1, 2);
assert(n->n_count[0] == 2);
assert(n->n_count[1] == 1);
assert(n->n_count[2] == 0);
node_remove_neighbor(n, 0, 3);
assert(n->n_count[0] == 1);
node_free(n);
// valgrind: 0 leaks
```

### 🔗 How it connects
`HNSWIndex` (Module 7) holds an array of `HNSWNode*`. `hnsw_insert` (Module 11) calls `node_add_neighbor` to wire the graph. `search_layer` (Module 9) reads `node->neighbors[layer]` to find candidates to explore.

---

---

## Module 7 — HNSW Index Skeleton

**Time estimate**: 1.5 hours
**Syllabus**: Unit I — ADT design
**CO**: CO1, CO2, CO4

### 📚 Learn First
- How to design a top-level ADT that owns all its resources
- The concept of a **function pointer** in C: `float (*dist_fn)(const float*, const float*, int)` lets you swap distance metrics without changing the search code
- Why the index holds a **parallel array** of `Vector` alongside `HNSWNode*` — vectors are data, nodes are graph topology

### 🛠️ Build `src/hnsw.h` and the skeleton of `src/hnsw.c`

```c
// hnsw.h
#ifndef HNSW_H
#define HNSW_H

#include "graph.h"
#include "vector.h"
#include "heap.h"
#include "utils.h"

typedef struct {
    HNSWNode **nodes;          // array of node pointers, indexed by ID
    int        node_count;
    int        node_capacity;

    float     *raw_vectors;    // flat array: raw_vectors + id*dim = vector for node id
                               // more cache-friendly than storing Vector structs

    int        entry_point;    // ID of the entry point (top-layer node)
    int        max_level;      // current highest layer

    int        dim;
    int        M;
    int        M0;             // = 2 * M
    int        ef_construction;
    int        ef_search;
    float      ml;             // = 1.0f / logf((float)M)

    float    (*dist_fn)(const float*, const float*, int);
} HNSWIndex;

typedef struct {
    int   id;
    float distance;
} KNNResult;

HNSWIndex *hnsw_create(int dim, int M, int ef_construction, int ef_search,
                        float (*dist_fn)(const float*, const float*, int));
void       hnsw_free(HNSWIndex *idx);
void       hnsw_print_stats(const HNSWIndex *idx);

// (Implemented in later modules)
void       hnsw_insert(HNSWIndex *idx, const float *vec, int id);
KNNResult *hnsw_search(HNSWIndex *idx, const float *query, int k);
void       hnsw_save(const HNSWIndex *idx, const char *path);
HNSWIndex *hnsw_load(const char *path);

#endif
```

Implement `hnsw_create` and `hnsw_free` fully. Leave `hnsw_insert` and `hnsw_search` as stubs.

### ✅ Verify
```c
HNSWIndex *idx = hnsw_create(128, 16, 200, 50, dist_l2_sq);
hnsw_print_stats(idx);
// Should print: dim=128, M=16, M0=32, ml=0.361, entry_point=-1 (empty), nodes=0
hnsw_free(idx);
// valgrind: 0 leaks
```

### 🔗 How it connects
All remaining modules fill in the functions declared here. The index is the container — everything lives inside it.

---

---

## Module 8 — Random Level Generator

**Time estimate**: 45 minutes
**Syllabus**: Unit V (probability in data structure design)
**CO**: CO3

### 📚 Learn First
- What a **geometric distribution** is: the number of coin flips until you get heads. In HNSW: keep going up layers while `rand() < 1/M`.
- Why this gives the right sparsity: P(level ≥ l) = (1/M)^l. With M=16, about 1/16 of nodes reach layer 1, 1/256 reach layer 2, etc.
- Why the formula is `-log(random) * ml`: this is the continuous equivalent of the geometric distribution.

### 🛠️ Add to `src/utils.c`

```c
// Returns a random level in [0, max_cap]
// ml = 1.0f / logf((float)M)
int random_level(float ml, int max_cap) {
    double r = (double)rand() / ((double)RAND_MAX + 1.0);
    if (r == 0.0) r = 1e-10;
    int level = (int)(-log(r) * ml);
    return (level < max_cap) ? level : max_cap;
}
```

Also add a seeded RNG initialiser:
```c
void utils_seed(unsigned int seed);  // wraps srand()
```

### ✅ Verify
```c
// Generate 10000 levels with M=16 (ml ≈ 0.361)
// Print a histogram — should look geometric:
// Level 0: ~9375 nodes  (93.75%)
// Level 1: ~586 nodes   (5.86%)
// Level 2: ~37 nodes    (0.37%)
// Level 3: ~2 nodes     (0.02%)
int counts[10] = {0};
for (int i = 0; i < 10000; i++) {
    int l = random_level(1.0f / logf(16.0f), 8);
    counts[l]++;
}
for (int l = 0; l < 10; l++)
    printf("Level %d: %d\n", l, counts[l]);
```

### 🔗 How it connects
`hnsw_insert` calls `random_level` once per inserted vector to decide which layers to add the node to.

---

---

## Module 9 — `search_layer()` ← The Core Algorithm

**Time estimate**: 4–6 hours
**Syllabus**: Unit III (Priority Queue), Unit V (Graph traversal)
**CO**: CO3, CO4

> ⚠️ **This is the hardest module.** Take your time. Draw diagrams. Test thoroughly before moving on.

### 📚 Learn First
- **Greedy search**: always move to the closest unvisited neighbour, without backtracking
- **Beam search**: instead of tracking 1 candidate (greedy), track `ef` candidates — a balance between speed and recall
- **Why two heaps**: the min-heap ensures you always explore the most promising candidate first. The max-heap tells you the worst result you'd keep, so you know when a candidate is too far to be useful.
- Read the pseudocode in the HNSW paper (Algorithm 2) before implementing. Understand each line.

### 🛠️ Add to `src/hnsw.c` (private function)

```c
// Returns a max-heap of up to `ef` nearest candidates found at layer `lc`
// Caller must heap_free() the returned heap
static Heap *search_layer(
    HNSWIndex   *idx,
    const float *query,      // the query vector
    int          ep_id,      // entry point node ID for this layer
    int          ef,         // beam width
    int          lc,         // layer number
    VisitedSet  *vs          // visited set (caller provides, reset between calls)
);
```

**Step-by-step implementation:**

```
1. float ep_dist = idx->dist_fn(query, vector_of(ep_id), idx->dim)
2. visited_mark(vs, ep_id)
3. candidates = heap_create(ef * 2, MIN_HEAP)
4. results    = heap_create(ef * 2, MAX_HEAP)
5. heap_push(candidates, ep_id, ep_dist)
6. heap_push(results,    ep_id, ep_dist)

7. while candidates is not empty:
   a. c = heap_pop(candidates)          // closest unexplored node
   b. f_dist = heap_peek(results).distance  // farthest in results
   c. if c.distance > f_dist:
        break  // can't improve — all remaining candidates are farther than worst result

   d. for each neighbor `e_id` of c in layer lc:
        if visited_check(vs, e_id): continue
        visited_mark(vs, e_id)
        e_dist = idx->dist_fn(query, vector_of(e_id), idx->dim)
        f_dist = heap_peek(results).distance

        if e_dist < f_dist OR heap_size(results) < ef:
            heap_push(candidates, e_id, e_dist)
            heap_push(results,    e_id, e_dist)
            if heap_size(results) > ef:
                heap_pop(results)   // evict farthest

8. heap_free(candidates)
9. return results   // caller gets the max-heap of ef nearest
```

### ✅ Verify
Insert 5 vectors manually by directly wiring `node_add_neighbor` (skip the full insert for now). Then call `search_layer` and verify it returns the right nodes in the right order.

Also test edge cases:
- `ef = 1` should return exactly 1 result (greedy search)
- Query == an indexed vector should return that vector at distance 0

### 🔗 How it connects
`hnsw_insert` calls `search_layer` once per layer during insertion. `hnsw_search` calls it once at layer 0 for the final result. This function is called more than any other in the codebase.

---

---

## Module 10 — `select_neighbors()`

**Time estimate**: 2 hours
**Syllabus**: Unit V (graph construction strategies)
**CO**: CO3, CO4

### 📚 Learn First
- After `search_layer` returns candidates, you need to pick **M** of them to actually connect as neighbours
- **Simple selection**: just take the M closest. Fast, but can create "clumps" where all your neighbours point in the same direction.
- **Heuristic selection** (Algorithm 4 in the paper): ensures diversity by checking that each added neighbour is closer to the query than it is to any already-chosen neighbour. Produces better-connected graphs.

### 🛠️ Add to `src/hnsw.c` (private functions)

```c
// Simple: take top-M from candidates heap
// candidates is a max-heap; we want the M smallest distances
static void select_neighbors_simple(
    Heap *candidates,    // max-heap from search_layer
    int   M,
    int  *out_ids,
    int  *out_count
);

// Heuristic: prefer diverse neighbours
static void select_neighbors_heuristic(
    HNSWIndex   *idx,
    const float *query,
    Heap        *candidates,
    int          M,
    int          layer,
    int         *out_ids,
    int         *out_count
);
```

**Implement `select_neighbors_simple` first.** The heuristic version is optional but improves recall by ~5%.

For simple: drain the max-heap into a temp array (this reverses order to ascending), then take the first M items.

### ✅ Verify
```c
Heap *h = heap_create(10, 0);  // max-heap
heap_push(h, 0, 9.0f);
heap_push(h, 1, 3.0f);
heap_push(h, 2, 7.0f);
heap_push(h, 3, 1.0f);
heap_push(h, 4, 5.0f);

int ids[3]; int count;
select_neighbors_simple(h, 3, ids, &count);
// Should select ids for distances 1.0, 3.0, 5.0
assert(count == 3);
// Verify the 3 selected are the 3 smallest distances
```

### 🔗 How it connects
`hnsw_insert` calls this immediately after `search_layer` to decide which nodes to connect the new node to.

---

---

## Module 11 — `hnsw_insert()`

**Time estimate**: 3–4 hours
**Syllabus**: Unit I (dynamic memory), Unit V (graph construction)
**CO**: CO4, CO5

### 📚 Learn First
- Why insertion must be **bidirectional**: adding edge A→B without B→A breaks the graph's navigability
- Why you must **prune neighbour lists** after adding a reverse edge: if B already has M0 neighbours and you add one more, you must evict the worst one
- The two-phase descent: Phase A (fast, ef=1) to find a good starting point, Phase B (careful, ef=ef_construction) to wire the graph properly
- What happens when the first node is inserted (empty index case)

### 🛠️ Implement `hnsw_insert` in `src/hnsw.c`

```c
void hnsw_insert(HNSWIndex *idx, const float *vec, int id) {
    int L = random_level(idx->ml, 16);   // assign level to new node
    // ... see README for full step-by-step
}
```

Key things to get right:
1. Handle the empty index case first (first insert sets entry_point)
2. Phase A uses `ef=1` and only updates the entry point variable, doesn't wire any edges
3. Phase B calls `search_layer` + `select_neighbors` + bidirectional `node_add_neighbor` + pruning
4. After wiring, if a neighbour's count exceeds M (or M0), call `select_neighbors` on their list too and prune

### ✅ Verify
```c
HNSWIndex *idx = hnsw_create(2, 4, 20, 10, dist_l2_sq);

// Insert 20 2D vectors in a grid
for (int i = 0; i < 20; i++) {
    float v[2] = {(float)(i % 5), (float)(i / 5)};
    hnsw_insert(idx, v, i);
}

hnsw_print_stats(idx);
// Expected:
//   node_count = 20
//   max_level >= 1
//   every node at layer 0 has at least 1 neighbor
//   no neighbor exceeds M0 connections at layer 0

hnsw_free(idx);
// valgrind: 0 leaks
```

### 🔗 How it connects
This is what builds the searchable index. Without a correct insert, the search will find wrong results.

---

---

## Module 12 — `hnsw_search()`

**Time estimate**: 1.5 hours
**Syllabus**: Unit V (Top-K problem, Graph traversal)
**CO**: CO3, CO5

### 📚 Learn First
- How `hnsw_search` is just two phases of `search_layer` calls — Phase A for fast positioning, Phase B for thorough search at layer 0
- Why `ef = max(ef_search, k)` — you must search at least `k` candidates to guarantee returning `k` results
- How to extract top-K from the max-heap (drain and reverse)

### 🛠️ Implement `hnsw_search` in `src/hnsw.c`

```c
KNNResult *hnsw_search(HNSWIndex *idx, const float *query, int k) {
    // Phase A: greedy descent from max_level down to layer 1
    // Phase B: wide beam search at layer 0 with ef = max(ef_search, k)
    // Extract top-K from result heap
    // Return malloc'd KNNResult[k] sorted ascending by distance
}
```

Also write a brute-force search for testing:
```c
// O(N * dim) — used only for recall testing
KNNResult *brute_force_search(HNSWIndex *idx, const float *query, int k);
```

### ✅ Verify
```c
// Insert 100 random 4D vectors
// For each of 10 query vectors:
//   Get brute_force top-5
//   Get hnsw top-5
//   Print both side by side
// They should be identical or nearly identical for small N
```

### 🔗 How it connects
This completes the core engine. Modules 13–16 are about measuring quality and making it presentable.

---

---

## Module 13 — Recall@K Testing

**Time estimate**: 2 hours
**Syllabus**: CO3 — Analyse and evaluate
**CO**: CO3, CO5

### 📚 Learn First
- What **Recall@K** means: of the true K nearest neighbours, what fraction does HNSW find?
  - `recall@K = |HNSW results ∩ true results| / K`
  - A value of 0.95 means HNSW found 9.5 out of 10 true neighbours on average
- What a **recall-vs-speed tradeoff** curve looks like: as you increase `ef_search`, recall goes up and speed goes down

### 🛠️ Build `tests/test_hnsw.c`

Write three test functions:

```c
void test_heap();           // pushes/pops, asserts sorted order
void test_distance();       // sanity checks for dist_l2_sq
void test_recall_at_k();    // builds index, runs HNSW vs brute force, prints recall
void test_latency();        // times HNSW query vs brute force, prints speedup table
```

For `test_recall_at_k`:
```c
// 1. Insert 1000 random 64-dim vectors
// 2. For 100 random query vectors:
//    a. Get brute force top-10
//    b. Get HNSW top-10
//    c. Count intersection
// 3. Print: "Recall@10: X.XX (avg over 100 queries)"
// 4. Assert recall > 0.85
```

### ✅ Verify
Run `make test && ./test_hnsw`. Should print something like:
```
[PASS] test_heap
[PASS] test_distance
Recall@10: 0.94 (avg over 100 queries, N=1000, dim=64, M=16, ef=50)
[PASS] test_recall_at_k
Brute force: 12.3ms | HNSW: 0.4ms | Speedup: 30x
[PASS] test_latency
```

### 🔗 How it connects
This is your evidence for the report. The recall number and speedup table go directly into the results section of your submission.

---

---

## Module 14 — Serialization (Save / Load)

**Time estimate**: 2 hours
**Syllabus**: Unit I (memory layout, data organisation)
**CO**: CO2, CO4

### 📚 Learn First
- How binary file I/O works in C: `fwrite`, `fread`, `fopen` with `"wb"` / `"rb"` modes
- What **serialization** means: converting in-memory pointer-based structs into a flat byte sequence that can be written to disk and reconstructed later
- Why you can't just `fwrite(idx, sizeof(HNSWIndex), 1, f)` — because `idx->nodes` is a pointer, not inline data

### 🛠️ Implement in `src/hnsw.c`

```c
void       hnsw_save(const HNSWIndex *idx, const char *path);
HNSWIndex *hnsw_load(const char *path);
```

Save format (binary):
```
[int dim] [int M] [int M0] [int ef_construction] [int ef_search]
[int node_count] [int entry_point] [int max_level]
[float raw_vectors: node_count * dim floats]
For each node:
  [int id] [int level]
  For each layer 0..level:
    [int n_count[layer]]
    [int neighbors[layer][0..n_count-1]]
```

### ✅ Verify
```c
// Build index with 500 vectors
// Save to "test.idx"
// Load back
// Search both for same query
// Results must be identical
```

### 🔗 How it connects
Makes your demo practical — build the index once, save it, load and query without rebuilding.

---

---

## Module 15 — Real Data Demo

**Time estimate**: 2 hours
**Syllabus**: CO5 — Real-world interdisciplinary application
**CO**: CO5

### 📚 Learn First
- What **word embeddings** are: pre-trained vectors where words with similar meanings are close together in vector space
- Download **GloVe 50-dimensional** embeddings (free, ~160MB): https://nlp.stanford.edu/data/glove.6B.zip
- Each line in the file: `word 0.1234 -0.5678 ... (50 floats)`

### 🛠️ Add to `src/main.c`

```c
// 1. Parse glove.6B.50d.txt — load first 50,000 word vectors
// 2. Build HNSW index (dim=50, M=16, ef=200)
// 3. Interactive loop:
//    - User types a word
//    - Find the vector for that word
//    - Run hnsw_search, return top-10 most similar words
//    - Print: word | distance
```

Example output:
```
Query: king
1. queen       | 0.142
2. prince      | 0.198
3. throne      | 0.231
4. emperor     | 0.267
5. royal       | 0.289
```

### ✅ Verify
Run a few queries. Results should be semantically sensible.

---

---

## Module 16 — Report

**Time estimate**: 3–4 hours (writing)
**Syllabus**: Part B submission requirement
**CO**: CO5

### 🛠️ Write `report/report.pdf` (2–4 pages)

Structure:

```
1. Abstract (5 lines)
   — What HNSW is, what you built, what results you achieved

2. Introduction (0.5 page)
   — The problem: brute-force search is O(N), not viable at scale
   — HNSW as the solution: O(log N) approximate nearest neighbour

3. Data Structures Used (0.5 page)
   — Table: structure | syllabus unit | how used in HNSW

4. Algorithm Explanation (1 page)
   — Diagram of the multi-layer graph
   — search_layer pseudocode with explanation
   — hnsw_insert pseudocode with explanation

5. Results (0.5 page)
   — Recall@10 achieved
   — Latency table: HNSW vs brute force at N=1K, 10K, 100K
   — Demo: word similarity results

6. Conclusion (0.25 page)
   — What you learned, what real systems use HNSW for

7. References
   — Malkov & Yashunin 2018, CLRS, Horowitz et al.
```

---

---

## ⏱️ Total Time Estimate

| Phase | Modules | Estimated Hours |
|-------|---------|----------------|
| Setup | 0 | 0.5h |
| Data structures | 1–7 | ~10h |
| Core algorithms | 8–12 | ~15h |
| Testing & validation | 13 | ~2h |
| Polish & demo | 14–15 | ~4h |
| Report | 16 | ~4h |
| **Total** | | **~35 hours** |

Spread over 3–4 weeks comfortably, or 2 intensive weeks.

---

## 🧠 Key Learning Outcomes by Module

| After Module | You will understand... |
|---|---|
| 0–2 | How to design ADTs in C, why distance functions use function pointers |
| 3–4 | How binary heaps work, sift-up/sift-down, why O(log n) |
| 5 | The generation counter trick — a classic O(1) vs O(N) optimization |
| 6–7 | How graphs are stored as adjacency lists, per-layer graph design |
| 8 | How probability distributions shape data structure behaviour |
| 9 | Greedy beam search — the most important graph traversal technique in modern ML |
| 10–11 | Graph construction, bidirectional wiring, neighbour pruning |
| 12–13 | How to measure approximate algorithm quality (recall@K) |
| 14 | Binary serialization of complex pointer-based data structures |
| 15–16 | How data structures power real NLP/AI applications |
