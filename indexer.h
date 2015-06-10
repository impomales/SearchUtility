#include <sys/stat.h>
#include <stdio.h>
#include <dirent.h>
#include "tokenizer.h"
#include "sorted-list.h"
#include <errno.h>
#include "uthash.h"	// third party lib from Troy D. Hanson at GitHub

// hash table.
struct Indexer
{
	char * token;	// key
	SortedListPtr records;
	UT_hash_handle hh;
};

typedef struct Indexer *IndexerPtr;

// changes a string to all lower cases. 
void toLower(char *token);
// comparison functions.. 0 (==), > 0 (first is greater), < 0 (second is greater)
int compareInts(void *p1, void *p2);
int compareStrings(void *p1, void *p2);
// updates the frequency count and list order.
void updateFreq(SortedListPtr list, NodePtr curr);
// creates a pointer containing the pathname and frequency.
// eachnode data pointer points to one of these. 
RecordPtr createRecord(char *pathName);
// searches for a file in a list. if not there file is added, otherwise updatefreq.
int searchRecords(SortedListPtr list, char *pathName);
// used when clearing memory, destroyer function in list.
void destroyRecord(void *rec);
// destroys hash struct
void destroyHash(struct Indexer** invertedIndex);
// creates buffer to store contents of a file
char * getBuffer(char *path);
// scans file, tokenized words are hashed onto a linked list.
struct Indexer* scanFile(char *fileName, char *pathName, struct Indexer *invertedIndex);
// scans directory. if there's a file, it is scanned. directories are scanned recursively. 
struct Indexer* scanDirectory(char *dirName, char *path, struct Indexer * invertedIndex);
void printHash(IndexerPtr *invertedIndex, FILE *output);
