#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

/****************************************************************************/
/* 関数名: enqueue(x)                                                       */
/* 動作: リストの末尾にデータ x が入ったセルを挿入する.                        */
/****************************************************************************/
void enqueue(int x, Queue *p) {
	Cell *newcell;
	newcell = (Cell *)malloc(sizeof(Cell));
	if (newcell == NULL){
		fprintf(stderr," enqueue: Malloc error [newcell]");
		exit(1);
	}
	newcell->data = x;
	newcell->next = NULL;
	if (p->tail != NULL) p->tail->next = newcell;
	p->tail = newcell;
	if(p->head == NULL) p->head = newcell;
	p->length++;


}

/****************************************************************************/
/* 関数名: dequeue(void)                                                    */
/* 動作:  リストの先頭のセルを削除して、先頭のセルに入っていたデータを返す。    */
/*        Pop とほぼ同じ。                                                  */
/****************************************************************************/

int dequeue(Queue *p) {
	Cell *temp;
	int x;
	if (queue_empty(p) == 1){
		printf("queue is empty\n");
	} else {
		x = p->head->data;
		temp = p->head;
		p->head = p->head->next;
		p->length--;
		if (temp != NULL) {
			free(temp);
			temp = NULL;
		}
		return(x);
	}
		
	return(0);

}

/****************************************************************************/
/*         関数名: traverse_list                                             */
/*         動作: リストの中身を画面に出力する。                             */
/****************************************************************************/
void traverse_list_q(Queue p) {
	Cell *temp = p.head;

	while(temp != NULL){
		printf("%d ", temp->data);
		temp = temp->next;
	}
	putchar('\n');
}


/****************************************************************************/
/*         関数名: queue_empty                                             */
/*         動作: queueの中身があれば0を返し、なければ1を返す                     */
/****************************************************************************/
int queue_empty(Queue *p){
    
    if(p->length >= 1){
        return(0);
    }else if(p->length == 0){
        return(1);
    }
    return(0);
}


