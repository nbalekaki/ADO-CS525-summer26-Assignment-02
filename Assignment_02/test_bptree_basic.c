/*******************************************************************************
 * Advanced Database Organization - Assignment 02
 * B+ Tree Basic Tests (Student Version)
 *
 * These are BASIC tests. The autograder runs more comprehensive hidden tests.
 * Passing these tests does NOT guarantee full marks.
 *
 * Grading (hidden tests): 70 points
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bptree.h"
#include "dberror.h"
#include "test_helper.h"

char *testName;
int testsPassed = 0;
int testsFailed = 0;

#define TEST_INDEX "test_bptree.bin"

void cleanup(void) {
    remove(TEST_INDEX);
}

/******************************************************************************
 * Test: Basic insert and search
 ******************************************************************************/
void testBasicInsertSearch(void) {
    RID rid, result;
    int rc;

    testName = "Basic Insert and Search";
    printf(">> TEST: %s\n", testName);

    cleanup();
    bpt_init(TEST_INDEX);

    /* Insert a few keys */
    rid.page_id = 1; rid.slot_id = 10;
    bpt_insert(50, rid);

    rid.page_id = 2; rid.slot_id = 20;
    bpt_insert(30, rid);

    rid.page_id = 3; rid.slot_id = 30;
    bpt_insert(70, rid);

    rid.page_id = 4; rid.slot_id = 40;
    bpt_insert(10, rid);

    rid.page_id = 5; rid.slot_id = 50;
    bpt_insert(90, rid);

    /* Search for existing keys */
    rc = bpt_search(50, &result);
    ASSERT_EQUALS_INT(RC_OK, rc, "search for key 50 should succeed");
    ASSERT_EQUALS_INT(1, result.page_id, "key 50 page_id");
    ASSERT_EQUALS_INT(10, result.slot_id, "key 50 slot_id");

    rc = bpt_search(30, &result);
    ASSERT_EQUALS_INT(RC_OK, rc, "search for key 30 should succeed");
    ASSERT_EQUALS_INT(2, result.page_id, "key 30 page_id");

    rc = bpt_search(10, &result);
    ASSERT_EQUALS_INT(RC_OK, rc, "search for key 10 should succeed");
    ASSERT_EQUALS_INT(4, result.page_id, "key 10 page_id");

    /* Search for non-existing key */
    rc = bpt_search(42, &result);
    ASSERT_TRUE(rc != RC_OK, "search for non-existing key 42 should fail");

    bpt_close();
    cleanup();

    TEST_DONE();
}

/******************************************************************************
 * Test: Range search
 ******************************************************************************/
void testRangeSearch(void) {
    RID rid;
    RID results[100];
    int count;

    testName = "Range Search";
    printf(">> TEST: %s\n", testName);

    cleanup();
    bpt_init(TEST_INDEX);

    /* Insert keys 10, 20, 30, ..., 100 */
    for (int i = 1; i <= 10; i++) {
        rid.page_id = i;
        rid.slot_id = i * 10;
        bpt_insert(i * 10, rid);
    }

    /* Range [25, 75] should return keys 30, 40, 50, 60, 70 */
    bpt_range_search(25, 75, results, &count);
    ASSERT_EQUALS_INT(5, count, "range [25,75] should find 5 keys");

    /* Verify ordering */
    ASSERT_EQUALS_INT(3, results[0].page_id, "first result page_id=3 (key 30)");
    ASSERT_EQUALS_INT(7, results[4].page_id, "last result page_id=7 (key 70)");

    /* Range [1, 9] should return nothing */
    bpt_range_search(1, 9, results, &count);
    ASSERT_EQUALS_INT(0, count, "range [1,9] should find 0 keys");

    /* Range [10, 100] should return all 10 */
    bpt_range_search(10, 100, results, &count);
    ASSERT_EQUALS_INT(10, count, "range [10,100] should find 10 keys");

    bpt_close();
    cleanup();

    TEST_DONE();
}

/******************************************************************************
 * Test: Many insertions (triggers splits)
 ******************************************************************************/
void testManyInsertions(void) {
    RID rid, result;
    int rc;
    int numKeys = 1000;

    testName = "Many Insertions (1000 keys)";
    printf(">> TEST: %s\n", testName);

    cleanup();
    bpt_init(TEST_INDEX);

    /* Insert 1000 keys in shuffled order */
    for (int i = 0; i < numKeys; i++) {
        int key = (i * 37 + 13) % numKeys;  /* pseudo-shuffle */
        rid.page_id = key;
        rid.slot_id = key * 2;
        bpt_insert(key, rid);
    }

    /* Verify all keys can be found */
    int found = 0;
    for (int i = 0; i < numKeys; i++) {
        rc = bpt_search(i, &result);
        if (rc == RC_OK && result.page_id == i) {
            found++;
        }
    }
    ASSERT_EQUALS_INT(numKeys, found, "all 1000 keys should be found");

    /* Non-existing key */
    rc = bpt_search(numKeys + 1, &result);
    ASSERT_TRUE(rc != RC_OK, "key beyond range should not be found");

    bpt_close();
    cleanup();

    TEST_DONE();
}

/******************************************************************************
 * Test: Persistence (close and reopen)
 ******************************************************************************/
void testPersistence(void) {
    RID rid, result;
    int rc;

    testName = "Persistence";
    printf(">> TEST: %s\n", testName);

    cleanup();
    bpt_init(TEST_INDEX);

    /* Insert some keys */
    for (int i = 1; i <= 100; i++) {
        rid.page_id = i;
        rid.slot_id = i;
        bpt_insert(i, rid);
    }

    bpt_close();

    /* Reopen and verify */
    bpt_init(TEST_INDEX);

    rc = bpt_search(50, &result);
    ASSERT_EQUALS_INT(RC_OK, rc, "key 50 should survive close/reopen");
    ASSERT_EQUALS_INT(50, result.page_id, "key 50 page_id after reopen");

    rc = bpt_search(1, &result);
    ASSERT_EQUALS_INT(RC_OK, rc, "key 1 should survive close/reopen");

    rc = bpt_search(100, &result);
    ASSERT_EQUALS_INT(RC_OK, rc, "key 100 should survive close/reopen");

    bpt_close();
    cleanup();

    TEST_DONE();
}

/******************************************************************************
 * MAIN
 ******************************************************************************/
int main(void) {
    printf("============================================================\n");
    printf("     B+ TREE BASIC TESTS (Student Version)\n");
    printf("     Note: Hidden tests are more comprehensive!\n");
    printf("============================================================\n\n");

    testBasicInsertSearch();
    testRangeSearch();
    testManyInsertions();
    testPersistence();

    cleanup();

    printf("\n============================================================\n");
    printf("Tests Passed: %d\n", testsPassed);
    printf("Tests Failed: %d\n", testsFailed);
    printf("============================================================\n");

    return (testsFailed == 0) ? 0 : 1;
}
