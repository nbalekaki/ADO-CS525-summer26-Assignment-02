# Advanced Database Organization - Assignment 02
# Student Makefile

CC = gcc
CFLAGS = -Wall -g

# Source files
BPTREE_SRC = bptree.c storage_mgr.c dberror.c

# Test files
TEST_BPTREE_SRC = test_bptree_basic.c

.PHONY: all clean test test-bptree

# Default target
all: test_bptree_basic

# Build B+ Tree test
test_bptree_basic: $(TEST_BPTREE_SRC) $(BPTREE_SRC)
	$(CC) $(CFLAGS) -o $@ $^

# Run all tests
test: test-bptree

test-bptree: test_bptree_basic
	@echo "========================================"
	@echo "Running B+ Tree Basic Tests"
	@echo "========================================"
	./test_bptree_basic

# Memory check with Valgrind
memcheck-bptree: test_bptree_basic
	valgrind --leak-check=full --show-leak-kinds=all ./test_bptree_basic

# Clean build artifacts
clean:
	rm -f test_bptree_basic
	rm -f *.o
	rm -f *.bin
