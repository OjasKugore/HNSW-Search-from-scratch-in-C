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
	rm -rf $(TARGET) test_hnsw *.o *.dSYM
