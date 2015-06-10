#include "parser.h"

IndexerPtr parseIndexFile(char *pathName)
{
	IndexerPtr invertedIndex = NULL;

	char *buffer = NULL;
	buffer = getBuffer(pathName);
	TokenizerT* tokenizer = TKCreate(buffer);
	if(tokenizer == NULL) 
	{
		perror("TKCreate:\n");
		return invertedIndex;
	}
	char *token;
	while ((token = TKGetNextToken(tokenizer)) != NULL)
	{
		if (strcmp(token, "<list>") == 0)
		{
			free(token);
			token = TKGetNextToken(tokenizer);
			if (token == NULL)
			{
				perror("TKGetNextToken:");
				return NULL;
			}

			IndexerPtr index = NULL;
			createIndex(&invertedIndex, index, token);
			HASH_FIND_STR(invertedIndex, token, index);
			free(token);
			createLists(index->records, tokenizer);
		} 
		else
		{
			printf("Incorrect file format\n");
			return NULL;
		}
	}
	free(tokenizer);
	return invertedIndex;
}

void createIndex(IndexerPtr *invertedIndex, IndexerPtr index, char *token)
{
	index = (struct Indexer *)malloc(sizeof(struct Indexer));
	char *tokenKey = (char *)malloc(strlen(token));
	strcpy(tokenKey, token);
	index->token = tokenKey;
	index->records = SLCreate(compareStrings, compareInts, destroyRecord);
	HASH_ADD_KEYPTR(hh, *invertedIndex, index->token, strlen(token), index);
}

void createLists(SortedListPtr list, TokenizerT* tokenizer)
{
	char *token;
	NodePtr curr = list->front;
	while (strcmp((token = TKGetNextToken(tokenizer)), "</list>") != 0)
	{
		if (token == NULL)
		{
			perror("TKGetNextToken:");
			return;
		}

		RecordPtr record = createRecord(token);
		free(token);
		token = TKGetNextToken(tokenizer);
		if (token == NULL)
		{
			perror("TKGetNextToken:");
			return;
		}
		int i;
		for (i = 0; i < strlen(token); i++)
		{
			if (isdigit(token[i]) == 0)
			{
				printf("Incorrect file format\n");
				return;
			}
		}
		int freq = atoi(token);
		record->freq = freq;
		NodePtr newNode = NodeCreate(record);
		if (curr == NULL)
		{
			list->front = newNode;
			curr = list->front;
		}
		else
		{
			curr->next = newNode;
			curr = curr->next;
		}
		newNode->refcnt++;
		free(token);
	}
}
