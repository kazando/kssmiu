#ifndef LIST
#define LIST
#include <stdio.h>
#include <stdlib.h>

typedef struct node {
  int value;
  struct node* prev;
  struct node* next;
}Node;

typedef struct list {
  struct node* head;
  struct node* tail;
}List;

void initialize(List* p);
void insertBefore(Node* p, int v);
void insertAfter(Node* p, int v);
void erase(Node* p);
void terminate_list(List* list);
void printNumbers(List* p);
void printString(List* p);

#endif
