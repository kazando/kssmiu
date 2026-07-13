#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "tree.h"
#include "matrix.h"
#include "stack.h"

/* 初期化, メモリ領域確保 */
void initialize_tree(Tree *T, int n, int m) {
    int i;

	T->first_child = (int *)malloc((n + 1) * sizeof(int));
     if(T->first_child == NULL) {
		fprintf(stderr, "initialize_tree(): malloc error [T->first_child]");
         exit(EXIT_FAILURE);
     }
     
     T->next_sibling = (int *)malloc((n + 1) * sizeof(int));
     if(T->next_sibling == NULL) {
		fprintf(stderr, "initialize_tree(): malloc error [T->next_sibling]");
         exit(EXIT_FAILURE);
     }
     
     T->height = (double *)malloc((n + 1) * sizeof(double));
     if(T->height == NULL) {
         fprintf(stderr, "initialize_tree(): malloc error [T->height]");
         exit(EXIT_FAILURE);
     }
     
     T->parent = (int *)malloc((n + 1) * sizeof(int));
     if(T->parent == NULL) {
         fprintf(stderr, "initialize_tree(): malloc error [T->parent]");
         exit(EXIT_FAILURE);
     }	
     
     T->pair = (int *)malloc((n + 1) * sizeof(int));
     if(T->pair == NULL) {
         fprintf(stderr, "initialize_tree(): malloc error [T->pair]");
         exit(EXIT_FAILURE);
     }
     
     T->numleaves = (int *)malloc((n + 1) * sizeof(int));
     if(T->numleaves == NULL) {
         fprintf(stderr, "initialize_tree(): malloc error [T->numleaves]");
         exit(EXIT_FAILURE);
     }
 
	for (i = 1; i <= T->n; i++) T->first_child[i] = 0;
	for (i = 1; i <= T->n; i++) T->next_sibling[i] = 0;
 }
 
 void print_height(Tree T) {
    int i;

    for (i = 1; i <= T.n; i++) {
        printf("h(%d):%f\n", i, T.height[i]);
    }
}

void print_tree(Tree T) {
     int j;
     printf("\n-------- Tree Edges --------\n");
     printf("Number of vertices: %d\n", T.n);
     printf("Number of edges: %d\n", T.m);
     printf(" -------------------\n");
	for (j = T.n/2+2; j <= T.n; j++){
	  printf("%d: (%d,%d)\n", j, T.first_child[j], T.next_sibling[T.first_child[j]]);
	}
     //for(i = 1; i <= T.n; i++) printf("[%d]bro---->%d\n",i, T.bro[i]);
 }
 
 /* メモリ領域解放 */
 void terminate_tree(Tree *T) {
	if (T->first_child != NULL) free(T->first_child);
	if (T->next_sibling != NULL) free(T->next_sibling);
     if (T->height != NULL) free(T->height);
     if (T->parent != NULL) free(T->parent);
	if (T->pair != NULL) free(T->pair);
     if (T->numleaves != NULL) free(T->numleaves);
 }
 
 void make_parent(Tree *T){
     int i;	
     for(i = (T->n + 1) / 2 +  1; i <= T->n; i++){
		T->parent[T->first_child[i]] = i;
		T->parent[T->next_sibling[T->first_child[i]]] = i;
     }
     T->parent[T->root] = 0;	
 }
 
 void make_brother(Tree *T){
     int i;	
     for(i = 1; i <= T->n; i++){
         if(T->first_child[i] == 0) continue;
		T->pair[T->first_child[i]] = T->next_sibling[T->first_child[i]];
		T->pair[T->next_sibling[T->first_child[i]]] = T->first_child[i];
     }
	T->pair[T->root] = 0;
 }
 
 void make_numleaves(Tree *T, int num){
     int i, t;
     for(i = 1; i <= num; i++) T->numleaves[i] = 1;
     for(i = num + 1; i <= T->n; i++) T->numleaves[i] = 0; 
     for(i = 1; i <= num; i++){
         t = T->parent[i];
         while(t != 0){
             T->numleaves[t]++;
             t = T->parent[t];
         }
     }
 }
 
 void cpy_tree(Tree T, Tree *Tmp){
     int i;	
     Tmp->n = T.n;
     Tmp->m = T.m;
     Tmp->root = T.root;
     for(i = 1; i <= T.n; i++){
		Tmp->first_child[i] = T.first_child[i];
		Tmp->next_sibling[i] = T.next_sibling[i];
		Tmp->parent[i] = T.parent[i];
		Tmp->height[i] = T.height[i];
     }	
 }

