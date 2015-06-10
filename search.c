#include "search.h"

void searchOptions(IndexerPtr *invertedIndex)
{
	char *input = (char *)malloc(sizeof(char)*256);

	for (;;)
	{
		printf("\nOptions\n\tsa\n\tso\n\tq\nEnter a command: ");
		scanf("%[^\n]%*c", input);
		toLower(input);
		TokenizerT* tokenizer = TKCreate(input);
		char *token = NULL;
		token = TKGetNextToken(tokenizer);
		if (strcmp(token, "sa") == 0)
		{
			free(token);
			sa(invertedIndex, tokenizer);
		}
		else if (strcmp(token, "so") == 0)
		{	
			free(token);
			so(invertedIndex, tokenizer);
		}
		else if (strcmp(token, "q") == 0)
		{
			free(token);
			printf("program exiting\n");
			return;
		}
		else
		{
			if (token != NULL)
				free(token);
			printf("Invalid option, try again\n");
		}
	}
	
	free(input);
}

void sa(IndexerPtr *invertedIndex, TokenizerT* tokenizer)
{
	char *term = NULL;
	SortedListPtr files = SLCreate(compareStrings, compareInts, destroyRecord);
	IndexerPtr index = NULL;
	term = TKGetNextToken(tokenizer);
	if (term == NULL)
	{
		printf("no terms entered");
		return;
	}
	HASH_FIND_STR(*invertedIndex, term, index);
	free(term);
	

	if (index == NULL)
	{
		printf("No files have all of these terms.\n");
		return;
	}
	
	NodePtr iter = index->records->front;
	NodePtr curr = files->front;
	NodePtr prev = NULL;
	while (iter != NULL)
	{
		RecordPtr record = createRecord(iter->data->pathName);
		record->freq = iter->data->freq;
		NodePtr newNode = NodeCreate(record);
		if (curr == NULL)
		{
			files->front = newNode;
			curr = files->front;
		}
		else
		{
			curr->next = newNode;
			curr = curr->next;
		}
		newNode->refcnt++;
		iter = iter->next;
	}

	while ((term = TKGetNextToken(tokenizer)) != NULL)
	{
		HASH_FIND_STR(*invertedIndex, term, index);
		if (index == NULL)
		{
			printf("No files have all of these terms.\n");
			return;
		}
		curr = files->front;
		iter = index->records->front;

		// this can be a function for both sa so. 
		int found = 0;
		while (curr != NULL)
		{
			found = 0;
			iter = index->records->front;
			while(iter != NULL)
			{
				if (strcmp(curr->data->pathName, iter->data->pathName) == 0)
				{
					found = 1;
					break;
				}
				iter = iter->next;
			}
			if (found == 0)
			{
				if (curr == files->front)		
					files->front = curr->next;
				else
					prev->next = curr->next;
				curr->refcnt--;
				NodePtr deleted = curr;
				if (files->front != curr->next)
				{
					curr = prev->next;
					if (prev->next != NULL)
						prev->next->refcnt++;
				}
				else curr = curr->next;
				deleted->isRemoved = 1;
				if (deleted->refcnt == 0)			
				{ 
					NodeDestroy(deleted, files->df);
					if (deleted->next != NULL)
						deleted->next->refcnt--;
				}
				continue;
			}
			prev = curr;
			curr = curr->next;
		}
		free(term);
	}

	curr = files->front;
	if (curr == NULL)
	{
		printf("No files have all of these terms.\n");
		return;
	}
	printf("\n");
	while (curr != NULL)
	{
		printf("%s\n", curr->data->pathName);
		curr = curr->next;
	}
	SLDestroy(files);
}

void so(IndexerPtr *invertedIndex, TokenizerT* tokenizer)
{
	char *term = NULL;
	SortedListPtr files = SLCreate(compareStrings, compareInts, destroyRecord);
	IndexerPtr index = NULL;

	while (index == NULL)
	{
		term = TKGetNextToken(tokenizer);
		if (term == NULL) break;
		HASH_FIND_STR(*invertedIndex, term, index);
		free(term);
	}

	if (index == NULL)
	{
		printf("No files have any of these terms or there are no terms.\n");
		return;
	}
	
	NodePtr iter = index->records->front;
	NodePtr curr = files->front;
	NodePtr prev = NULL;
	while (iter != NULL)
	{
		RecordPtr record = createRecord(iter->data->pathName);
		record->freq = iter->data->freq;
		NodePtr newNode = NodeCreate(record);
		if (curr == NULL)
		{
			files->front = newNode;
			curr = files->front;
		}
		else
		{
			curr->next = newNode;
			curr = curr->next;
		}
		newNode->refcnt++;
		iter = iter->next;
	}

	while((term = TKGetNextToken(tokenizer)) != NULL)
	{
		HASH_FIND_STR(*invertedIndex, term, index);
		if (index == NULL) continue;
		iter = index->records->front;
		// this can be a function for both sa so. 
		int found = 0;
		// this is wrong, need to fix this.
		
		while (iter != NULL)
		{
			found = 0;
			curr = files->front;
			while(curr != NULL)
			{
				if (strcmp(curr->data->pathName, iter->data->pathName) == 0)
				{
					found = 1;
					break;
				}
				prev = curr;
				curr = curr->next;
			}
			if (found == 0)
			{
				RecordPtr record = createRecord(iter->data->pathName);
				record->freq = iter->data->freq;
				NodePtr newNode = NodeCreate(record);
				prev->next = newNode;
			}
			iter = iter->next;
		}
		free(term);
	}

	// print
	curr = files->front;
	printf("\n");
	if (curr == NULL)
		printf("No files have any of these terms.\n");
	while (curr != NULL)
	{
		printf("%s\n", curr->data->pathName);
		curr = curr->next;
	}
	SLDestroy(files);
}

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("Invalid args\n");
		return 1;
	}
	
	IndexerPtr invertedIndex = parseIndexFile(argv[1]);
	if (invertedIndex == NULL) return 1;
	searchOptions(&invertedIndex);
	destroyHash(&invertedIndex);
	return 0;
}
