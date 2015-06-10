
#include <stdio.h>
#include <dirent.h>
#include "indexer.h"
#define NUM_OF_ARGS 3

struct Indexer *invertedIndex = NULL;

int main (int argc, char **argv) 
{
	if(argc != 3){
		printf("Error: invalid number of arguments\n");
		return -1;
	}
	
	// if file already exist, confirm.
	if (access(argv[1], F_OK) != -1)
	{
		printf("Any key to overwrite the file %s, 0 to quit: ", argv[1]);
		int choice;
		scanf("%d", &choice);
		if (choice == 0) exit(0);
	}
	FILE *output = fopen(argv[1], "w");
	
	// if not a directory than just scan the file.
	DIR * dir = opendir(argv[2]);
	if (!dir) 
	{
		if (errno == ENOTDIR)
			invertedIndex = scanFile(argv[2], argv[2], invertedIndex);
		else if (errno == ENOENT) {
			perror("opendir");
			exit(1);
		}
		else exit(1);
	}
	else
		invertedIndex = scanDirectory(argv[2], argv[2], invertedIndex);	
	// sorts hash in 0-9, a-z order
	HASH_SORT(invertedIndex, compareTokens);

	// printing the contents onto the output file.
	struct Indexer *index, *temp;
	HASH_ITER(hh, invertedIndex, index, temp)
	{
		NodePtr ptr = index->records->front;
		fprintf(output, "<list> %s\n\n", index->token);
		int i = 0;
		while(ptr != NULL)
		{
			
			fprintf(output, "%s %d ", ptr->data->fileName, ptr->data->freq);
			ptr = ptr->next;
			i = i++;
			if (i == 5)
				fprintf(output, "\n");
		}
		fprintf(output, "\n\n");
		fprintf(output, "</list>");
		fprintf(output, "\n\n");
	}
	
	// memory clean up
	destroyHash(invertedIndex);
	fclose(output);
	closedir(dir);

	return 0;
}
