CC = gcc
CFLAGS = -std=c99 -O2 -Wall

all: perturbedUM make_euclidean make_random

perturbedUM: instance_generation/perturbedUM.c ../src/common/tree.c ../src/common/matrix.c ../src/common/list.c ../src/common/stack.c
	$(CC) $(CFLAGS) instance_generation/perturbedUM.c ../src/common/tree.c ../src/common/matrix.c ../src/common/list.c ../src/common/stack.c -lm -o instance_generation/perturbedUM

make_euclidean: instance_generation/make_euclidean.c ../src/common/matrix.c 
	$(CC) $(CFLAGS) instance_generation/make_euclidean.c ../src/common/matrix.c -lm -o instance_generation/make_euclidean

make_random: instance_generation/make_random.c ../src/common/matrix.c 
	$(CC) $(CFLAGS) instance_generation/make_random.c ../src/common/matrix.c -lm -o instance_generation/make_random

clean:
	rm -f instance_generation/perturbedUM instance_generation/make_euclidean instance_generation/make_random 
