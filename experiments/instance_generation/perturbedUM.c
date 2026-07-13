/*
 * random_bintree
 *  Create a random rooted binary tree with `n` leaves.
 *  Leaves are numbered 1..n. Internal nodes are numbered n+1 .. 2*n-1.
 *  The function sets T->n, T->m, allocates arrays via initialize_tree,
 *  fills first_child/next_sibling/parent/height and sets T->root.
 *  Each internal node's height is strictly greater than its children's heights.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "../../src/common/list.h"
#include "../../src/common/tree.h"

static char *xstrdup(const char *s) {
    if (s == NULL) return NULL;
    size_t len = strlen(s) + 1;
    char *copy = malloc(len);
    if (copy != NULL) {
        memcpy(copy, s, len);
    }
    return copy;
}

void random_bintree(int n_leaves, int seed, Tree *T){
	if(n_leaves < 1) return;

	int total_nodes = 2 * n_leaves - 1;
	int total_edges = total_nodes - 1;

	T->n = total_nodes;
	T->m = total_edges;
	initialize_tree(T, T->n, T->m);

	/* initialize leaves */
	for(int i = 1; i <= n_leaves; ++i){
		T->first_child[i] = 0;
		T->next_sibling[i] = 0;
		T->parent[i] = 0;
		T->height[i] = 0.0; /* leaves at height 0 */
		T->numleaves[i] = 1;
		T->pair[i] = 0;
	}
	for(int i = n_leaves + 1; i <= total_nodes; ++i){
		T->first_child[i] = 0;
		T->next_sibling[i] = 0;
		T->parent[i] = 0;
		T->height[i] = 0.0;
		T->numleaves[i] = 0;
		T->pair[i] = 0;
	}

	/* prepare list of active nodes */
	List U;
	initialize(&U);
	for(int i = 1; i <= n_leaves; ++i) insertBefore(U.tail, i);

	/* random seed */
	if(seed == 0) seed = (int)time(NULL);
	srand(seed);

	int remaining = n_leaves;
	int next_internal = n_leaves + 1;

	while(remaining > 1){
		/* pick two distinct random positions in U */
		int size = remaining;
		int idx1 = rand() % size;
		int idx2 = rand() % (size - 1);
		if(idx2 >= idx1) idx2++;

		/* find nodes at idx1 and idx2 (0-based) */
		Node *p1 = U.head->next;
		for(int i = 0; i < idx1; ++i) p1 = p1->next;
		Node *p2 = U.head->next;
		for(int i = 0; i < idx2; ++i) p2 = p2->next;

		int child1 = p1->value;
		int child2 = p2->value;

		int internal = next_internal++;

		/* attach children to internal (order preserved as chosen) */
		T->first_child[internal] = child1;
		T->next_sibling[child1] = child2;
		T->next_sibling[child2] = 0;
		T->parent[child1] = internal;
		T->parent[child2] = internal;

		/* pick height: strictly greater than children's heights */
		double h1 = T->height[child1];
		double h2 = T->height[child2];
		double base = (h1 > h2) ? h1 : h2;
		/* add a positive random offset in (0,1] */
		double offset = (rand() / (double)RAND_MAX) + 1e-6;
		T->height[internal] = base + offset;

		/* replace the two children in U with the new internal node */
		/* erase the node with larger index first to keep pointers valid */
		if(idx1 > idx2){
			erase(p1);
			erase(p2);
		} else {
			erase(p2);
			erase(p1);
		}
		insertBefore(U.tail, internal);

		remaining = remaining - 1; /* two removed, one added */
	}

	/* remaining single node is the root */
	int root = U.head->next->value;
	T->root = root;

	/* cleanup temporary list */
	while(U.head->next != U.tail) erase(U.tail->prev);
	free(U.head);
	free(U.tail);
}


void perturbed_ultrametric(Tree *T, Matrix *M, double rho, int seed) {
	int n_leaves = (T->n + 1) / 2;

	/* initialize output matrix */
	init_matrix(M, n_leaves);

	/* prepare LCA matrix and compute LCA for leaves */
	IntMatrix LCA;
	init_matrix2(&LCA, n_leaves);
	make_lca(*T, &LCA);

	/* seed RNG using provided seed (if seed==0 use time) */
	if(seed == 0) seed = (int)time(NULL);
	srand((unsigned int)seed);

	/* build ultrametric U(x,y) = 2*height(lca(x,y)) and apply independent perturbations */
	for(int i = 1; i <= n_leaves; ++i){
		M->cpn[i][i] = 0.0;
		for(int j = i + 1; j <= n_leaves; ++j){
			int lca = LCA.cpn[i][j];
			double U = 2.0 * T->height[lca];
			double eta = (rand() / (double)RAND_MAX) * rho; /* 0 <= eta <= rho */
			double val = (1.0 - eta) * U;
			M->cpn[i][j] = M->cpn[j][i] = val;
		}
	}

	del_matrix2(&LCA);
}
int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <n_leaves> <rho> <seed> <output.phylip>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int n_leaves = atoi(argv[1]);
    double rho = atof(argv[2]);
    int seed = atoi(argv[3]);
    const char *outfile = argv[4];

    if (n_leaves < 2) {
        fprintf(stderr, "Error: n_leaves must be at least 2.\n");
        return EXIT_FAILURE;
    }
   	if (rho < 0.0 || rho >= 1.0) {
   		fprintf(stderr, "Error: rho must satisfy 0 <= rho < 1.\n");
    	return EXIT_FAILURE;
	}

    Tree T;
    Matrix M;

    random_bintree(n_leaves, seed, &T);
    perturbed_ultrametric(&T, &M, rho, seed+1);

    for (int i = 1; i <= n_leaves; i++) {
        char label[32];
        snprintf(label, sizeof(label), "Seq%d", i);
        M.labels[i] = xstrdup(label);
        if (M.labels[i] == NULL) {
            fprintf(stderr, "Error: strdup failed for label %d.\n", i);
            del_matrix(&M);
            terminate_tree(&T);
            return EXIT_FAILURE;
        }
    }

    write_matrix_phylip(outfile, &M);
    del_matrix(&M);
    terminate_tree(&T);

    return EXIT_SUCCESS;
}