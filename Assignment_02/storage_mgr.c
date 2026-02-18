/*******************************************************************************
 * Advanced Database Organization - Assignment 02
 * Storage Manager Implementation (from Assignment 01)
 *
 * This is the reference implementation of the Storage Manager.
 * Students should already have a working version from PA1.
 * Replace this file with your own storage_mgr.c if desired.
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "storage_mgr.h"
#include "dberror.h"

/******************************************************************************
 * Initialize the storage manager
 ******************************************************************************/
void initStorageManager(void) {
    /* Nothing to initialize */
}

/******************************************************************************
 * Create a new page file with one empty page (filled with zeros)
 ******************************************************************************/
RC createPageFile(char *fileName) {
    FILE *fp = fopen(fileName, "wb");
    if (fp == NULL) {
        return RC_FILE_NOT_FOUND;
    }

    /* Allocate one page of zeros */
    char *emptyPage = (char *)calloc(PAGE_SIZE, sizeof(char));
    if (emptyPage == NULL) {
        fclose(fp);
        return RC_WRITE_FAILED;
    }

    /* Write the empty page */
    size_t written = fwrite(emptyPage, sizeof(char), PAGE_SIZE, fp);
    free(emptyPage);

    if (written != PAGE_SIZE) {
        fclose(fp);
        return RC_WRITE_FAILED;
    }

    fclose(fp);
    return RC_OK;
}

/******************************************************************************
 * Open an existing page file
 ******************************************************************************/
RC openPageFile(char *fileName, SM_FileHandle *fHandle) {
    FILE *fp = fopen(fileName, "r+b");
    if (fp == NULL) {
        return RC_FILE_NOT_FOUND;
    }

    /* Get file size to calculate number of pages */
    fseek(fp, 0, SEEK_END);
    long fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    /* Store file name (allocate memory for it) */
    fHandle->fileName = (char *)malloc(strlen(fileName) + 1);
    if (fHandle->fileName == NULL) {
        fclose(fp);
        return RC_FILE_HANDLE_NOT_INIT;
    }
    strcpy(fHandle->fileName, fileName);

    /* Calculate total pages */
    fHandle->totalNumPages = (int)(fileSize / PAGE_SIZE);
    fHandle->curPagePos = 0;
    fHandle->mgmtInfo = fp;

    return RC_OK;
}

/******************************************************************************
 * Close an open page file
 ******************************************************************************/
RC closePageFile(SM_FileHandle *fHandle) {
    if (fHandle->mgmtInfo == NULL) {
        return RC_FILE_HANDLE_NOT_INIT;
    }

    fclose((FILE *)fHandle->mgmtInfo);
    free(fHandle->fileName);
    fHandle->mgmtInfo = NULL;
    fHandle->fileName = NULL;

    return RC_OK;
}

/******************************************************************************
 * Destroy (delete) a page file
 ******************************************************************************/
RC destroyPageFile(char *fileName) {
    if (remove(fileName) != 0) {
        return RC_FILE_NOT_FOUND;
    }
    return RC_OK;
}

/******************************************************************************
 * Read a specific block from the file
 ******************************************************************************/
RC readBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage) {
    if (fHandle->mgmtInfo == NULL) {
        return RC_FILE_HANDLE_NOT_INIT;
    }

    /* Validate page number */
    if (pageNum < 0 || pageNum >= fHandle->totalNumPages) {
        return RC_READ_NON_EXISTING_PAGE;
    }

    FILE *fp = (FILE *)fHandle->mgmtInfo;

    /* Seek to the correct page */
    if (fseek(fp, (long)pageNum * PAGE_SIZE, SEEK_SET) != 0) {
        return RC_READ_NON_EXISTING_PAGE;
    }

    /* Read the page */
    size_t bytesRead = fread(memPage, sizeof(char), PAGE_SIZE, fp);
    if (bytesRead != PAGE_SIZE) {
        return RC_READ_NON_EXISTING_PAGE;
    }

    /* Update current page position */
    fHandle->curPagePos = pageNum;

    return RC_OK;
}