/* 内部関数：部分木を距離付き Newick 形式の文字列に変換 */
static void write_newick_subtree_with_height(Matrix *M, Tree *T, int node, FILE *fp) {
    // 内部ノードは括弧付きで子を出力
    if (node > T->n/2+1) {
        fprintf(fp, "(");
        // 最初の子
        int child = T->first_child[node];
        write_newick_subtree_with_height(M, T, child, fp);
        
        // 兄弟（2番目の子）
        int sibling = T->next_sibling[child];
        fprintf(fp, ",");
        write_newick_subtree_with_height(M, T, sibling, fp);
        
        // 閉じ括弧と自身のID、親までの距離
        if (node == T->root) {
            //fprintf(fp, ")%d:0.0", node);  // rootは距離 0
            fprintf(fp, "):0.0");  // rootは距離 0
        } else {
            //fprintf(fp, ")%d:%lf", node, T->height[T->parent[node]]-T->height[node]);
            fprintf(fp, "):%lf", T->height[T->parent[node]]-T->height[node]);
        }
    } else {
        // 葉ノードはIDと距離のみ
        fprintf(fp, "%s:%lf", M->labels[node], T->height[T->parent[node]]);
    }
}

/* Tree を距離付き Newick 形式でファイルに出力 */
void write_tree_newick_with_height(Matrix *M, Tree *T, const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open file %s for writing\n", filename);
        return;
    }
    
    write_newick_subtree_with_height(M, T, T->root, fp);
    fprintf(fp, ";\n");  // Newick 形式は ; で終わる
    
    fclose(fp);
}

/* Tree を距離付き Newick 形式で標準出力に出力（デバッグ用） */
void print_tree_newick_with_height(Matrix *M, Tree *T) {
    write_newick_subtree_with_height(M, T, T->root, stdout);
    printf(";\n");
}

