CC = gcc
FLAGS = -Wall -g -o

all:	index main.o tokenizer.o indexer.o sorted-list.o

main.o:	main.c 
	$(CC) -g -c main.c

tokenizer.o: tokenizer.c tokenizer.h
	$(CC) -g -c tokenizer.c

indexer.o: indexer.c indexer.h
	$(CC) -g -c indexer.c

sorted-list.o: sorted-list.c sorted-list.h
	$(CC) -g -c sorted-list.c

index:	main.o tokenizer.o indexer.o sorted-list.o
	$(CC) $(FLAGS) index main.o tokenizer.o indexer.o sorted-list.o

clean: 
	rm -rf *.o index
