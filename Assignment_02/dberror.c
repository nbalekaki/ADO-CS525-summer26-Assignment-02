#include <stdlib.h>
#include <string.h>
#include "dberror.h"

char *RC_message = NULL;

void printError(RC error) {
    char *msg = errorMessage(error);
    printf("Error Code %d: %s\n", error, msg);
    free(msg);
}

char *errorMessage(RC error) {
    char *message;

    if (RC_message != NULL) {
        message = (char *)malloc(strlen(RC_message) + 50);
        sprintf(message, "EC (%d): %s", error, RC_message);
        return message;
    }

    message = (char *)malloc(100);

    switch (error) {
        case RC_OK:
            strcpy(message, "OK");
            break;
        case RC_FILE_NOT_FOUND:
            strcpy(message, "File not found");
            break;
        case RC_FILE_HANDLE_NOT_INIT:
            strcpy(message, "File handle not initialized");
            break;
        case RC_WRITE_FAILED:
            strcpy(message, "Write operation failed");
            break;
        case RC_READ_NON_EXISTING_PAGE:
            strcpy(message, "Attempted to read non-existing page");
            break;
        case RC_FILE_ALREADY_EXISTS:
            strcpy(message, "File already exists");
            break;
        case RC_INVALID_PAGE_NUM:
            strcpy(message, "Invalid page number");
            break;
        case RC_BM_POOL_NOT_INIT:
            strcpy(message, "Buffer pool not initialized");
            break;
        case RC_BM_PAGE_NOT_FOUND:
            strcpy(message, "Page not found in buffer pool");
            break;
        case RC_BM_PAGE_PINNED:
            strcpy(message, "Cannot evict pinned page");
            break;
        case RC_BM_NO_FREE_FRAME:
            strcpy(message, "No free frame available (all pages pinned)");
            break;
        case RC_BM_INVALID_STRATEGY:
            strcpy(message, "Invalid replacement strategy");
            break;
        case RC_BPT_NOT_INITIALIZED:
            strcpy(message, "B+ Tree not initialized");
            break;
        case RC_BPT_KEY_NOT_FOUND:
            strcpy(message, "Key not found in B+ Tree");
            break;
        case RC_BPT_DUPLICATE_KEY:
            strcpy(message, "Duplicate key in B+ Tree");
            break;
        case RC_BPT_NODE_FULL:
            strcpy(message, "B+ Tree node is full");
            break;
        case RC_BPT_SPLIT_FAILED:
            strcpy(message, "B+ Tree node split failed");
            break;
        case RC_BPT_INVALID_NODE:
            strcpy(message, "Invalid B+ Tree node");
            break;
        default:
            sprintf(message, "Unknown error code: %d", error);
            break;
    }

    return message;
}
