# Programming Assignment 2: Disk-Based B+ Tree Index

## Overview
Implement a disk-based B+ Tree index that maps integer keys to Record Identifiers (RIDs).
The tree is stored entirely on disk using the Storage Manager from PA1, with each node
occupying exactly one 4096-byte page.

## Files

| File | Description |
|------|-------------|
| `bptree.h` | B+ Tree header with data structures and API declarations |
| `bptree.c` | **Your implementation goes here** |
| `storage_mgr.h` | Storage Manager interface (from PA1) |
| `storage_mgr.c` | Storage Manager implementation (reference from PA1) |
| `dberror.h` | Error codes and macros |
| `dberror.c` | Error message utilities |
| `dt.h` | Boolean type definitions |
| `test_helper.h` | Test assertion macros |
| `test_bptree_basic.c` | Basic tests (student version) |
| `Makefile` | Build and test targets |

## Building and Testing

```bash
# Build
make

# Run tests
make test

# Memory check (requires Valgrind)
make memcheck-bptree

# Clean
make clean
```

## Design

### On-Disk Layout

**Page 0 (Metadata):**
```
| root_page (int) | next_free_page (int) | unused ... |
```

**Leaf Node:**
```
| BPTNodeHeader (16 bytes) | keys[LEAF_ORDER] | RIDs[LEAF_ORDER] |
```

**Internal Node:**
```
| BPTNodeHeader (16 bytes) | keys[INTERNAL_ORDER] | children[INTERNAL_ORDER+1] |
```

### Node Header
```c
typedef struct {
    int is_leaf;       // 1 = leaf, 0 = internal
    int num_keys;      // number of keys currently stored
    int parent_page;   // parent node page number (-1 for root)
    int next_leaf;     // next leaf in linked list (-1 if none/internal)
} BPTNodeHeader;
```

### Capacity
- Header size: 16 bytes
- Available per page: 4096 - 16 = 4080 bytes
- **Leaf**: each entry = 4 (key) + 8 (RID) = 12 bytes -> max 340 keys
- **Internal**: n keys + (n+1) children = 8n + 4 bytes -> max 509 keys

### B+ Tree Structure Diagram

```
                    [Internal Node]
                   /   keys: [50]   \
                  /                   \
         [Leaf: 10,20,30,40]  -->  [Leaf: 50,60,70,80,90]
         next_leaf --------^
```

After splitting a leaf that overflows:
```
                       [Internal Node]
                      / keys: [30,60] \
                     /       |         \
           [Leaf:10,20] [Leaf:30,40,50] [Leaf:60,70,80,90]
                   \-------->/ \--------->|
                   (next_leaf chain maintains sorted order)
```

### Split Logic

**Leaf Split:**
1. Create temporary array with all existing keys + new key (sorted)
2. Split at midpoint: left half stays, right half goes to new leaf
3. Update `next_leaf` pointers to maintain linked list
4. Push the first key of the new (right) leaf up to the parent

**Internal Split:**
1. Create temporary array with all existing keys/children + new entry
2. Split at midpoint: left half stays, middle key pushed up, right half goes to new node
3. Update parent pointers of all children that moved to the new node
4. Propagate the pushed-up key to the parent (may cascade)

**New Root Creation:**
- When the current root splits, a new root is created with one key
  and two children (the old root halves)

### Assumptions
- Integer keys only (no strings, no floats)
- No duplicate keys
- No deletion support
- Single-threaded access (no concurrency)
- Storage Manager from PA1 is fully functional
- Caller allocates sufficient space for range search results
- Page size is always 4096 bytes

## API Reference

```c
void bpt_init(const char *index_file);  // Open or create index
void bpt_close(void);                    // Close index, flush metadata

int  bpt_search(int key, RID *result);   // Point lookup
void bpt_insert(int key, RID rid);       // Insert key-RID pair
void bpt_range_search(int low, int high, RID *results, int *count);  // Range scan
```

## Deliverables
- `bptree.c` (your implementation)
- `README.md` (this file, updated with your design notes)
- Test output demonstrating correctness
