#ifndef BPTREE_H
#define BPTREE_H

#include "storage_mgr.h"
#include "dberror.h"

/************************************************************
 *                    data structures                       *
 ************************************************************/

/* Record Identifier: page_id + slot_id */
typedef struct {
    int page_id;
    int slot_id;
} RID;

/* Node header stored at the beginning of each page */
typedef struct {
    int is_leaf;
    int num_keys;
    int parent_page;
    int next_leaf;      /* leaf nodes only; -1 for internal */
} BPTNodeHeader;

/************************************************************
 *                    computed constants                    *
 ************************************************************/

/* Available space for keys/pointers after the header */
#define BPT_HEADER_SIZE  ((int)sizeof(BPTNodeHeader))
#define BPT_AVAIL_SPACE  (PAGE_SIZE - BPT_HEADER_SIZE)

/*
 * Leaf node layout:  | header | keys[n] | rids[n] |
 *   each entry = sizeof(int) + sizeof(RID) = 4 + 8 = 12 bytes
 *   max keys = BPT_AVAIL_SPACE / 12
 */
#define LEAF_ORDER  (BPT_AVAIL_SPACE / (int)(sizeof(int) + sizeof(RID)))

/*
 * Internal node layout:  | header | keys[n] | children[n+1] |
 *   n keys + (n+1) children => 4n + 4(n+1) = 8n + 4 bytes
 *   max keys = (BPT_AVAIL_SPACE - 4) / 8
 */
#define INTERNAL_ORDER  ((BPT_AVAIL_SPACE - (int)sizeof(int)) / (int)(sizeof(int) + sizeof(int)))

/* Sentinel values */
#define BPT_NO_PAGE  -1
#define BPT_META_PAGE  0    /* page 0 stores tree metadata */

/************************************************************
 *                    interface                             *
 ************************************************************/

/* Index lifecycle */
extern void bpt_init(const char *index_file);
extern void bpt_close(void);

/* Operations */
extern int  bpt_search(int key, RID *result);
extern void bpt_insert(int key, RID rid);
extern void bpt_range_search(int low, int high, RID *results, int *count);

/* BONUS: Bulk loading - builds a B+ Tree from pre-sorted key-RID arrays */
extern void bpt_bulk_load(int *keys, RID *rids, int n);

#endif /* BPTREE_H */
