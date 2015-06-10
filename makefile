CC = gcc
FLAGS = -Wall -g -o

all:	search search.o parser.o tokenizer.o indexer.o sorted-list.o

search.o:	search.c search.h
	$(CC) -g -c search.c

parser.o:	parser.c parser.h
	$(CC) -g -c parser.c

tokenizer.o: tokenizer.c tokenizer.h
	$(CC) -g -c tokenizer.c

indexer.o: indexer.c indexer.h
	$(CC) -g -c indexer.c

sorted-list.o: sorted-list.c sorted-list.h
	$(CC) -g -c sorted-list.c

search:	search.o parser.o tokenizer.o indexer.o sorted-list.o
	$(CC) $(FLAGS) search search.o parser.o tokenizer.o indexer.o sorted-list.o

clean: 
	rm -rf *.o search
