#include "indexer.h"

// scans file and creates a hash table. 
IndexerPtr parseIndexFile(char *pathName);

// initiales an index
void createIndex(IndexerPtr *invertedIndex, IndexerPtr index, char *token);

// creates lists for files that contain a specific token.
void createLists(SortedListPtr list, TokenizerT* tokenizer);
