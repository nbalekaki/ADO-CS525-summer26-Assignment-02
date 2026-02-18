/*******************************************************************************
 * Advanced Database Organization - Assignment 02
 * B+ Tree Index Implementation
 *
 * Student Name: [YOUR NAME]
 * Student ID:   [YOUR ID]
 *
 * INSTRUCTIONS:
 * - Implement all functions declared in bptree.h
 * - Do not modify the function signatures
 * - Use the Storage Manager from Assignment 01 for all disk I/O
 * - Use the provided dberror.h return codes
 * - Test your implementation using the provided test files
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bptree.h"
#include "dberror.h"

/******************************************************************************
 * INTERNAL STATE
 ******************************************************************************/

/*
 * Metadata stored on page 0 of the index file.
 * You may extend this structure as needed.
 */
typedef struct {
    int root_page;      /* page number of root node, -1 if tree is empty */
    int next_free_page; /* next page number to allocate */
} BPTMetadata;

/* Global state for the currently open index */
static SM_FileHandle indexFH;
static int indexOpen = 0;
static BPTMetadata meta;

/******************************************************************************
 * HELPER: READ / WRITE METADATA (page 0)
 ******************************************************************************/

static void readMeta(void) {
    /* TODO: Read metadata from page 0 into the 'meta' struct */
}

static void writeMeta(void) {
    /* TODO: Write the 'meta' struct to page 0 */
}

/******************************************************************************
 * HELPER: READ / WRITE A NODE (one page per node)
 ******************************************************************************/

static void readNode(int pageNum, char *page) {
    /* TODO: Read a node page using the storage manager */
}

static void writeNode(int pageNum, char *page) {
    /* TODO: Write a node page using the storage manager */
}

/******************************************************************************
 * HELPER: ACCESS NODE FIELDS
 ******************************************************************************/

/*
 * Use these helpers to access fields within a page buffer.
 *
 * Page layout for LEAF nodes:
 *   | BPTNodeHeader | keys[LEAF_ORDER] | RIDs[LEAF_ORDER] |
 *
 * Page layout for INTERNAL nodes:
 *   | BPTNodeHeader | keys[INTERNAL_ORDER] | children[INTERNAL_ORDER+1] |
 */

static BPTNodeHeader *getHeader(char *page) {
    return (BPTNodeHeader *)page;
}

static int *getKeys(char *page) {
    return (int *)(page + BPT_HEADER_SIZE);
}

/* Leaf: RIDs follow keys */
static RID *getLeafRIDs(char *page) {
    return (RID *)(page + BPT_HEADER_SIZE + LEAF_ORDER * sizeof(int));
}

/* Internal: child page ids follow keys */
static int *getChildren(char *page) {
    return (int *)(page + BPT_HEADER_SIZE + INTERNAL_ORDER * sizeof(int));
}

/******************************************************************************
 * HELPER: ALLOCATE A NEW PAGE
 ******************************************************************************/

static int allocatePage(void) {
    /* TODO: Return the next free page number and ensure capacity */
    return -1;
}

/******************************************************************************
 * HELPER: INITIALIZE AN EMPTY NODE
 ******************************************************************************/

static void initNode(char *page, int is_leaf) {
    /* TODO: Zero out page and initialize the header fields */
}

/******************************************************************************
 * HELPER: FIND THE LEAF PAGE THAT SHOULD CONTAIN A KEY
 ******************************************************************************/

static int findLeaf(int key) {
    /* TODO: Traverse from root to leaf following internal node pointers */
    return BPT_NO_PAGE;
}

/******************************************************************************
 * INDEX LIFECYCLE
 ******************************************************************************/

/**
 * Initialize or open a B+ Tree index file.
 * If the file does not exist, create it with a metadata page.
 * If it exists, read the metadata.
 *
 * @param index_file Name of the index file
 */
void bpt_init(const char *index_file) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Initialize the storage manager
     * 2. Try to open an existing index file
     * 3. If it exists, read metadata from page 0
     * 4. If it does not exist, create the file, write initial metadata
     * 5. Set indexOpen = 1
     */
}

/**
 * Close the B+ Tree index file.
 * Flush metadata and close the underlying storage manager file.
 */
void bpt_close(void) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Write metadata to page 0
     * 2. Close the page file
     * 3. Set indexOpen = 0
     */
}

/******************************************************************************
 * SEARCH
 ******************************************************************************/

/**
 * Search for a key in the B+ Tree.
 *
 * @param key The key to search for
 * @param result Pointer to RID to populate on success
 * @return RC_OK if found, RC_BPT_KEY_NOT_FOUND otherwise
 */
int bpt_search(int key, RID *result) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. If tree is empty, return RC_BPT_KEY_NOT_FOUND
     * 2. Find the leaf page using findLeaf()
     * 3. Scan the leaf's keys for a match
     * 4. If found, copy the RID and return RC_OK
     * 5. If not found, return RC_BPT_KEY_NOT_FOUND
     */
    return RC_BPT_KEY_NOT_FOUND;
}

/******************************************************************************
 * INSERT
 ******************************************************************************/

/**
 * Insert a key-RID pair into the B+ Tree.
 * Splits nodes when full and propagates upward.
 * Creates a new root when the current root splits.
 *
 * @param key The integer key to insert
 * @param rid The record identifier to associate with the key
 */
void bpt_insert(int key, RID rid) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. If tree is empty, create a leaf root node with this entry
     * 2. Otherwise, find the leaf using findLeaf()
     * 3. If the leaf has room (num_keys < LEAF_ORDER), insert directly
     * 4. If the leaf is full, split the leaf:
     *    a. Create a temp array of all keys + new key, sorted
     *    b. Split into left (first half) and new right (second half) leaf
     *    c. Update next_leaf pointers
     *    d. Push the first key of the right leaf into the parent
     * 5. Splitting the parent may cascade upward
     * 6. If the root splits, create a new root
     */
}

/******************************************************************************
 * RANGE SEARCH
 ******************************************************************************/

/**
 * Perform a range search for all keys in [low, high].
 * Traverses the leaf-level linked list.
 *
 * @param low Lower bound (inclusive)
 * @param high Upper bound (inclusive)
 * @param results Array to populate with matching RIDs (caller allocates)
 * @param count Pointer to int; set to the number of results found
 */
void bpt_range_search(int low, int high, RID *results, int *count) {
    /* TODO: Implement this function
     *
     * Steps:
     * 1. Set *count = 0
     * 2. If tree is empty, return
     * 3. Find the leaf containing the 'low' key using findLeaf()
     * 4. Scan keys in the leaf; collect those in [low, high]
     * 5. Follow next_leaf pointers to continue scanning
     * 6. Stop when key > high or no more leaves
     */
}

/******************************************************************************
 * BONUS: BULK LOADING (5 pts)
 ******************************************************************************/

void bpt_bulk_load(int *keys, RID *rids, int n) {
    /* BONUS TODO: Implement bulk loading
     *
     * Build a B+ Tree from pre-sorted key-RID arrays more efficiently
     * than inserting one key at a time.
     *
     * Steps:
     * 1. Fill leaf nodes sequentially (each leaf holds up to LEAF_ORDER keys)
     * 2. Link leaves via next_leaf pointers
     * 3. Build internal nodes bottom-up using separator keys
     * 4. Set the root page in metadata
     *
     * Precondition: keys[] must be sorted in ascending order
     *
     * Hint: A simple fallback is to just call bpt_insert() in a loop,
     * but for full bonus credit, implement the bottom-up approach.
     */
}
