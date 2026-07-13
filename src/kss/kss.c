#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/resource.h>
#include <unistd.h>
#include "../common/stack.h"
#include "kss.h"

//#ifdef TEST
//#define TEST_MESSAGE(s) printf("by %s\n",s)
//#else
//#define TEST_MESSAGE(s)
//#endif /* TEST */

void algo_ope(Matrix M, Matrix *X, Matrix *A, Matrix *K, IntMatrix *LCA, Tree *T, List *order, List *order2, List *Renew, int k, char* filename){
	int i, j, v, w, v1, v2, w1, w2, u, best_lca = 0, best_left = 0, best_right = 0;
	double max;
	int swapped, tmp_node, tmp_bro;
	int c, t;
	int max_w1, max_w2, max_v1, max_v2, tmp_w2;
	int currentedge[T->n + 1], label[T->n + 1];
	int count, cn;
	double sum1, sum2, improve=0;
	int **visited_mark;
	int kin;
	Stack sta;
	Node* rt;
	Node* rt2;
	clock_t start, end, start2, end2;
	double time_sum = 0.0;
	start = clock();
	
	post_order(T, order);
	pre_order(T, order2);
	make_ancestor(*T, A);
	//show_matrix(*A);
	make_distance(*T, M, *A, K, order);
	make_distance2(*T, M, *A, K, order2);
	//show_matrix(M);
	//show_matrix(*K);
	//print_list(*order2);

	mutt(M, *K, T, order);
	make_lca(*T, LCA);
	make_xlist(*T, X, *LCA);

	////////////////////////////////////////
	sum1 = error(M, *X);
	//print_height(*T);
	//print_tree(*T);
	count = 0;
	swapped = 1;
	visited_mark = (int **)malloc(sizeof(int *)*(T->n+1));
	for(i = 1;i <= T->n;i++){
		visited_mark[i] = (int *)malloc(sizeof(int) * (T->n+1) );
	}
	for(i = 1; i <= T->n; i++)
		for(j = 1; j <= T->n; j++)
			visited_mark[i][j] = visited_mark[j][i] = 0;
	cn = 0;
	kin = 0;
	//show_matrix(*A);
	while(swapped == 1){
		start2 = clock();
		max = 0; swapped = 0;
		//getrusage(RUSAGE_SELF, &start);
		//timespec_get(&tsStart, TIME_UTC);
		for(i = 1; i < T->n; i++){
			t = T->parent[i];
			tmp_w2 =	calcurate_norm(*T, *K, &max, i, t, k, &best_lca, &best_left, &best_right, visited_mark, &cn, &kin);
			//printf("tmp_w2 : %d\n", tmp_w2);
			if(tmp_w2 != -1){
				max_w1 = i; max_v1 = t;
				max_w2 = tmp_w2; max_v2 = T->parent[max_w2];
			}
		}
		//getrusage(RUSAGE_SELF, &end);
		//timespec_get(&tsEnd, TIME_UTC);
		//printf("  CPU time: %.06f sec\n", diffUserTime(&start, &end));
		//printf("  CPU time: %lf sec\n", diffUserTime2(&tsStart, &tsEnd));
		//fprintf(fp6, "%lf\n", diffUserTime2(&tsStart, &tsEnd));
		//fprintf(fp6, "%lf\n", diffUserTime(&start, &end));
		cn++;
			
		if(max > 0.0000001){
			v1 = max_v1; v2 = max_v2;
			w1 = max_w1; w2 = max_w2;
			u = T->pair[w1]; c = T->pair[w2];
			//NNI-operate
			T->parent[w2] = v1;
			T->parent[w1] = v2;
			T->pair[c] = w1;
			T->pair[w1] = c;
			T->pair[u] = w2;
			T->pair[w2] = u;
			if(T->first_child[v1] == w1){
				T->first_child[v1] = w2;
				T->next_sibling[w2] = u;
				T->next_sibling[w1] = 0;
			}else{
				T->next_sibling[u] = w2;
				T->next_sibling[w2] = 0;
			}
			if(T->first_child[v2] == c){
				T->next_sibling[c] = w1;
				T->next_sibling[w1] = 0;
			}else{
				T->first_child[v2] = w1;
				T->next_sibling[w1] = c;
			}
			//swapped = 1
			swapped = 1;
			count++;
			//numleavesの更新
			tmp_node = v2;
			while(tmp_node != best_lca){
				T->numleaves[tmp_node] = T->numleaves[tmp_node] - T->numleaves[w2] + T->numleaves[w1];
				tmp_node = T->parent[tmp_node];
			}
			tmp_node = v1;
			while(tmp_node != best_lca){
				T->numleaves[tmp_node] = T->numleaves[tmp_node] - T->numleaves[w1] + T->numleaves[w2];
				tmp_node = T->parent[tmp_node];
			}
			
			//先祖かどうか(A)の更新 Ver.2
			sta.length = 0;
			sta.root = NULL;
			for(i = 1; i <= T->n; i++) {
				currentedge[i] = T->first_child[i]; 
				label[i] = 0; 
			}
			push(w1, &sta);
			label[w1] = 1;
			while(sta.length != 0){
				v = sta.root->data;
				if(currentedge[v] == 0){
					tmp_node = v2;
					while(tmp_node != best_lca){			
						A->cpn[v][tmp_node] = 1;
						A->cpn[tmp_node][v] = 0;
						tmp_node = T->parent[tmp_node];
					}
					tmp_node = v1;
					while(tmp_node != best_lca){			
						A->cpn[v][tmp_node] = 0;
						A->cpn[tmp_node][v] = 0;
						tmp_node = T->parent[tmp_node];
					}					
					pop(&sta);			
				}else{
					w = currentedge[v];
					currentedge[v] = T->next_sibling[w];
					if(label[w] == 0){
						push(w,&sta);
						label[w] = 1;
					}
				}
			}
			sta.length = 0;
			sta.root = NULL;
			for(i = 1; i <= T->n; i++) {
				currentedge[i] = T->first_child[i]; 
				label[i] = 0; 
			}
			push(w2, &sta);
			label[w2] = 1;
			while(sta.length != 0){
				v = sta.root->data;
				if(currentedge[v] == 0){
					tmp_node = v1;
					while(tmp_node != best_lca){			
						A->cpn[v][tmp_node] = 1;
						A->cpn[tmp_node][v] = 0;
						tmp_node = T->parent[tmp_node];
					}
					tmp_node = v2;
					while(tmp_node != best_lca){			
						A->cpn[v][tmp_node] = 0;
						A->cpn[tmp_node][v] = 0;
						tmp_node = T->parent[tmp_node];
					}					
					pop(&sta);			
				}else{
					w = currentedge[v];
					currentedge[v] = T->next_sibling[w];
					if(label[w] == 0){
						push(w,&sta);
						label[w] = 1;
					}
				}
			}
			
			//更新する点のリストRenewとorderの更新
			tmp_node = v1;
			while(tmp_node != best_lca){
				insertBefore(Renew->tail, tmp_node);
				tmp_node = T->parent[tmp_node];
			}
			tmp_node = v2;
			while(tmp_node != best_lca){
				insertBefore(Renew->tail, tmp_node);
				tmp_node = T->parent[tmp_node];
			}
			insertBefore(Renew->tail, best_lca);
			post_order(T, order);
			//pre_order(T, order2);
			//print_list(*Renew);

			//部分木間距離(K)の更新
			for(rt2 = Renew->head->next; rt2->value != best_lca; rt2 = rt2->next){
				for(rt = order->head->next; rt->next != NULL; rt = rt->next){
					if(rt->value == rt2->value) continue;
					if(A->cpn[rt->value][rt2->value] == 0 && A->cpn[rt2->value][rt->value] == 0) {
						if(rt->value <= M.n) K->cpn[rt->value][rt2->value] = K->cpn[rt2->value][rt->value] = fmax(K->cpn[T->first_child[rt2->value]][rt->value],K->cpn[T->next_sibling[T->first_child[rt2->value]]][rt->value]);
						else K->cpn[rt->value][rt2->value] = K->cpn[rt2->value][rt->value] = fmax(K->cpn[rt2->value][T->first_child[rt->value]],K->cpn[rt2->value][T->next_sibling[T->first_child[rt->value]]]);
					}
				}	
			}
			///////////
			//Kの更新2
			///////////////
			tmp_node = T->parent[best_lca];
			while(tmp_node != 0){
				if(A->cpn[T->first_child[tmp_node]][best_lca] == 0 && A->cpn[best_lca][T->first_child[tmp_node]]== 0) tmp_bro = T->first_child[tmp_node];
				else tmp_bro = T->next_sibling[T->first_child[tmp_node]];
				for(rt2 = Renew->tail->prev; rt2->prev != 0; rt2 = rt2->prev){
					K->cpn[rt2->value][tmp_node] = fmax(K->cpn[tmp_node][T->parent[rt2->value]], K->cpn[T->pair[rt2->value]][tmp_bro]);
					K->cpn[tmp_node][rt2->value] = K->cpn[rt2->value][tmp_node];
				}
				tmp_node = T->parent[tmp_node];
			}
			for(rt2 = Renew->tail->prev; rt2->prev != 0; rt2 = rt2->prev){
				if(rt2->value <= M.n) continue;
				sta.length = 0;
				sta.root = NULL;
				for(i = 1; i <= T->n; i++) {
					currentedge[i] = T->first_child[i]; 
					label[i] = 0; 
				}
				push(rt2->value, &sta);
				label[rt2->value] = 1;

				while(sta.length != 0){
					v = sta.root->data;
					if(currentedge[v] == 0){
						pop(&sta);	
					}else{
						w = currentedge[v];
						currentedge[v] = T->next_sibling[w];
						if(label[w] == 0){
							push(w,&sta);
							label[w] = 1;
							if(A->cpn[w][T->first_child[rt2->value]] != 1 && A->cpn[T->first_child[rt2->value]][w] != 1) tmp_bro = T->first_child[rt2->value];
							else tmp_bro = T->next_sibling[T->first_child[rt2->value]];
							K->cpn[w][rt2->value]  = fmax(K->cpn[rt2->value][T->parent[w]], K->cpn[T->pair[w]][tmp_bro]);
							K->cpn[rt2->value][w] = K->cpn[w][rt2->value];
						}
					}
				}
			
			}
			//hの更新
			for(rt2 = Renew->head->next; rt2->value != best_lca; rt2 = rt2->next){
				T->height[rt2->value] = fmax(fmax(T->height[T->first_child[rt2->value]], T->height[T->next_sibling[T->first_child[rt2->value]]]), K->cpn[T->first_child[rt2->value]][T->next_sibling[T->first_child[rt2->value]]]);
			}
			//Renew削除
			while(Renew->head->next != Renew->tail) erase(Renew->tail->prev);	
	
		}
	  	end2 = clock();
	  	time_sum += (double)(end2-start2)/CLOCKS_PER_SEC * 1000;
	  	//printf("%f\n",(double)(end2-start2)/1000);	
	  	//printf("%f\n",(double)(end2-start2)/CLOCKS_PER_SEC*1000000);	
	  	//sum_t += (end - start);
		//printf("%.4f,  %d\n",(double)(end2-start2)/CLOCKS_PER_SEC, k_kazu);
	}
	//printf("%f\n", (cpu_time()*1000)/count);
	//fprintf(fp6, "%f\n",(e_time())/count);
	//printf("%f\n",(double)(sum_t)/(CLOCKS_PER_SEC/1000)/count);
	
	end = clock();
	make_lca(*T, LCA);
	make_xlist(*T, X, *LCA);
	sum2 = error(M, *X);
	improve =(sum1-sum2)/sum1 * 100;
	//printf("improve : %f\n", improve);
	//check_matrix(M, *X);
	//printf("%.03f\n",improve);
	//printf("%d\n", count);
	//printf("%.4f\n",(double)(end-start)/CLOCKS_PER_SEC);	
	//printf("Swapped %d times.\n",count);	
	printf(
		"input: %s, k: %d, totaltime [s]: %f, "
		"#improving moves: %d, #neighborhood scans: %d, "
		"time/iter [ms]: %f, ave size neighbor: %f, "
		"init. obj: %f, obj: %f, impr. rate [%%]: %f\n",
		filename,k,(double)(end-start)/CLOCKS_PER_SEC,
		count,cn,
		time_sum/(double)cn,(double)kin/cn, 
		sum1, sum2, improve
	);
	for(i = 1;i <= T->n;i++) free(visited_mark[i]);
	free(visited_mark);
}

