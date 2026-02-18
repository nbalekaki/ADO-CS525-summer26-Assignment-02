#ifndef DBERROR_H
#define DBERROR_H

#include <stdio.h>

/* module wide constants */
#define PAGE_SIZE 4096

/* return code definitions */
typedef int RC;

#define RC_OK 0
#define RC_FILE_NOT_FOUND 1
#define RC_FILE_HANDLE_NOT_INIT 2
#define RC_WRITE_FAILED 3
#define RC_READ_NON_EXISTING_PAGE 4
#define RC_FILE_ALREADY_EXISTS 5
#define RC_INVALID_PAGE_NUM 6

/* Buffer Manager Error Codes */
#define RC_BM_POOL_NOT_INIT 100
#define RC_BM_PAGE_NOT_FOUND 101
#define RC_BM_PAGE_PINNED 102
#define RC_BM_NO_FREE_FRAME 103
#define RC_BM_INVALID_STRATEGY 104

/* B+ Tree Error Codes */
#define RC_BPT_NOT_INITIALIZED 200
#define RC_BPT_KEY_NOT_FOUND 201
#define RC_BPT_DUPLICATE_KEY 202
#define RC_BPT_NODE_FULL 203
#define RC_BPT_SPLIT_FAILED 204
#define RC_BPT_INVALID_NODE 205

/* holder for error messages */
extern char *RC_message;

/* print a message to standard out describing the error */
extern void printError(RC error);
extern char *errorMessage(RC error);

#define THROW(rc, message) \
    do { \
        RC_message = message; \
        return rc; \
    } while (0)

/* check the return code and exit if it is an error */
#define CHECK(code) \
    do { \
        int rc_internal = (code); \
        if (rc_internal != RC_OK) { \
            char *message = errorMessage(rc_internal); \
            printf("[%s-L%i-%s] ERROR: Operation returned error: %s\n", \
                   __FILE__, __LINE__, __TIME__, message); \
            free(message); \
            exit(1); \
        } \
    } while(0)

#endif /* DBERROR_H */
