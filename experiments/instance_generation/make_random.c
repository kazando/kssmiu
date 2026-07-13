#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "../../src/common/matrix.h"

static char *xstrdup(const char *s) {
    if (s == NULL) return NULL;
    size_t len = strlen(s) + 1;
    char *copy = malloc(len);
    if (copy != NULL) {
        memcpy(copy, s, len);
    }
    return copy;
}

int main(int argc, char *argv[]) {

	int i, j, n, seed;
	double num;
	char filename[1024];
	
	if(argc < 3 || argc > 4) {
		fprintf(stderr, "Usage: %s <node> <seed> [<filename>]\n", argv[0]);
		exit(1);
	}
	
	n = atoi(argv[1]);
	seed=atoi(argv[2]);

  	srand(seed);

	if(argc == 4) {
		strcpy(filename, argv[3]);
	} else {
		sprintf(filename, "node%d_%d.phylip", n, seed);
	}

	Matrix M;
	init_matrix(&M, n);
	
	for (i = 1; i < n; i++) {
		for (j = i + 1; j <= n; j++) {
			num = ((double)rand() / RAND_MAX) * n;
			M.cpn[i][j] = M.cpn[j][i] = num;
		}
	}

	for (int i = 1; i <= n; i++) {
    	char label[32];
    	snprintf(label, sizeof(label), "Seq%d", i);
    	M.labels[i] = xstrdup(label);
    	if (M.labels[i] == NULL) {
        	fprintf(stderr, "Error: strdup failed for label %d.\n", i);
        	del_matrix(&M);
        	return EXIT_FAILURE;
        }
	}

	write_matrix_phylip(filename, &M);

}