double calcurate_norm(Tree T, Matrix K, double* max, int w1, int v1, int k, int* best_lca, int* best_left, int* best_right, int **visited_mark, int* cn, int *kin){
	int i, v2, w2, t, s, v, a, u, left, right, g, dist2, best_w2, ch;
	double tmp_height;
	double norm1, norm2, norm3, norm;
	int tmp_numleaves, dist, move_node, move_node2, move_node3, tmp, tmp_ko, tmp_numleaves_l, tmp_numleaves_r;
	double tmp_K;	
	int currentedge[T.n + 1];
	Stack sta;

	ch = 0;
	t = v1;
	g = w1;
	dist2 = 0;
	g = t;
	t = T.parent[t];
	dist2++;
	while(t != 0){
			dist = dist2;
			s = T.pair[g];
			//printf("t:%d, g:%d, s:%d\n",t,g, s);
			sta.length = 0;
			sta.root = NULL;
			push(s, &sta);
			if (s > (T.n+1)/2){currentedge[s] = 1;
			}else currentedge[s] = 3;
			if(dist == k) currentedge[s] = 3;
			while(sta.length != 0){
				v = sta.root->data;
				if(currentedge[v] == 3){
					norm = norm1 = norm2 = norm3 = 0;
					w2 = v;
					v2 = T.parent[w2];
					if(visited_mark[w1][w2] == *cn + 1){
						pop(&sta);
						dist--;	
						continue;
					}
					visited_mark[w1][w2] = visited_mark[w2][w1] = *cn + 1;
					if(v1 == v2) continue;
					(*kin)++;
					//printf("(%d,%d)|(%d,%d), %d, %d : %d\n",v1,w1,v2,w2, g, t, *cn);
					u = T.pair[w2];
					move_node = v2;
					move_node2 = w2;
					move_node3 = T.pair[move_node2];
					tmp_height = T.height[w1];		
					while(move_node != t){
						tmp_numleaves = T.numleaves[move_node2] - T.numleaves[w2] + T.numleaves[w1];
						tmp_K = fmax(K.cpn[w2][move_node], K.cpn[w1][move_node3]);
						tmp_height = fmax(fmax(tmp_height, T.height[move_node3]), tmp_K);
						norm1 += (T.numleaves[move_node2] * T.numleaves[move_node3]) * T.height[move_node] - (tmp_numleaves * T.numleaves[move_node3]) * tmp_height;
						move_node2 = move_node;
						move_node3 = T.pair[move_node2];
						move_node = T.parent[move_node];
					}
					
					move_node = v1;
					move_node2 = w1;
					move_node3 = T.pair[move_node2];
					tmp_height = T.height[w2];
					while(move_node != t){
						tmp_numleaves = T.numleaves[move_node2] - T.numleaves[w1] + T.numleaves[w2];
						tmp_K = fmax(K.cpn[w1][move_node], K.cpn[w2][move_node3]);
						tmp_height = fmax(fmax(tmp_height, T.height[move_node3]), tmp_K);
						norm2 += (T.numleaves[move_node2] * T.numleaves[move_node3]) * T.height[move_node] - (tmp_numleaves * T.numleaves[move_node3]) * tmp_height;
		
						move_node2 = move_node;
						move_node3 = T.pair[move_node2];
						move_node = T.parent[move_node];						
				 	}
				 	left = g;
				 	right = T.pair[left];
				 	tmp_numleaves_l = T.numleaves[left] - T.numleaves[w1] + T.numleaves[w2];
				 	tmp_numleaves_r = T.numleaves[right] - T.numleaves[w2] + T.numleaves[w1];
				 	norm3 = (T.numleaves[left] * T.numleaves[right] - tmp_numleaves_l * tmp_numleaves_r) * T.height[t];
				 	norm = norm1 + norm2 + norm3;
				 	if(*max < norm){
				 		//printf("(v1,w1) (v2,w2) norm : (%d,%d) (%d,%d) %f\n",v1,w1,v2,w2,norm);
				 		best_w2 = w2;
				 		*max = norm;
				 		ch = 1;
				 		*best_lca = t;
				 		*best_left = left;
				 		*best_right = right;
				 	}
					pop(&sta);
					dist--;			
				}else{
					if(currentedge[v] == 1){
						a = T.first_child[v];
						currentedge[a] = 1;
						currentedge[v] = 2;
					}else{
						a = T.next_sibling[T.first_child[v]];
						currentedge[a] = 1;
						currentedge[v] = 3;						
					}
					if(dist < k && v > (T.n+1)/2){ //fixed if(dist <= k && v > (T.n+1)/2){
						push(a,&sta);
						dist++;
					}else currentedge[v] = 3;
				}
			}
			g = t;
			t = T.parent[t];
			dist2++;
			if(dist2 > k) break;
	}
	if(ch != 0) return best_w2;
	else return -1;

}

