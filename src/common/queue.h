#ifndef _QUEUE
#define _QUEUE
#include "graph.h"


typedef struct QUEUE{
    Cell *head;
    Cell *tail;
    int length;
}Queue;

/* 関数のプロトタイプ宣言 */
void traverse_list_q(Queue p);
void enqueue(int x, Queue *p);
int dequeue(Queue *p);
int queue_empty(Queue *p);


#endif
