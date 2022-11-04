#Makefile
CC=g++
CFLAGS=-Wall -O -g
OBJS=Lab03_pipe.o
Lab03_pipe:$(OBJS)
    $(CC)  $^ -o $(OBJS)
Lab03_pipe:Lab03_pipe.c
    $(CC)   $(CFLAGS)   -c  $<  -o  $@

.PHONY:clean all
clean:
    rm   -f   *.o