void mutt(Matrix M, Matrix K, Tree *T, List *order){
	int i, j;
	double max;
	Node *rt;
	
	for(rt = order->head->next; rt->next != NULL; rt = rt->next){
		if(rt->value <= M.n){
			T->height[rt->value] = 0;
		}else{
			max = fmax(T->height[T->first_child[rt->value]], T->height[T->next_sibling[T->first_child[rt->value]]]);
			max = fmax(max, K.cpn[T->first_child[rt->value]][T->next_sibling[T->first_child[rt->value]]]);
			T->height[rt->value] = max;
		}
	}
}

void make_xlist(Tree T, Matrix *X, IntMatrix LCA){
	int i, j;

	for(i = 1; i <= X->n; i++) X->cpn[i][i] = 0;
	for(i = 1; i <= X->n-1; i++){
		for(j = i + 1; j <= X->n; j++){
			X->cpn[i][j] = X->cpn[j][i] = 2 * T.height[LCA.cpn[i][j]];
		}
	}
}

void make_ancestor(Tree T, Matrix *A){
	int i, j;
	for(i = 1; i <= T.n; i++){
		A->cpn[i][i] = 1;
		j = T.parent[i];
		while(j != 0){
			A->cpn[i][j] = 1;
			A->cpn[j][i] = 0;
			j = T.parent[j];
		}
	}
}

