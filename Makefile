OBJS = main.c 
CC = gcc
CFLAGS = -std=c99 -Werror -g -O0 -lpthread

main.out:$(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) 

.PHONY: all
all:main.out

.PHONY: clean
clean: 
	rm -f *.o *.out
