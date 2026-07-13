#ifndef _kSS
#define _kSS
#define _POSIX_C_SOURCE 200809L
#include "../common/list.h"
#include "../common/matrix.h"
#include "../common/tree.h"
#include <time.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>

void algo_ope(Matrix M, Matrix *X, Matrix *A, Matrix *K, IntMatrix *LCA, Tree *T, List *order, List *order2, List *Renew, int k, char* filename);
double calcurate_norm(Tree T, Matrix K, double* max, int w1, int v1, int k, int* best_lca, int* best_left, int* best_right, int **visited_mark, int* cn, int *kin);
void algo_ope2(Matrix M, Matrix *X, Matrix *A, Matrix *K, Matrix *H, IntMatrix *LCA, Tree *T, List *order,  List *order2, List *Renew, int k, char *name);
double calcurate_norm2(Tree T, Matrix K, Matrix H, double* max, int w1, int v1, int k, int* best_lca, int* best_left, int* best_right, int **visited_mark, int* cn, int *kin);
void mutt(Matrix M, Matrix K, Tree *T, List *order);
void make_xlist(Tree T, Matrix *X, IntMatrix LCA);
void make_ancestor(Tree T, Matrix *A);
void make_distance(Tree T, Matrix M, Matrix A, Matrix *K, List *order);
void make_distance2(Tree T, Matrix M, Matrix A, Matrix *K, List *order2);
double error(Matrix M, Matrix X);
double error2(Matrix M, Matrix X);
void make_sum(Tree T, Matrix M, Matrix A, Matrix *H, List *order);
void check_matrix(Matrix M, Matrix X);
double diffUserTime(struct rusage *start, struct rusage *end);
double diffUserTime2(struct timespec *tsStart, struct timespec *tsEnd);
#endif
