#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "list.h"
#include "linkage.h"

void c_link(Matrix M, Matrix *D, Tree *T){
	double min, max;
	int i, j, min_i = 0, min_j = 0, count;
	List U;
	Node* p;
	Node* q;
	
	initialize(&U);
	for(i = 1; i <= M.n; i++) insertBefore(U.tail, i);

	for(i = 1; i <= M.n; i++){
		for(j = i + 1; j <= M.n; j++){	
			D->cpn[i][j] = M.cpn[i][j];
			D->cpn[j][i] = M.cpn[j][i];
		}
	}
	count = 1;
	for(i = 1; i < M.n - 1; i++){
		min = 1000000000000;
		////////////////5 //////////////////////////////
		for(p = U.head->next; p->next != NULL; p = p->next){
			for(q = p-> next; q->next != NULL;  q = q->next){
				if(D->cpn[p->value][q->value] < min){
					min = D->cpn[p->value][q->value];
					min_i = p->value;
					min_j = q->value;
				}
			}
		}
		//////// 7/////////////////////////
		T->first_child[M.n + count] = min_i;
		T->next_sibling[T->first_child[M.n + count]] = min_j;
		T->height[M.n + count] = min / 2.0;
		////////////////8////////////////////////////
		for(p = U.head->next; p->next != NULL; p = p->next){
			if(p->value == min_i){
				erase(p);
				break;
			}
		}
		for(p = U.head->next; p->next != NULL; p = p->next){
			if(p->value == min_j){
				erase(p);
				break;
			}
		}
		insertBefore(U.tail, (M.n + count));	
		///////////9/////////////////////////////////
		for(p = U.head->next; p->next != 0; p = p->next){
			if(p->value != (count + M.n)){
				max = D->cpn[p->value][min_i];
				if(max < D->cpn[p->value][min_j]){
					max = D->cpn[p->value][min_j];
				}
				D->cpn[M.n + count][p->value] = max;
				D->cpn[p->value][M.n + count] = max;
			}
		}
		//show_matrix(*D);
		count++;
	}
	T->first_child[M.n + count] = U.head->next->value;
	T->next_sibling[T->first_child[M.n + count]] = U.tail->prev->value;
	T->root = T->n;
	min=D->cpn[T->first_child[M.n + count]][T->next_sibling[T->first_child[M.n + count]]];
	T->height[T->root] = min / 2.0;
	///////////////////////////////////////////////////////
		
	while(U.head->next != U.tail) erase(U.tail->prev);
	free(U.head);
	free(U.tail);		
}

void s_link(Matrix M, Matrix *D, Tree *T){
	double min, mmin;
	int i, j, min_i = 0, min_j = 0, count;
	List U;
	Node* p;
	Node* q;
	
	initialize(&U);
	for(i = 1; i <= M.n; i++) insertBefore(U.tail, i);

	for(i = 1; i <= M.n; i++){
		for(j = i + 1; j <= M.n; j++){	
			D->cpn[i][j] = M.cpn[i][j];
			D->cpn[j][i] = M.cpn[j][i];
		}
	}
	count = 1;
	for(i = 1; i < M.n - 1; i++){
		min = 1000000000000;
		////////////////5 //////////////////////////////
		for(p = U.head->next; p->next != NULL; p = p->next){
			for(q = p-> next; q->next != NULL;  q = q->next){
				if(D->cpn[p->value][q->value] < min){
					min = D->cpn[p->value][q->value];
					min_i = p->value;
					min_j = q->value;
				}
			}
		}
		//////// 7/////////////////////////
		T->first_child[M.n + count] = min_i;
		T->next_sibling[T->first_child[M.n + count]] = min_j;
		T->height[M.n + count] = min / 2.0;
		////////////////8////////////////////////////
		for(p = U.head->next; p->next != NULL; p = p->next){
			if(p->value == min_i){
				erase(p);
				break;
			}
		}
		for(p = U.head->next; p->next != NULL; p = p->next){
			if(p->value == min_j){
				erase(p);
				break;
			}
		}
		insertBefore(U.tail, (M.n + count));	
		///////////9/////////////////////////////////
		for(p = U.head->next; p->next != 0; p = p->next){
			if(p->value != (count + M.n)){
				mmin = D->cpn[p->value][min_i];
				if(mmin > D->cpn[p->value][min_j]){
					mmin = D->cpn[p->value][min_j];
				}
				D->cpn[M.n + count][p->value] = mmin;
				D->cpn[p->value][M.n + count] = mmin;
			}
		}
		//show_matrix(*D);
		count++;
	}
	T->first_child[M.n + count] = U.head->next->value;
	T->next_sibling[T->first_child[M.n + count]] = U.tail->prev->value;
	T->root = T->n;
	min=D->cpn[T->first_child[M.n + count]][T->next_sibling[T->first_child[M.n + count]]];
	T->height[T->root] = min / 2.0;
	///////////////////////////////////////////////////////
		
	while(U.head->next != U.tail) erase(U.tail->prev);
	free(U.head);
	free(U.tail);		
}

