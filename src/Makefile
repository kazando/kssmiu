CC = gcc
CFLAGS = -D_POSIX_C_SOURCE=200809L -I . -I common -I kss -std=c99 -O2 -Wall -Wno-unused-variable -Wno-unused-but-set-variable -Wno-maybe-uninitialized -g

all: ../app/min_inc

../app/min_inc: ../app/min_inc.c common/list.o common/matrix.o common/tree.o common/linkage.o kss/kss.o common/stack.o
	$(CC) $(CFLAGS) ../app/min_inc.c common/list.o common/matrix.o common/tree.o common/linkage.o kss/kss.o common/stack.o -lm -o $@

common/%.o: common/%.c
	$(CC) $(CFLAGS) -c $< -o $@

kss/%.o: kss/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f ../app/min_inc common/*.o kss/*.o

