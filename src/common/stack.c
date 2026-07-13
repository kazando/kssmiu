#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

/****************************************************************************/
/* 関数名: push(x)                                                          */
/* 動作: リストの先頭にデータ x が入ったセルを挿入する.                     */
/*       Insert(x,NULL)と同じであることに注意せよ。                         */
/****************************************************************************/
void push(int x, Stack *r) {
	Cell *newcell;
	newcell = (Cell *)malloc(sizeof(Cell));
	newcell->data = x;
	if (stack_empty(r) == 1) {
		r->root = newcell;
		newcell->next = NULL;
	} else {
		newcell->next = r->root;
		r->root = newcell;
	}
	r->length++;
}

/****************************************************************************/
/* 関数名: pop(void)                                                        */
/* 動作: リストの先頭のセルを削除して、先頭のセルに入っていたデータを返す。 */
/*       Delete(NULL)とほとんど同じであることに注意せよ。                   */
/****************************************************************************/

int pop(Stack *r) {
	Cell *temp;
	int x;
	if (stack_empty(r) == 1){
		printf("Stack is empty\n");
		return(0);
	}
	x = r->root->data;
	temp = r->root;
	r->root = r->root->next;
	r->length--;
	if (temp != NULL) free(temp);
	return(x);


}

/****************************************************************************/
/*         関数名: traverse_list                                             */
/*         動作: リストの中身を画面に出力する。                             */
/****************************************************************************/

void traverse_list_s(Stack *p){
	Cell *temp = p->root;

	while (temp != NULL) {
		printf("%d ", temp->data);
		temp = temp->next;
	}
	putchar('\n');
}

/****************************************************************************/
/*         関数名: stack_empty_empty                                             */
/*         動作: stackの中身があれば0を返し、なければ1を返す                     */
/****************************************************************************/
int stack_empty(Stack *p){
    if(p->length >= 1){
        return(0);
    }else if(p->length == 0){
        return(1);
    }
    return(0);
}