void make_distance(Tree T, Matrix M, Matrix A, Matrix *K, List *order){
	int ch;
	double max;
	Node *rt, *rt2;
	
	for(rt = order->head->next; rt->next != NULL; rt = rt->next){
		ch = 0;
		for(rt2 = order->head->next; rt2->next != NULL; rt2 = rt2->next){
			if(rt2->value == rt->value){
				ch = 1;
				K->cpn[rt->value][rt2->value] = 0;
				continue;
			}
			if(ch == 1){
				if(A.cpn[rt->value][rt2->value] != 1 && A.cpn[rt2->value][rt->value] != 1){
					if(rt->value <= M.n && rt2->value <= M.n){
				  	K->cpn[rt->value][rt2->value] = K->cpn[rt2->value][rt->value] = M.cpn[rt->value][rt2->value]/2;
				  }else if(rt->value > M.n && rt2->value <= M.n){
						K->cpn[rt->value][rt2->value] = K->cpn[rt2->value][rt->value] = fmax(K->cpn[T.first_child[rt->value]][rt2->value],K->cpn[T.next_sibling[T.first_child[rt->value]]][rt2->value]);
					}else{
						K->cpn[rt->value][rt2->value] = K->cpn[rt2->value][rt->value] = fmax(K->cpn[rt->value][T.first_child[rt2->value]],K->cpn[rt->value][T.next_sibling[T.first_child[rt2->value]]]);
					}
				}else{
					K->cpn[rt->value][rt2->value] = K->cpn[rt2->value][rt->value] = 0;
				}
			}
		}
	}
}

void make_distance2(Tree T, Matrix M, Matrix A, Matrix *K, List *order2){
	int ch;
	double max;
	Stack sta;
	int i, v, w;
	int b;
	int currentedge[(T.n)+1];
	int label[T.n+1];
	Node *rt;
	
	for(rt = order2->head->next; rt->next != NULL; rt = rt->next){
		if(rt->value <= M.n) continue;
		
		sta.length = 0;
		sta.root = NULL;
		for(i = 1; i <= T.n; i++) {
			currentedge[i] = T.first_child[i]; 
			label[i] = 0; 
		}
		push(rt->value, &sta);
		label[rt->value] = 1;

		while(sta.length != 0){
			v = sta.root->data;
			if(currentedge[v] == 0){
				pop(&sta);	
			}else{
				w = currentedge[v];
				currentedge[v] = T.next_sibling[w];
				if(label[w] == 0){
					push(w,&sta);
					label[w] = 1;
					if(A.cpn[w][T.first_child[rt->value]] == 0 && A.cpn[T.first_child[rt->value]][w] ==0) b = T.first_child[rt->value];
					else b = T.next_sibling[T.first_child[rt->value]];
					K->cpn[w][rt->value]  = fmax(K->cpn[rt->value][T.parent[w]], K->cpn[T.pair[w]][b]);
					K->cpn[rt->value][w] = K->cpn[w][rt->value];

				}
			}
		}	

	}

}

double error(Matrix M, Matrix X){
	int i, j;
	double sum;
	
	sum = 0;
	for(i = 1; i < M.n; i++){
		for(j = i + 1; j <= M.n; j++){
			sum = sum + (X.cpn[i][j] - M.cpn[i][j]);
		}
	}
	
	return(sum);
}

void check_matrix(Matrix M, Matrix X){
	int i, j;
	
	for(i = 1; i < M.n; i++){
		for(j = i + 1; j <= M.n; j++){
			if(X.cpn[i][j] - M.cpn[i][j] < 0) printf("Matrix Error\n");
		}
	}
	
}	

