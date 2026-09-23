# 🔍 HNSW Vector Search Engine in C

> **Course**: CS233AI — Data Structures and Applications (Semester III)
> **Category**: Experiential Learning Project (Part B) — 40 Marks
> **Program**: B.E. Computer Science and Engineering

---

## 📖 Table of Contents

1. [What is This Project?](#what-is-this-project)
2. [What is HNSW?](#what-is-hnsw)
3. [Why This Qualifies as Experiential Learning](#why-this-qualifies-as-experiential-learning)
4. [Syllabus Alignment](#syllabus-alignment)
5. [Course Outcome Mapping](#course-outcome-mapping)
6. [Project Structure](#project-structure)
7. [C Components — Data Structures](#c-components--data-structures)
8. [C Components — Algorithms](#c-components--algorithms)
9. [Key Constants and Hyperparameters](#key-constants-and-hyperparameters)
10. [Implementation Phases](#implementation-phases)
11. [How to Build and Run](#how-to-build-and-run)
12. [Validation and Testing](#validation-and-testing)
13. [Submission Checklist](#submission-checklist)
14. [References](#references)

---

## What is This Project?

This project is a **mini vector search engine** built entirely from scratch in C, using an algorithm called **HNSW (Hierarchical Navigable Small World graphs)**.

In plain terms: given a large collection of vectors (arrays of floating-point numbers representing data like text, images, or audio), this engine can find the **K most similar vectors** to any query vector — extremely fast, without scanning the entire dataset.

This is the same algorithm that powers production vector databases like **Pinecone, Weaviate, Qdrant, and pgvector**, which are used in AI applications, semantic search, and recommendation systems.

Every single component — the heap, the graph, the search algorithm — is written from scratch in C using concepts directly covered in the CS233AI syllabus.

---

## What is HNSW?

HNSW stands for **Hierarchical Navigable Small World graphs**. It was introduced by Malkov & Yashunin (2018) and is the state-of-the-art algorithm for Approximate Nearest Neighbour (ANN) search.

### Core Idea

HNSW organises all vectors into a **multi-layer proximity graph**:

```
Layer 2:  o ————————————————— o          (sparse, long-range, few nodes)
               \             /
Layer 1:  o — o — o ————— o — o          (medium density)
           \   |   \     /   /
Layer 0:  o-o-o-o-o-o-o-o-o-o-o-o        (dense, ALL vectors here)
```

- **Upper layers** are sparse "express lanes" — they let you jump across the dataset in large steps.
- **Lower layers** are dense "local lanes" — they do precise neighbourhood exploration.
- **Layer 0** contains every vector with the most connections.

### How Search Works (in plain English)

1. Start at the single **entry point** node at the top layer.
2. **Greedily walk** towards the query vector (always move to the closest neighbour).
3. Once you can't get closer at this layer, **drop down** one layer.
4. Repeat until you reach Layer 0.
5. At Layer 0, do a wider **beam search** (explore `ef_search` candidates) and return the top-K closest vectors found.

This gives you approximate nearest neighbours in **O(log N)** time instead of O(N) for brute force.

---

## Why This Qualifies as Experiential Learning

The syllabus (Part B) states:

> *"Students are encouraged to implement an advanced data structure **not covered in the syllabus**, such as a Red-Black Tree, Fibonacci Heap, Segment Tree, Skip List, Suffix Tree, or **advanced graph algorithms**, and demonstrate one of its applications."*

HNSW satisfies every requirement:

| Requirement | How HNSW Satisfies It |
|-------------|----------------------|
| Advanced data structure not in syllabus | HNSW multi-layer proximity graph |
| Built on syllabus-covered structures | Uses heaps, linked lists, graphs, ADTs |
| Demonstrates a real-world application | Vector similarity search (used in AI/NLP/image retrieval) |
| Working code + brief report | Full C codebase + report in `report/` |

---

## Syllabus Alignment

HNSW is unique because it **uses every unit of the syllabus**:

| Unit | Topic | How It's Used in HNSW |
|------|--------|-----------------------|
| **Unit I** — Linked Lists & ADTs | ADT, Dynamic Memory, Linked Lists | `Vector` and `HNSWIndex` are ADTs. Per-layer neighbor lists are dynamically allocated arrays (like linked lists). |
| **Unit II** — Stacks | Stack, iterative problem-solving | Iterative graph traversal using an explicit stack (avoids recursive stack overflow during deep graph walks) |
| **Unit III** — Queues & Priority Queues | Priority Queue, bounded buffers | **The most used structure in HNSW.** Min-heap for results, Max-heap for candidates in every `search_layer` call |
| **Unit IV** — Trees & BST | Tree hierarchy, traversal | The HNSW layer structure is a tree of graphs. Greedy descent mirrors preorder tree traversal |
| **Unit V** — Heaps, Graphs, Advanced Trees | Binary Heap, Graph (Adjacency List), Top-K | Binary Heap implements the priority queue. HNSW is a graph stored as adjacency lists. `hnsw_search()` solves Top-K problem |

---

## Course Outcome Mapping

| CO | Statement | How This Project Covers It |
|----|-----------|---------------------------|
| **CO1** | Demonstrate understanding of fundamental linear and non-linear data structures | Implements heap (linear), adjacency list graph (non-linear), ADT interfaces from scratch |
| **CO2** | Apply data structures and memory management to organise data efficiently | Manual `malloc`/`free` for per-layer neighbor arrays, dynamic heap growth, visited-set generation trick |
| **CO3** | Analyse computational problems and select appropriate data structures | Justifies min-heap vs max-heap choice, analyses O(M log N) insert and O(log N) query, measures Recall@K |
| **CO4** | Develop solutions using good programming practices | Modular `.c/.h` files, separated concerns, Makefile build, no memory leaks (verified with Valgrind) |
| **CO5** | Develop and demonstrate solutions for real-world problems through experiential learning | Demonstrated on real word embeddings — perform semantic nearest-neighbour search as the application |

---

## Project Structure

```
hnsw/
│
├── src/                        # All C source files
│   ├── main.c                  # Entry point: CLI to load vectors, build index, query
│   ├── vector.c                # Vector ADT: allocation, distance functions
│   ├── vector.h
│   ├── heap.c                  # Min-heap and Max-heap (binary heap)
│   ├── heap.h
│   ├── graph.c                 # HNSWNode: per-layer adjacency lists
│   ├── graph.h
│   ├── hnsw.c                  # Core: insert, search_layer, select_neighbors, search
│   ├── hnsw.h                  # Public HNSW API
│   └── utils.c                 # Random level generator, timing helpers
│
├── tests/
│   └── test_hnsw.c             # Unit tests: heap, distance, recall@K
│
├── data/
│   └── sample_vectors.txt      # Sample float vectors for testing
│
├── report/
│   └── report.pdf              # Submission report (Part B requirement)
│
├── Makefile                    # Build system
└── README.md                   # This file
```

---

## C Components — Data Structures

These are the exact structs and types you will define and implement in C.

---

### 1. `Vector` — `vector.h`

The fundamental unit. Wraps a float array with dimension and ID metadata.

```c
typedef struct {
    float  *data;       // The actual float array, e.g. data[128] for 128-dim
    int     dim;        // Dimensionality (e.g. 128, 384, 768)
    int     id;         // Unique integer ID
    char   *label;      // Optional string label (e.g. "cat.jpg")
} Vector;
```

**Functions you implement:**
```c
Vector *vector_create(int dim);
void    vector_free(Vector *v);
void    vector_copy(Vector *dst, const Vector *src);
void    vector_print(const Vector *v);
```

---

### 2. Distance Functions — `vector.c`

HNSW is metric-agnostic. You implement at least two distance functions:

```c
// Squared Euclidean distance — most common, avoids sqrt for speed
float dist_l2_sq(const float *a, const float *b, int dim);

// Cosine distance = 1 - cosine_similarity
float dist_cosine(const float *a, const float *b, int dim);

// Dot product / inner product (for normalized embeddings)
float dist_inner_product(const float *a, const float *b, int dim);
```

> **Note**: You store distances, not similarities. Smaller distance = more similar.

---

### 3. `Heap` (Priority Queue) — `heap.h`

The **most critical data structure** in this entire project. Used in every search and insert call. You need BOTH a min-heap and a max-heap because:
- **Max-heap** = candidate set (tracks the farthest known candidate, prune if over capacity)
- **Min-heap** = result set (efficiently extract the closest K results at the end)

```c
#define HEAP_MIN 0
#define HEAP_MAX 1

typedef struct {
    int   node_id;
    float distance;
} HeapItem;

typedef struct {
    HeapItem *items;     // Array of heap items
    int       size;      // Current number of items
    int       capacity;  // Max capacity (reallocate if exceeded)
    int       type;      // HEAP_MIN or HEAP_MAX
} Heap;
```

**Functions you implement:**
```c
Heap    *heap_create(int initial_capacity, int type);
void     heap_push(Heap *h, int node_id, float dist);
HeapItem heap_pop(Heap *h);            // Remove and return root
HeapItem heap_peek(const Heap *h);     // View root without removing
void     heap_free(Heap *h);
int      heap_is_empty(const Heap *h);
int      heap_size(const Heap *h);
```

**Internal helpers (private):**
```c
static void sift_up(Heap *h, int idx);    // After push
static void sift_down(Heap *h, int idx);  // After pop
static void swap_items(Heap *h, int i, int j);
```

> **Implementation detail**: Use a standard binary heap array. For a node at index `i`, its children are at `2i+1` and `2i+2`. Its parent is at `(i-1)/2`. For min-heap: parent ≤ children. For max-heap: parent ≥ children.

---

### 4. `VisitedSet` — `utils.c`

During graph traversal, you need to mark nodes as visited. A naive approach clears a bool array on every query — O(N) per query. Instead, use a **generation counter** for O(1) reset:

```c
typedef struct {
    int *gen;           // gen[node_id] = the generation when it was last visited
    int  current_gen;   // Current generation counter
    int  capacity;
} VisitedSet;
```

**Functions you implement:**
```c
VisitedSet *visited_create(int capacity);
void        visited_mark(VisitedSet *vs, int node_id);
int         visited_check(const VisitedSet *vs, int node_id); // 1 = visited
void        visited_reset(VisitedSet *vs);    // Just increments current_gen — O(1)!
void        visited_free(VisitedSet *vs);
```

---

### 5. `HNSWNode` — `graph.h`

A single node in the HNSW graph. It exists in multiple layers and holds a separate neighbour list for each layer it appears in.

```c
typedef struct {
    int    id;                  // Same as index in HNSWIndex.nodes[]
    int    level;               // Max layer this node appears in (0-indexed)
    int  **neighbors;           // neighbors[layer] = int array of neighbor IDs
    int   *neighbor_count;      // neighbors[layer] has neighbor_count[layer] entries
    int   *neighbor_capacity;   // Allocated size of neighbors[layer]
} HNSWNode;
```

**Functions you implement:**
```c
HNSWNode *node_create(int id, int level, int M, int M0);
void      node_free(HNSWNode *node);
void      node_add_neighbor(HNSWNode *node, int layer, int neighbor_id);
void      node_remove_neighbor(HNSWNode *node, int layer, int neighbor_id);
```

> **Why per-layer?** At layer 0 each node has up to `M0 = 2×M` neighbours. At upper layers, up to `M` neighbours. The per-layer lists keep memory compact.

---

### 6. `HNSWIndex` — `hnsw.h`

The top-level ADT. The "database" that holds everything.

```c
typedef struct {
    /* Graph */
    HNSWNode **nodes;           // Array of pointers, indexed by node ID
    int        node_count;      // Number of vectors currently indexed
    int        node_capacity;   // Allocated capacity of nodes[]

    /* Vectors (parallel array to nodes[]) */
    Vector    *vectors;         // vectors[i] stores the float data for node i

    /* Entry point (the top of the hierarchy) */
    int        entry_point_id;  // ID of the node at the highest layer
    int        max_level;       // Current highest layer in the graph

    /* Hyperparameters */
    int        dim;             // Vector dimensionality
    int        M;               // Max neighbours per layer (upper layers)
    int        M0;              // Max neighbours at layer 0 (= 2 * M)
    int        ef_construction; // Beam width during index build
    int        ef_search;       // Beam width during query
    float      ml;              // Level multiplier = 1.0f / logf((float)M)

    /* Pluggable distance function */
    float    (*dist_fn)(const float*, const float*, int);
} HNSWIndex;
```

**Functions you implement:**
```c
HNSWIndex *hnsw_create(int dim, int M, int ef_construction, int ef_search,
                        float (*dist_fn)(const float*, const float*, int));
void       hnsw_free(HNSWIndex *idx);
void       hnsw_insert(HNSWIndex *idx, const float *vec, int id);
KNNResult *hnsw_search(HNSWIndex *idx, const float *query, int k);
void       hnsw_save(const HNSWIndex *idx, const char *filepath);
HNSWIndex *hnsw_load(const char *filepath);
void       hnsw_print_stats(const HNSWIndex *idx);
```

---

### 7. `KNNResult` — `hnsw.h`

The output of a search query — an array of the K closest nodes with their distances.

```c
typedef struct {
    int   id;
    float distance;
} KNNResult;

// hnsw_search returns a malloc'd array of KNNResult[k], sorted closest-first
// Caller is responsible for free()-ing it
```

---

## C Components — Algorithms

These are the core functions you will write inside `hnsw.c`. They are the intellectual heart of the project.

---

### Algorithm 1: `random_level()`

Every new node is assigned a random maximum layer. This creates the sparse-to-dense hierarchy. Uses a geometric distribution so that most nodes land at layer 0, fewer at layer 1, even fewer at layer 2, etc.

```c
// Returns a random level in [0, max_allowed]
// P(level >= l) = (1/M)^l
static int random_level(float ml) {
    double r = (double)rand() / (double)RAND_MAX;
    // If r is close to 0, level is high. If r is close to 1, level is 0.
    int level = (int)(-log(r) * ml);
    return level;
}
```

**What you'll do**: Implement this, then print a histogram of 10,000 generated levels to verify the geometric distribution looks correct.

---

### Algorithm 2: `search_layer()` ← The Core

This is the greedy beam search within a single layer. It is called many times during both insert and query. Getting this right is the hardest and most important part of the project.

```c
// Searches layer `lc` starting from `entry_point_id`.
// Returns a max-heap of `ef` nearest candidates found.
static Heap *search_layer(
    HNSWIndex   *idx,
    const float *query_vec,
    int          entry_point_id,
    int          ef,              // beam width
    int          lc,              // layer number
    VisitedSet  *visited
);
```

**What it does (step by step):**

```
1. Mark entry_point as visited
2. Init candidates (min-heap) with {entry_point, dist(query, entry_point)}
3. Init results   (max-heap) with {entry_point, dist(query, entry_point)}

4. While candidates is not empty:
   a. Pop the CLOSEST candidate `c`
   b. Let `f` = the FARTHEST item in results (peek at max-heap top)
   c. If dist(query, c) > dist(query, f):
      → We can't improve results anymore → BREAK
   d. For each neighbour `e` of `c` at layer lc:
      → If `e` already visited → skip
      → Mark `e` visited
      → Compute dist(query, e)
      → If dist(query,e) < dist(query, f)  OR  results.size < ef:
         → Push `e` into candidates (min-heap)
         → Push `e` into results   (max-heap)
         → If results.size > ef: pop from results (remove farthest)

5. Return results (max-heap of ef nearest found)
```

---

### Algorithm 3: `select_neighbors_simple()`

When connecting a new node to its neighbours, pick the top-M closest from the candidate set.

```c
static void select_neighbors_simple(
    Heap *candidates,   // max-heap of candidates
    int   M,            // max neighbours to select
    int  *out_ids,      // output: selected neighbour IDs
    int  *out_count     // output: how many selected
);
```

**What you'll do**: Pop from the heap until you have M items or the heap is empty. Store the IDs in `out_ids`.

---

### Algorithm 4: `select_neighbors_heuristic()` (Optional, improves quality)

Instead of just picking the M closest, this heuristic ensures **diversity**: it prefers candidates that are closer to the query AND not already "covered" by a closer candidate. This improves graph navigability significantly.

```c
static void select_neighbors_heuristic(
    HNSWIndex *idx,
    const float *query_vec,
    Heap      *candidates,
    int        M,
    int        layer,
    int       *out_ids,
    int       *out_count
);
```

---

### Algorithm 5: `hnsw_insert()` — Building the Index

This is how a new vector gets added to the HNSW graph. It ties everything together.

```c
void hnsw_insert(HNSWIndex *idx, const float *vec, int id);
```

**What it does (step by step):**

```
1. Assign a random level L to the new node using random_level()
2. Create the HNSWNode with level L and allocate its neighbor lists
3. Store the vector in idx->vectors[id]

4. If the index is empty (first insert):
   → Set entry_point = new node, max_level = L, return

5. ep = idx->entry_point_id  (start search from top)

6. Phase A — Greedy descent from max_level down to L+1:
   For layer = max_level down to L+1:
     → Run search_layer(query=vec, ep=ep, ef=1, layer)
     → ep = the single closest node found (update entry point for next layer)

7. Phase B — Careful insertion from L down to 0:
   For layer = min(L, max_level) down to 0:
     → Run search_layer(query=vec, ep=ep, ef=ef_construction, layer)
     → W = result candidates from search_layer
     → neighbours = select_neighbors(W, M or M0 if layer==0)
     → Add bidirectional edges: new_node ↔ each neighbour at this layer
     → For each neighbour: if its neighbor_count[layer] > M (or M0):
         → Prune its neighbor list using select_neighbors() again
     → ep = closest node from W (for next layer down)

8. If L > max_level:
   → idx->entry_point_id = new node ID
   → idx->max_level = L
```

---

### Algorithm 6: `hnsw_search()` — Querying

```c
KNNResult *hnsw_search(HNSWIndex *idx, const float *query, int k);
```

**What it does:**

```
1. ep = idx->entry_point_id

2. Phase A — Greedy descent from max_level down to layer 1:
   For layer = max_level down to 1:
     → search_layer(query, ep, ef=1, layer)
     → ep = closest node found

3. Phase B — Wide beam search at layer 0:
   → W = search_layer(query, ep, ef=max(ef_search, k), layer=0)

4. Extract top-K from W (the K items with smallest distance)
5. Sort them by distance (ascending)
6. Return as KNNResult array
```

---

## Key Constants and Hyperparameters

| Parameter | Default | What It Controls |
|-----------|---------|-----------------|
| `M` | 16 | Max neighbours per node per upper layer. Higher = better recall, more memory and slower build |
| `M0` | 32 | Max neighbours per node at layer 0. Should always be `2 × M` |
| `ef_construction` | 200 | How wide the beam search is during index build. Higher = better graph quality, slower build time |
| `ef_search` | 50 | How wide the beam search is during query. Higher = better recall, slower query. Can be tuned at query time without rebuilding |
| `ml` | `1/ln(M)` ≈ 0.36 | Controls the geometric distribution of node levels. Automatically derived from M, don't change manually |

**Rule of thumb**: Start with M=16, ef_construction=200, ef_search=50. If recall is low, increase ef_search. If build is too slow, decrease ef_construction.

---

## Implementation Phases

### Phase 1 — Foundation (EL Phase 1, 20 Marks)

> Goal: A working index that can insert vectors and has a correct graph structure.

| Step | File | What You'll Do |
|------|------|---------------|
| 1 | `vector.c/.h` | Implement Vector struct, `vector_create`, `vector_free`, `dist_l2_sq`, `dist_cosine` |
| 2 | `heap.c/.h` | Implement Heap struct, `heap_create`, `heap_push`, `heap_pop`, `sift_up`, `sift_down` for both min and max modes |
| 3 | `utils.c` | Implement `VisitedSet` with generation counter, `random_level()` |
| 4 | `graph.c/.h` | Implement `HNSWNode` with per-layer neighbour lists, `node_create`, `node_add_neighbor` |
| 5 | `hnsw.c/.h` | Implement `HNSWIndex`, `hnsw_create`, `hnsw_free` |
| 6 | `hnsw.c` | Implement `search_layer()` |
| 7 | `hnsw.c` | Implement `select_neighbors_simple()` |
| 8 | `hnsw.c` | Implement `hnsw_insert()` |
| 9 | `main.c` | Insert 500 random 4-dim vectors, print graph stats, verify node counts per layer |

**Phase 1 deliverable**: The program inserts N vectors and prints the graph structure (node count per layer, entry point level, avg neighbours).

---

### Phase 2 — Query + Validation + Demo (EL Phase 2, 20 Marks)

> Goal: A working search engine with measured recall, compared against brute force.

| Step | File | What You'll Do |
|------|------|---------------|
| 10 | `hnsw.c` | Implement `hnsw_search()` |
| 11 | `tests/test_hnsw.c` | Write brute-force KNN, compare with HNSW, compute Recall@K |
| 12 | `hnsw.c` | Implement `hnsw_save()` / `hnsw_load()` for index persistence |
| 13 | `main.c` | Load real word embeddings (GloVe 50d), build index, demo semantic search |
| 14 | `report/` | Write 2–4 page report: what HNSW is, what syllabus structures were used, results table |

**Phase 2 deliverable**: Demo program that takes a query word/vector, returns the K most similar vectors from the dataset, with Recall@K ≥ 90% printed in output.

---

## How to Build and Run

```bash
# Build everything
make

# Run the demo (inserts 1000 random vectors, runs 10 queries)
./hnsw_demo

# Run with custom parameters
./hnsw_demo --dim 128 --M 16 --ef-construction 200 --ef-search 50 --vectors data/sample_vectors.txt

# Run tests (includes Recall@K benchmark)
make test
./test_hnsw

# Check for memory leaks
valgrind --leak-check=full ./hnsw_demo
```

**Makefile targets:**

```makefile
all:        builds hnsw_demo
test:       builds test_hnsw
clean:      removes all .o and binaries
valgrind:   runs hnsw_demo under valgrind
```

---

## Validation and Testing

### 1. Heap Correctness Test
```
Push 1000 random (node_id, distance) pairs into a min-heap.
Pop all 1000 items.
Assert: each popped distance ≤ previous popped distance.
→ PASS means heap is correct.
```

### 2. Distance Sanity Test
```
For any vector v: dist_l2_sq(v, v) == 0.0f          (zero self-distance)
For any a, b:     dist_l2_sq(a, b) == dist_l2_sq(b, a) (symmetry)
For a, b, c:      dist(a,c) ≤ dist(a,b) + dist(b,c)    (triangle inequality)
```

### 3. Insert Sanity Test
```
After inserting N vectors:
  - idx->node_count == N
  - idx->max_level > 0 (for N > 50)
  - Every node at layer 0 has at least 1 neighbour
  - entry_point has the highest level
```

### 4. Recall@K (Main Quality Metric)
```
For 100 random query vectors:
  1. Compute true top-10 using brute force O(N × dim)
  2. Compute HNSW top-10 using hnsw_search()
  3. recall = |intersection| / K

Target: recall@10 ≥ 0.90 with M=16, ef_search=50
Print: "Recall@10: 0.94  (avg over 100 queries)"
```

### 5. Latency Comparison
```
Dataset sizes: N = 1K, 10K, 100K vectors (dim=128)
Measure: avg query time (microseconds) for brute force vs HNSW
Print table:
  N         | Brute Force | HNSW    | Speedup
  1,000     | 0.3 ms      | 0.05 ms | 6x
  10,000    | 3.1 ms      | 0.08 ms | 38x
  100,000   | 31.0 ms     | 0.11 ms | 280x
```

---

## Submission Checklist

### Code
- [ ] `src/vector.c/.h` — Vector ADT + distance functions
- [ ] `src/heap.c/.h` — Min/Max binary heap
- [ ] `src/graph.c/.h` — HNSWNode with per-layer adjacency lists
- [ ] `src/hnsw.c/.h` — Full HNSW index (insert + search)
- [ ] `src/main.c` — Demo program
- [ ] `tests/test_hnsw.c` — Tests with Recall@K output
- [ ] `Makefile` — Builds and runs cleanly with `make`

### Report (`report/report.pdf`, 2–4 pages)
- [ ] What is HNSW and how does it differ from basic graph BFS/DFS?
- [ ] Which syllabus data structures were reused, and how?
- [ ] What real-world problem does this solve? (application section)
- [ ] Complexity analysis: O(M log N) insert, O(log N) query
- [ ] Results table: Recall@K values and latency comparison

### Demo
- [ ] Program compiles with `make` without errors or warnings
- [ ] Inserts at least 1000 vectors
- [ ] Returns correct nearest neighbours for at least 5 query examples
- [ ] Prints Recall@10 ≥ 0.90

---

## References

| # | Reference | Used For |
|---|-----------|---------|
| [1] | Horowitz et al., *Fundamentals of Data Structures in C*, Computer Science Press | Heap and ADT implementation patterns |
| [2] | Karumanchi, *Data Structures and Algorithms Made Easy*, CareerMonk | Priority queue and graph reference |
| [3] | Cormen et al., *Introduction to Algorithms* (CLRS, 4th ed.), MIT Press | Heap correctness proofs, graph theory |
| [4] | Malkov & Yashunin, *"Efficient and robust approximate nearest neighbor search using HNSW"*, arXiv:1603.09320 (2018) | Primary algorithm reference |
| [5] | hnswlib (reference C++ implementation) — https://github.com/nmslib/hnswlib | Reference implementation for verification |

---

*Built for CS233AI Experiential Learning (Part B) — Semester III, Computer Science and Engineering*
