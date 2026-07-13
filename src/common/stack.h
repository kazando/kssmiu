#ifndef _STACK
#define _STACK

typedef struct CELL {
	int data;
	struct CELL *next;
}Cell;

typedef struct STACK{
    Cell *root;
    int length;
}Stack;

/* 関数のプロトタイプ宣言 */
void traverse_list_s(Stack *p);
void push(int  x,Stack *r);
int pop(Stack *r);
int stack_empty(Stack *p);

#endif
