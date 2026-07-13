#include <stdio.h>
#include <stdlib.h>
#include "list.h"

void initialize(struct list* p) {
  p->head = malloc(sizeof(struct node));
  p->tail = malloc(sizeof(struct node));
  p->head->prev = 0;
  p->head->next = p->tail;
  p->tail->prev = p->head;
  p->tail->next = 0;
}

void insertBefore(struct node* p, int v) {
  struct node* newNode = malloc(sizeof(struct node));
  newNode->value = v;
  newNode->next = p;
  p->prev->next = newNode;
  newNode->prev = p->prev;
  p->prev = newNode;
}

void insertAfter(struct node* p, int v) {
  struct node* newNode = malloc(sizeof(struct node));
  newNode->value = v;
  newNode->prev = p;
  p->next->prev = newNode;
  newNode->next = p->next;
  p->next = newNode;
}

void erase(struct node* p) {
  p->prev->next = p->next;
  p->next->prev = p->prev;
  free(p);
}

void terminate_list(List *list) {
  while (list->head->next != list->tail) erase(list->tail->prev);
  free(list->head);
  free(list->tail);
}

void printNumbers(struct list* p) {
  for (struct node* i = p->head->next; i->next != 0; i = i->next)
    printf("%d ", i->value);
  printf("\n");
}

void printString(struct list* p) {
  for (struct node* i = p->head->next; i->next != 0; i = i->next)
    printf("%c", i->value);
  printf("\n");
}