void algo_ope2(Matrix M, Matrix *X, Matrix *A, Matrix *K, Matrix *H, IntMatrix *LCA, Tree *T, List *order, List *order2, List *Renew, int k, char *filename){
	int i, j, v, w, v1, v2, w1, w2, u, best_lca = 0, best_left = 0, best_right = 0;
	double max;
	int swapped, tmp_node, tmp_bro;
	int c, t;
	int max_w1, max_w2, max_v1, max_v2, tmp_w2;
	int currentedge[T->n + 1], label[T->n + 1];
	int count, cn;
	double sum1, sum2, improve=0;
	int **visited_mark;
	int kin;
	Stack sta;
	Node* rt;
	Node* rt2;
	clock_t start, end, start2, end2;
	double time_sum = 0.0;
	start = clock();
	
	post_order(T, order);
	pre_order(T, order2);
	make_ancestor(*T, A);
	//show_matrix(*A);
	make_distance(*T, M, *A, K, order);
	make_distance2(*T, M, *A, K, order2);
	make_sum(*T, M, *A, H, order);
	//show_matrix(*H);
	//show_matrix(*K);
	//print_list(*order);

	mutt(M, *K, T, order);
	make_lca(*T, LCA);
	make_xlist(*T, X, *LCA);

	////////////////////////////////////////
	sum1 = error2(M, *X);
	sum1 = sqrt(sum1);
	//print_height(*T);
	//print_tree(*T);
	count = 0;
	swapped = 1;
	visited_mark = (int **)malloc(sizeof(int *)*(T->n+1));
	for(i = 1;i <= T->n;i++){
		visited_mark[i] = (int *)malloc(sizeof(int) * (T->n+1) );
	}
	for(i = 1; i <= T->n; i++)
		for(j = 1; j <= T->n; j++)
			visited_mark[i][j] = visited_mark[j][i] = 0;
	cn = 0;
	kin = 0;
	//show_matrix(*A);
	while(swapped == 1){
		start2 = clock();
		max = 0; swapped = 0;
		//getrusage(RUSAGE_SELF, &start);
		//timespec_get(&tsStart, TIME_UTC);
		for(i = 1; i < T->n; i++){
			t = T->parent[i];
			tmp_w2 =	calcurate_norm2(*T, *K, *H, &max, i, t, k, &best_lca, &best_left, &best_right, visited_mark, &cn, &kin);
			//printf("tmp_w2 : %d\n", tmp_w2);
			if(tmp_w2 != -1){
				max_w1 = i; max_v1 = t;
				max_w2 = tmp_w2; max_v2 = T->parent[max_w2];
			}
		}
		//getrusage(RUSAGE_SELF, &end);
		//timespec_get(&tsEnd, TIME_UTC);
		//printf("  CPU time: %.06f sec\n", diffUserTime(&start, &end));
		//printf("  CPU time: %lf sec\n", diffUserTime2(&tsStart, &tsEnd));
		//fprintf(fp6, "%lf\n", diffUserTime2(&tsStart, &tsEnd));
		//fprintf(fp6, "%lf\n", diffUserTime(&start, &end));
		cn++;
			
		if(max > 0.000001){		
			v1 = max_v1; v2 = max_v2;
			w1 = max_w1; w2 = max_w2;
			//printf("%d : %f, (%d,%d):(%d,%d)\n",count, max, v1,w1,v2,w2);
			u = T->pair[w1]; c = T->pair[w2];		
			//NNI-operate
			T->parent[w2] = v1;
			T->parent[w1] = v2;
			T->pair[c] = w1;
			T->pair[w1] = c;
			T->pair[u] = w2;
			T->pair[w2] = u;
			if(T->first_child[v1] == w1){
				T->first_child[v1] = w2;
				T->next_sibling[w2] = u;
				T->next_sibling[w1] = 0;
			}else{
				T->next_sibling[u] = w2;
				T->next_sibling[w2] = 0;
			}
			if(T->first_child[v2] == c){
				T->next_sibling[c] = w1;
				T->next_sibling[w1] = 0;
			}else{
				T->first_child[v2] = w1;
				T->next_sibling[w1] = c;
			}
			//swapped = 1
			swapped = 1;
			count++;
			//numleavesの更新
			tmp_node = v2;
			while(tmp_node != best_lca){
				T->numleaves[tmp_node] = T->numleaves[tmp_node] - T->numleaves[w2] + T->numleaves[w1];
				tmp_node = T->parent[tmp_node];
			}
			tmp_node = v1;
			while(tmp_node != best_lca){
				T->numleaves[tmp_node] = T->numleaves[tmp_node] - T->numleaves[w1] + T->numleaves[w2];
				tmp_node = T->parent[tmp_node];
			}
			
			//先祖かどうか(A)の更新 Ver.2
			sta.length = 0;
			sta.root = NULL;
			for(i = 1; i <= T->n; i++) {
				currentedge[i] = T->first_child[i]; 
				label[i] = 0; 
			}
			push(w1, &sta);
			label[w1] = 1;
			while(sta.length != 0){
				v = sta.root->data;
				if(currentedge[v] == 0){
					tmp_node = v2;
					while(tmp_node != best_lca){			
						A->cpn[v][tmp_node] = 1;
						A->cpn[tmp_node][v] = 0;
						tmp_node = T->parent[tmp_node];
					}
					tmp_node = v1;
					while(tmp_node != best_lca){			
						A->cpn[v][tmp_node] = 0;
						A->cpn[tmp_node][v] = 0;
						tmp_node = T->parent[tmp_node];
					}					
					pop(&sta);			
				}else{
					w = currentedge[v];
					currentedge[v] = T->next_sibling[w];
					if(label[w] == 0){
						push(w,&sta);
						label[w] = 1;
					}
				}
			}
			sta.length = 0;
			sta.root = NULL;
			for(i = 1; i <= T->n; i++) {
				currentedge[i] = T->first_child[i]; 
				label[i] = 0; 
			}
			push(w2, &sta);
			label[w2] = 1;
			while(sta.length != 0){
				v = sta.root->data;
				if(currentedge[v] == 0){
					tmp_node = v1;
					while(tmp_node != best_lca){			
						A->cpn[v][tmp_node] = 1;
						A->cpn[tmp_node][v] = 0;
						tmp_node = T->parent[tmp_node];
					}
					tmp_node = v2;
					while(tmp_node != best_lca){			
						A->cpn[v][tmp_node] = 0;
						A->cpn[tmp_node][v] = 0;
						tmp_node = T->parent[tmp_node];
					}					
					pop(&sta);			
				}else{
					w = currentedge[v];
					currentedge[v] = T->next_sibling[w];
					if(label[w] == 0){
						push(w,&sta);
						label[w] = 1;
					}
				}
			}
			
			//更新する点のリストRenewとorderの更新
			tmp_node = v1;
			while(tmp_node != best_lca){
				insertBefore(Renew->tail, tmp_node);
				tmp_node = T->parent[tmp_node];
			}
			tmp_node = v2;
			while(tmp_node != best_lca){
				insertBefore(Renew->tail, tmp_node);
				tmp_node = T->parent[tmp_node];
			}
			insertBefore(Renew->tail, best_lca);
			
			post_order(T, order);
				
			//部分木間距離(K)の更新
			for(rt2 = Renew->head->next; rt2->value != best_lca; rt2 = rt2->next){
				for(rt = order->head->next; rt->next != NULL; rt = rt->next){
					if(rt->value == rt2->value) H->cpn[rt->value][rt2->value] = H->cpn[rt2->value][rt->value] = 0;
					else if(A->cpn[rt->value][rt2->value] != 1 && A->cpn[rt2->value][rt->value] != 1){
						if(rt->value <= M.n){
							K->cpn[rt->value][rt2->value] = K->cpn[rt2->value][rt->value] = fmax(K->cpn[T->first_child[rt2->value]][rt->value],K->cpn[T->next_sibling[T->first_child[rt2->value]]][rt->value]);
							H->cpn[rt->value][rt2->value] = H->cpn[rt2->value][rt->value] = H->cpn[T->first_child[rt2->value]][rt->value] + H->cpn[T->next_sibling[T->first_child[rt2->value]]][rt->value];
						}
						else{
							K->cpn[rt->value][rt2->value] = K->cpn[rt2->value][rt->value] = fmax(K->cpn[rt2->value][T->first_child[rt->value]],K->cpn[rt2->value][T->next_sibling[T->first_child[rt->value]]]);
							H->cpn[rt->value][rt2->value] = H->cpn[rt2->value][rt->value] = H->cpn[rt2->value][T->first_child[rt->value]] + H->cpn[rt2->value][T->next_sibling[T->first_child[rt->value]]];
						}
					}else{
						if(A->cpn[rt->value][rt2->value] == 1) H->cpn[rt->value][rt2->value] = H->cpn[rt2->value][rt->value] = H->cpn[T->first_child[rt2->value]][rt->value] + H->cpn[T->next_sibling[T->first_child[rt2->value]]][rt->value];
						else H->cpn[rt->value][rt2->value] = H->cpn[rt2->value][rt->value] = H->cpn[rt2->value][T->first_child[rt->value]] + H->cpn[rt2->value][T->next_sibling[T->first_child[rt->value]]];
					}
				}
			}

///////////
//Kの更新2
//////////

			tmp_node = T->parent[best_lca];
			while(tmp_node != 0){
				if(A->cpn[T->first_child[tmp_node]][best_lca] == 0 && A->cpn[best_lca][T->first_child[tmp_node]]== 0) tmp_bro = T->first_child[tmp_node];
				else tmp_bro = T->next_sibling[T->first_child[tmp_node]];
				for(rt2 = Renew->tail->prev; rt2->prev != 0; rt2 = rt2->prev){
					K->cpn[rt2->value][tmp_node] = fmax(K->cpn[tmp_node][T->parent[rt2->value]], K->cpn[T->pair[rt2->value]][tmp_bro]);
					K->cpn[tmp_node][rt2->value] = K->cpn[rt2->value][tmp_node];
				}
				tmp_node = T->parent[tmp_node];
			}
			for(rt2 = Renew->tail->prev; rt2->prev != 0; rt2 = rt2->prev){
				if(rt2->value <= M.n) continue;
				sta.length = 0;
				sta.root = NULL;
				for(i = 1; i <= T->n; i++) {
					currentedge[i] = T->first_child[i]; 
					label[i] = 0; 
				}
				push(rt2->value, &sta);
				label[rt2->value] = 1;

				while(sta.length != 0){
					v = sta.root->data;
					if(currentedge[v] == 0){
						pop(&sta);	
					}else{
						w = currentedge[v];
						currentedge[v] = T->next_sibling[w];
						if(label[w] == 0){
							push(w,&sta);
							label[w] = 1;
							if(A->cpn[w][T->first_child[rt2->value]] != 1 && A->cpn[T->first_child[rt2->value]][w] != 1) tmp_bro = T->first_child[rt2->value];
							else tmp_bro = T->next_sibling[T->first_child[rt2->value]];
							K->cpn[w][rt2->value]  = fmax(K->cpn[rt2->value][T->parent[w]], K->cpn[T->pair[w]][tmp_bro]);
							K->cpn[rt2->value][w] = K->cpn[w][rt2->value];
						}
					}
				}
			
			}
			
			
			//hの更新
			for(rt2 = Renew->head->next; rt2->value != best_lca; rt2 = rt2->next){
				T->height[rt2->value] = fmax(fmax(T->height[T->first_child[rt2->value]], T->height[T->next_sibling[T->first_child[rt2->value]]]), K->cpn[T->first_child[rt2->value]][T->next_sibling[T->first_child[rt2->value]]]);
			}
			
			//Renew削除
			while(Renew->head->next != Renew->tail) erase(Renew->tail->prev);	
			
		}
		end2 = clock();
	  	time_sum += (double)(end2-start2)/CLOCKS_PER_SEC * 1000;
	  	//fprintf(fp6, "%f\n",(double)(end2-start2)/1000);	
	  	//printf("%f\n",(double)(end2-start2)/CLOCKS_PER_SEC*1000000);	
	  	//sum_t += (end2 - start2);
		//printf("%.4f,  %d\n",(double)(end2-start2)/CLOCKS_PER_SEC, k_kazu);
	}
	
	end = clock();
	make_lca(*T, LCA);
	make_xlist(*T, X, *LCA);
	sum2 = error2(M, *X);
	sum2=sqrt(sum2);
	improve =(sum1-sum2)/sum1 * 100;
	//printf("improve : %f\n", improve);
	//check_matrix(M, *X);
	//printf("%.03f\n",improve);
	//printf("%d\n", count);
	//printf("%.4f\n",(double)(end-start)/CLOCKS_PER_SEC);	
	//printf("Swapped %d times.\n",count);	
	//printf("count: %d, cn: %d, sum1 : %f, sum2 : %f, improve rate: %f\n",count,cn,sum1, sum2, improve);

	printf(
		"input: %s, k: %d, totaltime [s]: %f, "
		"#improving moves: %d, #neighborhood scans: %d, "
		"time/iter [ms]: %f, ave size neighbor: %f, "
		"init. obj: %f, obj: %f, impr. rate [%%]: %f\n",
		filename,k,(double)(end-start)/CLOCKS_PER_SEC,
		count,cn,
		time_sum/(double)cn,(double)kin/cn, 
		sum1, sum2, improve
	);

	for(i = 1;i <= T->n;i++) free(visited_mark[i]);
	free(visited_mark);
}

