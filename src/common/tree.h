#ifndef _TREE
#define _TREE
#define N_MAX 30000
#define M_MAX 200000
#include <math.h>
#include "list.h"
#include "matrix.h"

typedef struct {
  int n;	//頂点の数
  int m;	//枝の数
  /* 子ノード表現: first_child はその頂点の最初の子（なければ0）.
     next_sibling は子ノードの兄弟リンク（右隣の子, なければ0）.
     pair は同じ親を持つもう一方の子への双方向マッピング（元コードの bro）*/
  int *first_child;
  int *next_sibling;
  double *height;
  int *parent;
  int root;
  int *numleaves;
  int *pair;
} Tree;

void initialize_tree(Tree *T, int n, int m);
void print_height(Tree T);
void print_tree(Tree T);
void construct_adjacency_list(Tree *Tp);
void construct_adjacency_list_tree(Tree *Tp);
void terminate_tree(Tree *T);
void post_order(Tree *T, List *order);
void pre_order(Tree *T, List *order2);
void make_lca(Tree T, IntMatrix *LCA);
void make_parent(Tree *T);
void make_brother(Tree *T);
void make_numleaves(Tree *T, int num);
void cpy_tree(Tree T, Tree *Tmp);

void write_tree_newick_with_height(Matrix *M, Tree *T, const char *filename); 
void print_tree_newick_with_height(Matrix *M, Tree *T);


#endif