void make_lca(Tree T, IntMatrix *LCA){
	Stack sta;
	int i, j, v, w, dp;
	int currentedge[(T.n)+1], label[T.n+1];
	int new_label[T.n + 1], last[T.n + 1], tmp_label[T.n + 1];
	int depth[T.n * 2 - 1], node[T.n * 2 - 1];
	int **table, **memo;
	int col, max_n, ex, tmp, log_tmp;
	int min_node, max_node, min;
	//int min_last, max_last;
	int n_num;
	int log_array[T.n * 2], ex_array[T.n * 2];
	max_n = T.n * 2 - 1;
	n_num = (T.n + 1) / 2;
	
	log_array[0] = 0;
	ex_array[0] = 0;
	ex = 1; log_tmp = 0;
	j = 2; 
	for(i = 1; i <= max_n; i++){
		if(i != j){
			log_array[i] = log_tmp;
			ex_array[i] = ex;
		}else{
			ex = ex * 2; log_tmp++;
			j = j * 2;
			log_array[i] = log_tmp;
			ex_array[i] = ex;
		}
	}
	
	////////新しく頂点にラベル付け
	sta.length = 0;
	sta.root = NULL;
	for(i = 1; i <= T.n; i++) {
		currentedge[i] = T.first_child[i];
		label[i] = 0;  
	}
	for(i = 0; i < T.n; i++) new_label[i] = 0;
	push(T.root, &sta);
	i = 1;
	j = 0; dp = 0;
	new_label[0] = T.root;
	tmp_label[T.root] = 0;
	label[T.root] = 1;
	depth[0] = 0;
	while(sta.length != 0){
		v = sta.root->data;
		node[j] = tmp_label[v];
		last[tmp_label[v]] = j;		
		if(currentedge[v] == 0){
			dp--;
			depth[j+1] = dp;		
			pop(&sta);	
		}else{
			w = currentedge[v];
			currentedge[v] = T.next_sibling[w];
			dp++;
		  depth[j+1] = dp;			
			if(label[w] == 0){
				push(w,&sta);
				new_label[i] = w;
				tmp_label[w] = i;
				label[w] = 1;
				i++;
			}
		}
		j++;
	}
	//for(i = 0; i < T.n; i++) printf("%d ",new_label[i]); putchar('\n');
	//for(i = 1; i <= T.n; i++) printf("%d ",tmp_label[i]); putchar('\n');
	//printf("depth : ");	for(i = 0; i < max_n; i++) printf("%d ", depth[i]); putchar('\n');
	//printf("node : ");	for(i = 0; i < max_n; i++) printf("%d ", node[i]); putchar('\n');	
	
	////// RMQ Tableをつくる.
	col = log_array[max_n];
	col++;
	table	= (int **)malloc(col * sizeof(int *));
	for(i = 0; i < col; i++){
		table[i] = (int *)malloc(max_n * sizeof(int));
	}

	memo	= (int **)malloc(col * sizeof(int *));
	for(i = 0; i < col; i++){
		memo[i] = (int *)malloc(max_n * sizeof(int));
	}	
	
	for(i = 0; i < col; i++){
		for(j = 0; j < max_n; j++){
			table[i][j] = -1;
			memo[i][j] = -1;
		}
	}

	for(j = 0; j < max_n; j++){
		table[0][j] = depth[j];
		memo[0][j] = j;
	}
	
	ex = 1;
	for(i = 1; i < col; i++){
		for(j = 0; j < max_n - 1; j++){
			tmp = table[i-1][j + ex];
			if(tmp == -1) break;
			table[i][j] = fmin(table[i-1][j], tmp);	
			if(table[i][j] == tmp) memo[i][j] = memo[i-1][j + ex];
			else memo[i][j] = memo[i-1][j];
		}
		ex = ex * 2;
	}

	/////////////LCAを求める.
	for(i = 1; i < n_num; i++){
		for(j = i + 1; j <= n_num; j++){
			min_node = fmin(last[tmp_label[i]], last[tmp_label[j]]);
			if(min_node == last[tmp_label[i]]) max_node = last[tmp_label[j]];
			else max_node = last[tmp_label[i]];
			tmp = max_node - min_node + 1;
			log_tmp = log_array[tmp];
			ex = ex_array[tmp];
			max_node = max_node - ex + 1;
			min = fmin(table[log_tmp][min_node], table[log_tmp][max_node]);
			if(min == table[log_tmp][min_node]){
				 LCA->cpn[i][j] = new_label[node[memo[log_tmp][min_node]]]; 
				 LCA->cpn[j][i] = new_label[node[memo[log_tmp][min_node]]];
			}else{
				 LCA->cpn[i][j] = new_label[node[memo[log_tmp][max_node]]];
				 LCA->cpn[j][i] = new_label[node[memo[log_tmp][max_node]]];
			}
		}
	}
	for(i = 1; i <= n_num; i++) LCA->cpn[i][i] = i;

/*
	putchar('\n');	
	for(i = 1; i <= n_num; i++){
		for(j = 1; j <= n_num; j++){
			printf("%d ",LCA->cpn[i][j]);
		}
		putchar('\n');
	}
*/	
	for(i = 0; i < col; i++){
		free(table[i]);
		free(memo[i]);
	}
	free(table);
	free(memo);
}

void post_order(Tree *T, List *order){
    Stack sta;
    int i, v, w;
    int currentedge[(T->n)+1];
    int label[T->n+1];
    Node *rt;
    
    rt = order->head->next;
    sta.length = 0;
    sta.root = NULL;

    for(i = 1; i <= T->n; i++) {
        currentedge[i] = T->first_child[i]; 
        label[i] = 0; 
    }

    push(T->root, &sta);
    i = 1;
    label[T->root] = i;

    while(sta.length != 0){
        v = sta.root->data;
        if(currentedge[v] == 0){
            pop(&sta);
            rt->value = v;
            rt = rt->next;			
        }else{
            w = currentedge[v];
            currentedge[v] = T->next_sibling[w];
            if(label[w] == 0){
                push(w,&sta);
                i++;
                label[w] = i;
            }
        }
    }
}

void pre_order(Tree *T, List *order2){
    Stack sta;
    int i, v, w;
    int currentedge[(T->n)+1];
    int label[(T->n)+1];
    Node *rt;
    
    rt = order2->head->next;
    sta.length = 0;
    sta.root = NULL;

    for(i = 1; i <= T->n; i++) {
        currentedge[i] = T->first_child[i]; 
        label[i] = 0; 
    }

    push(T->root, &sta);
    label[T->root] = 1;
    rt->value = T->root;
    rt = rt->next;

    while(sta.length != 0){
        v = sta.root->data;
        if(currentedge[v] == 0){
            pop(&sta);	
        }else{
            w = currentedge[v];
            currentedge[v] = T->next_sibling[w];
            if(label[w] == 0){
                rt->value = w;
                rt = rt->next;
                push(w,&sta);
                label[w] = 1;
            }
        }
    }
}