void a_link(Matrix M, Matrix *D, Tree *T){
	double min, ave;
	int i, j, min_i = 0, min_j = 0, count;
	List U;
	Node* p;
	Node* q;
	int *size=(int *)malloc(sizeof(int)*(2*M.n));
	
	for (i = 1; i <= T->n; i++) size[i]=1;
	
	initialize(&U);
	for(i = 1; i <= M.n; i++) insertBefore(U.tail, i);

	for(i = 1; i <= M.n; i++){
		for(j = i + 1; j <= M.n; j++){	
			D->cpn[i][j] = M.cpn[i][j];
			D->cpn[j][i] = M.cpn[j][i];
		}
	}
	count = 1;
	for(i = 1; i < M.n - 1; i++){
		min = 1000000000000;
		////////////////5 //////////////////////////////
		for(p = U.head->next; p->next != NULL; p = p->next){
			for(q = p-> next; q->next != NULL;  q = q->next){
				if(D->cpn[p->value][q->value] < min){
					min = D->cpn[p->value][q->value];
					min_i = p->value;
					min_j = q->value;
				}
			}
		}
		//////// 7/////////////////////////
		T->first_child[M.n + count] = min_i;
		T->next_sibling[T->first_child[M.n + count]] = min_j;
		T->height[M.n + count] = min / 2.0;
		//////////// debug //////////////////////
		//printf("[%d] %lf (%d, %d)\n", M.n+count, min, min_i, min_j);
		////////////////8////////////////////////////
		for(p = U.head->next; p->next != NULL; p = p->next){
			if(p->value == min_i){
				erase(p);
				break;
			}
		}
		for(p = U.head->next; p->next != NULL; p = p->next){
			if(p->value == min_j){
				erase(p);
				break;
			}
		}
		insertBefore(U.tail, (M.n + count));	
		size[M.n+count]=size[min_i]+size[min_j];
		///////////9/////////////////////////////////
		for(p = U.head->next; p->next != 0; p = p->next){
			if(p->value != (count + M.n)){
				ave = (size[min_i]*D->cpn[p->value][min_i]
						+size[min_j]*D->cpn[p->value][min_j])/(double)size[M.n+count];
				D->cpn[M.n + count][p->value] = ave;
				D->cpn[p->value][M.n + count] = ave;
			}
		}
		//show_matrix(*D);
		count++;
	}
	T->first_child[M.n + count] = U.head->next->value;
	T->next_sibling[T->first_child[M.n + count]] = U.tail->prev->value;
	T->root = T->n;
	min=D->cpn[T->first_child[M.n + count]][T->next_sibling[T->first_child[M.n + count]]];
	T->height[T->root] = min / 2.0;
	///////////////////////////////////////////////////////
		
	while(U.head->next != U.tail) erase(U.tail->prev);
	free(U.head);
	free(U.tail);		
}