double calcurate_norm2(Tree T, Matrix K, Matrix H, double* max, int w1, int v1, int k, int* best_lca, int* best_left, int* best_right, int **visited_mark, int* cn, int *kin){
	int i, v2, w2, t, s, v, a, u, left, right, g, dist2, best_w2, ch;
	double tmp_height;
	double norm1, norm2, norm3, norm;
	int tmp_numleaves, dist, move_node, move_node2, move_node3, tmp, tmp_ko, tmp_numleaves_l, tmp_numleaves_r;
	double tmp_K;
	double tmp_H;
	int currentedge[T.n + 1];
	Stack sta;
	
	ch = 0;
	t = v1;
	g = w1;
	dist2 = 0;
	g = t;
	t = T.parent[t];
	dist2++;
	while(t != 0){
			dist = dist2;
			s = T.pair[g];
			//printf("t:%d, g:%d, s:%d\n",t,g, s);
			sta.length = 0;
			sta.root = NULL;
			push(s, &sta);
			if (s > (T.n+1)/2){currentedge[s] = 1;
			}else currentedge[s] = 3;
			if(dist >= k) currentedge[s] = 3;
			while(sta.length != 0){
				v = sta.root->data;
				if(currentedge[v] == 3){
					norm = norm1 = norm2 = norm3 = 0;
					w2 = v;
					v2 = T.parent[w2];
					if(visited_mark[w1][w2] == *cn + 1){
						pop(&sta);
						dist--;	
						continue;
					}
					visited_mark[w1][w2] = visited_mark[w2][w1] = *cn + 1;
					if(v1 == v2) continue;
					(*kin)++;
					//printf("(%d,%d)|(%d,%d), %d, %d : %d\n",v1,w1,v2,w2, g, t, *cn);
					/////////////////////////v2からlcaへの道上の点について
					u = T.pair[w2];
					move_node = v2;
					move_node2 = w2;
					move_node3 = T.pair[move_node2];
					tmp_height = T.height[w1];		
					while(move_node != t){
						tmp_numleaves = T.numleaves[move_node2] - T.numleaves[w2] + T.numleaves[w1];
						tmp_K = fmax(K.cpn[w2][move_node], K.cpn[w1][move_node3]);
						tmp_H = H.cpn[move_node2][move_node3] - H.cpn[w2][move_node3] + H.cpn[w1][move_node3];
						tmp_height = fmax(fmax(tmp_height, T.height[move_node3]), tmp_K);
						norm1 += (T.numleaves[move_node2] * T.numleaves[move_node3]) * T.height[move_node] * T.height[move_node] - (tmp_numleaves * T.numleaves[move_node3]) * tmp_height * tmp_height;
						norm1 += tmp_H * tmp_height - H.cpn[move_node2][move_node3] * T.height[move_node];
						move_node2 = move_node;
						move_node3 = T.pair[move_node2];
						move_node = T.parent[move_node];
					}
					
					////////////////////////v1からlcaへの道上の点について
					move_node = v1;
					move_node2 = w1;
					move_node3 = T.pair[move_node2];
					tmp_height = T.height[w2];
					while(move_node != t){
						tmp_numleaves = T.numleaves[move_node2] - T.numleaves[w1] + T.numleaves[w2];
						tmp_K = fmax(K.cpn[w1][move_node], K.cpn[w2][move_node3]);
						tmp_H = H.cpn[move_node2][move_node3] - H.cpn[w1][move_node3] + H.cpn[w2][move_node3];
						tmp_height = fmax(fmax(tmp_height, T.height[move_node3]), tmp_K);
						norm2 += T.numleaves[move_node2] * T.numleaves[move_node3] * T.height[move_node] * T.height[move_node] - tmp_numleaves * T.numleaves[move_node3] * tmp_height * tmp_height;
						norm2 += tmp_H * tmp_height - H.cpn[move_node2][move_node3] * T.height[move_node];
						move_node2 = move_node;
						move_node3 = T.pair[move_node2];
						move_node = T.parent[move_node];						
				 	}
				 	///////////////////////////LCA
				 	left = g;
				 	right = T.pair[left];
				 	tmp_H = H.cpn[left][right] - H.cpn[w1][right] - H.cpn[w2][left] + H.cpn[w1][left] + H.cpn[w2][right] + 2*H.cpn[w1][w2];
				 	tmp_numleaves_l = T.numleaves[left] - T.numleaves[w1] + T.numleaves[w2];
				 	tmp_numleaves_r = T.numleaves[right] - T.numleaves[w2] + T.numleaves[w1];
				 	norm3 = (T.numleaves[left] * T.numleaves[right] - tmp_numleaves_l * tmp_numleaves_r) * T.height[t] * T.height[t];
				 	norm3 += (tmp_H-H.cpn[left][right]) * T.height[t];
				 	norm = norm1 + norm2 + norm3;
				 	if(*max < norm){
				 		//printf("(v1,w1) (v2,w2) norm : (%d,%d) (%d,%d) %f\n",v1,w1,v2,w2,norm);
				 		best_w2 = w2;
				 		*max = norm;
				 		ch = 1;
				 		*best_lca = t;
				 		*best_left = left;
				 		*best_right = right;
				 	}
					pop(&sta);
					dist--;			
				}else{
					if(currentedge[v] == 1){
						a = T.first_child[v];
						currentedge[a] = 1;
						currentedge[v] = 2;
					}else{
						a = T.next_sibling[T.first_child[v]];
						currentedge[a] = 1;
						currentedge[v] = 3;						
					}
					if(dist < k && v > (T.n+1)/2){ //fixed if(dist <= k && v > (T.n+1)/2){
						push(a,&sta);
						dist++;
					}else currentedge[v] = 3;
				}
			}
			g = t;
			t = T.parent[t];
			dist2++;
			if(dist2 > k) break;
	}
	if(ch != 0) return best_w2;
	else return -1;

}

