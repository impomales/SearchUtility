#include "parser.h"

// loops inititely until q comman etered.
// promts user to enter sa, so , or q command. 
void searchOptions(IndexerPtr *invertedIndex);
// search logical and of terms.
void sa(IndexerPtr *invertedIndex, TokenizerT *tokenizer);
// search logical or of terms.
void so(IndexerPtr *invertedIndex, TokenizerT *tokenizer);
