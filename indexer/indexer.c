#include "indexer.h"

int compareInts(void *p1, void *p2)
{
	int i1 = *(int*)p1;
	int i2 = *(int*)p2;

	return i1 - i2;
}

int compareStrings(void *p1, void *p2)
{
	char *s1 = p1;
	char *s2 = p2;

	return strcmp(s1, s2);
}

int compareTokens(struct Indexer *p1, struct Indexer *p2)
{
	char *s1 = p1->token;
	char *s2 = p2->token;

	return strcmp(s1, s2);
}

void toLower(char * token)
{
	int i;
	for (i = 0; i < strlen(token); i++)
	{
		token[i] = tolower(token[i]);
	}
}

void updateFreq(SortedListPtr list, NodePtr curr)
{
	curr->data->freq++;
	if (curr == list->front) return;
	NodePtr ptr = list->front;
	int compare;
	// if the current freq is larger than the ptr freq.
	// the data is swapped.
	// this repeats uptil the order is fixed. 
	while (ptr != NULL)
	{
		compare = list->cf2(&(curr->data->freq), &(ptr->data->freq));
		if (compare > 0)
		{
			RecordPtr temp = curr->data;
			curr->data = ptr->data;
			ptr->data = temp;
			ptr = ptr->next;
		}
		else break;
	}
}

RecordPtr createRecord(char *pathName)
{
	RecordPtr record = NULL;
	record = (RecordPtr)malloc(sizeof(struct record));
	record->pathName = (char *)malloc(strlen(pathName) + 1);
	strcpy(record->pathName, pathName);
	char * name = (char *)malloc(strlen(pathName) + 1);
	int i,j, k;
	for(i = strlen(pathName); i >= 0; i--)
	{
		if (pathName[i] == '/') break;
			
	}
	for(j = i + 1,k = 0; j < strlen(pathName); j++, k++)
	{
		name[k] = pathName[j];
	}
	name[k] = '\0';
	record->fileName = name;
	record->freq = 1;
	return record;
}

int searchRecords(SortedListPtr list, char *pathName)	// -1 if error.
{
	if (!pathName) 
	{
		printf("invalid path.\n");
		return -1;
	}
	
	NodePtr curr = list->front;
	NodePtr prev = NULL;
	int compare;
	while (curr != NULL)
	{
		compare = list->cf(pathName, curr->data->pathName);
		if (compare == 0)
		{
			updateFreq(list, curr);
			return 0;
		}
		prev = curr;
		curr = curr->next;
	}
	
	RecordPtr record = createRecord(pathName);
	NodePtr newNode = NodeCreate(record);
	if (prev == NULL)
		list->front = newNode;
	else
		prev->next = newNode;
	newNode->refcnt++;
	return 0;
}

void destroyRecord(void *rec)
{
	RecordPtr record = (RecordPtr)rec;
	if (!record)
		return;
	free(record->fileName);
	free(record->pathName);
	free(record);
	record->pathName = NULL;
	record = NULL;
}

void destroyHash(struct Indexer* invertedIndex)
{
	struct Indexer * index, *temp;
	HASH_ITER(hh, invertedIndex, index, temp)
	{	
		HASH_DEL(invertedIndex, index);
		if (!index)
		{
			free(index->token);
			SLDestroy(index->records);
			free(index);
		}
	}
}

char * getBuffer(char *path)
{
	
	int fd = open(path, O_RDONLY);
	if (fd < 0) 
	{
		perror("open");
		exit(1);
	}
	int filesize = lseek(fd, 0, SEEK_END);
	char * buffer = (char *)malloc(filesize + 1);
	lseek(fd, 0, SEEK_SET);
	read(fd, buffer, filesize);
	buffer[filesize] = '\0';
	if(close(fd) == -1)
	{
		perror("close");
		exit(1);	
	}
	return buffer;
}

struct Indexer* scanFile(char *fileName, char *pathName, struct Indexer *invertedIndex)
{
		char * buffer = NULL;
		buffer = getBuffer(fileName);
		TokenizerT* tokenizer = TKCreate(buffer);
		
		if(tokenizer == NULL) 
		{
			perror("TKCreate\n");
		}
		char* token = NULL;
		while((token = TKGetNextToken(tokenizer)) != NULL) 
		{
			toLower(token);
			struct Indexer *index = NULL;
			HASH_FIND_STR(invertedIndex, token, index);
			if (!index)
			{
				index = (struct Indexer *)malloc(sizeof(struct Indexer));
				char *tokenKey = (char *)malloc(strlen(token));
				strcpy(tokenKey, token);
				index->token = tokenKey;
				index->records = SLCreate(compareStrings, compareInts, destroyRecord);
				HASH_ADD_KEYPTR(hh, invertedIndex, index->token, strlen(token), index);
			}
			searchRecords(index->records, pathName);
			free(token);
		}
		free(tokenizer);
		free(buffer);
		return invertedIndex;
}

struct Indexer* scanDirectory(char *dirName, char *path, struct Indexer * invertedIndex)
{
	DIR * dir = opendir(dirName);
	if (!dir) 
	{
		perror("opendir");
		exit(0);
	}
	
	struct dirent * entry = NULL;
	struct stat statbuf;
	chdir(dirName);
	char *pathName = (char *)malloc(sizeof(char)*256);
	strcpy(pathName, path);
	strcat(pathName, "/");
	int len = strlen(pathName);
	while (entry = readdir(dir))
	{
		lstat(entry->d_name, &statbuf);
		if (S_ISDIR(statbuf.st_mode)) {
			if (!strcmp(".", entry->d_name) || !strcmp("..", entry->d_name))
				continue;
			strcat(pathName, entry->d_name);
			invertedIndex = scanDirectory(entry->d_name, pathName, invertedIndex);
			pathName[len] = '\0';
		}
		else
		{
			strcat(pathName, entry->d_name);
			invertedIndex = scanFile(entry->d_name, pathName, invertedIndex);
			pathName[len] = '\0';
		}
	}
	chdir("..");
	closedir(dir);
	return invertedIndex;
}