void make_sum(Tree T, Matrix M, Matrix A, Matrix *H, List *order){
	int ch;
	Node *rt, *rt2;
	
	for(rt = order->head->next; rt->next != NULL; rt = rt->next){
		ch = 0;
		for(rt2 = order->head->next; rt2->next != NULL; rt2 = rt2->next){
			if(rt2->value == rt->value){
				ch = 1;
				H->cpn[rt->value][rt2->value] = 0;
				continue;
			}
			if(ch == 1){
				if(A.cpn[rt->value][rt2->value] != 1 && A.cpn[rt2->value][rt->value] != 1){
					if(rt->value <= M.n && rt2->value <= M.n){
				  	H->cpn[rt->value][rt2->value] = H->cpn[rt2->value][rt->value] = M.cpn[rt->value][rt2->value];
				  }else if(rt->value > M.n && rt2->value <= M.n){
						H->cpn[rt->value][rt2->value] = H->cpn[rt2->value][rt->value] = H->cpn[T.first_child[rt->value]][rt2->value]+H->cpn[T.next_sibling[T.first_child[rt->value]]][rt2->value];
					}else{
						H->cpn[rt->value][rt2->value] = H->cpn[rt2->value][rt->value] = H->cpn[rt->value][T.first_child[rt2->value]] + H->cpn[rt->value][T.next_sibling[T.first_child[rt2->value]]];
					}
				}else{
					if(A.cpn[rt->value][rt2->value] == 1){
						H->cpn[rt->value][rt2->value] = H->cpn[rt2->value][rt->value] = H->cpn[rt->value][T.first_child[rt2->value]] + H->cpn[rt->value][T.next_sibling[T.first_child[rt2->value]]];
					}else{
						H->cpn[rt->value][rt2->value] = H->cpn[rt2->value][rt->value] =  H->cpn[T.first_child[rt->value]][rt2->value] + H->cpn[T.next_sibling[T.first_child[rt->value]]][rt2->value];
					}
				}
			}
		}
	}
}

double error2(Matrix M, Matrix X){
	int i, j;
	double sum;
	
	sum = 0;
	for(i = 1; i < M.n; i++){
		for(j = i + 1; j <= M.n; j++){
			sum = sum + (X.cpn[i][j] - M.cpn[i][j]) * (X.cpn[i][j] - M.cpn[i][j]);
		}
	}
	
	return(sum);
}

double diffUserTime(struct rusage *start, struct rusage *end)
{
    return ((double)end->ru_utime.tv_sec - (double)start->ru_utime.tv_sec)*1000 + ((double)end->ru_utime.tv_usec - (double)start->ru_utime.tv_usec)/1000;
}

double diffUserTime2(struct timespec *tsStart, struct timespec *tsEnd)
{
    return ((double)tsEnd->tv_sec - (double)tsStart->tv_sec)*1000 + ((double)tsEnd->tv_nsec - (double)tsStart->tv_nsec)/1000000;
}