/******************************************************************************
 * Get current block position
 ******************************************************************************/
int getBlockPos(SM_FileHandle *fHandle) {
    return fHandle->curPagePos;
}

/******************************************************************************
 * Read the first block
 ******************************************************************************/
RC readFirstBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    return readBlock(0, fHandle, memPage);
}

/******************************************************************************
 * Read the previous block (relative to current position)
 ******************************************************************************/
RC readPreviousBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    return readBlock(fHandle->curPagePos - 1, fHandle, memPage);
}

/******************************************************************************
 * Read the current block
 ******************************************************************************/
RC readCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    return readBlock(fHandle->curPagePos, fHandle, memPage);
}

/******************************************************************************
 * Read the next block (relative to current position)
 ******************************************************************************/
RC readNextBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    return readBlock(fHandle->curPagePos + 1, fHandle, memPage);
}

/******************************************************************************
 * Read the last block
 ******************************************************************************/
RC readLastBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    return readBlock(fHandle->totalNumPages - 1, fHandle, memPage);
}

/******************************************************************************
 * Write a block at a specific page position
 ******************************************************************************/
RC writeBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage) {
    if (fHandle->mgmtInfo == NULL) {
        return RC_FILE_HANDLE_NOT_INIT;
    }

    /* Validate page number */
    if (pageNum < 0 || pageNum >= fHandle->totalNumPages) {
        return RC_WRITE_FAILED;
    }

    FILE *fp = (FILE *)fHandle->mgmtInfo;

    /* Seek to the correct page */
    if (fseek(fp, (long)pageNum * PAGE_SIZE, SEEK_SET) != 0) {
        return RC_WRITE_FAILED;
    }

    /* Write the page */
    size_t bytesWritten = fwrite(memPage, sizeof(char), PAGE_SIZE, fp);
    if (bytesWritten != PAGE_SIZE) {
        return RC_WRITE_FAILED;
    }

    /* Flush to ensure data is written to disk */
    fflush(fp);

    /* Update current page position */
    fHandle->curPagePos = pageNum;

    return RC_OK;
}

/******************************************************************************
 * Write to the current block
 ******************************************************************************/
RC writeCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
    return writeBlock(fHandle->curPagePos, fHandle, memPage);
}

/******************************************************************************
 * Append an empty block to the end of the file
 ******************************************************************************/
RC appendEmptyBlock(SM_FileHandle *fHandle) {
    if (fHandle->mgmtInfo == NULL) {
        return RC_FILE_HANDLE_NOT_INIT;
    }

    FILE *fp = (FILE *)fHandle->mgmtInfo;

    /* Seek to end of file */
    if (fseek(fp, 0, SEEK_END) != 0) {
        return RC_WRITE_FAILED;
    }

    /* Create and write empty page */
    char *emptyPage = (char *)calloc(PAGE_SIZE, sizeof(char));
    if (emptyPage == NULL) {
        return RC_WRITE_FAILED;
    }

    size_t bytesWritten = fwrite(emptyPage, sizeof(char), PAGE_SIZE, fp);
    free(emptyPage);

    if (bytesWritten != PAGE_SIZE) {
        return RC_WRITE_FAILED;
    }

    fflush(fp);
    fHandle->totalNumPages++;

    return RC_OK;
}

/******************************************************************************
 * Ensure the file has at least numberOfPages pages
 ******************************************************************************/
RC ensureCapacity(int numberOfPages, SM_FileHandle *fHandle) {
    if (fHandle->mgmtInfo == NULL) {
        return RC_FILE_HANDLE_NOT_INIT;
    }

    while (fHandle->totalNumPages < numberOfPages) {
        RC rc = appendEmptyBlock(fHandle);
        if (rc != RC_OK) {
            return rc;
        }
    }

    return RC_OK;
}
