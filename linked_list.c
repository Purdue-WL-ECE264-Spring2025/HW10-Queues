#include "linked_list.h"

#include <assert.h>
#include <stdlib.h>

struct list_node *new_node(size_t value) {
  struct list_node *node = malloc(sizeof(struct list_node));
  node->value = value;
  node->next = NULL;
  return node;
}

void insert_at_head(struct linked_list *list, size_t value) {
  struct list_node *new_head = new_node(value);
  new_head->next = list->head;
  list->head = new_head;
}

void insert_at_tail(struct linked_list *list, size_t value) {
  if (list->head == NULL) {
    list->head = new_node(value);
    return;
  }

  struct list_node *cur;
  for (cur = list->head; cur->next != NULL; cur = cur->next)
    ;
  cur->next = new_node(value);
}

size_t remove_from_head(struct linked_list *list) {
  assert(list->head != NULL);
  struct list_node *to_delete = list->head;
  list->head = list->head->next;
  size_t value = to_delete->value;
  free(to_delete);
  return value;
}

size_t remove_from_tail(struct linked_list *list) {
  assert(list->head != NULL);
  if (list->head->next == NULL) {
    return remove_from_head(list);
  }

  struct list_node *prev = list->head;
  struct list_node *cur = list->head->next;

  while (cur->next != NULL) {
    prev = prev->next;
    cur = cur->next;
  }

  prev->next = NULL;
  size_t value = cur->value;
  free(cur);
  return value;
}

void free_list_node(struct list_node *node) {
  if (node->next != NULL)
    free_list_node(node->next);
  free(node);
}

void free_list(struct linked_list list) { free_list_node(list.head); }

bool exists(struct linked_list list, size_t value) {
  for (struct list_node *cur = list.head; cur != NULL; cur = cur->next) {
    if (cur->value == value)
      return true;
  }
  return false;
}

void dump_list(FILE *fp, struct linked_list list) {
  fprintf(fp, "[ ");
  for (struct list_node *cur = list.head; cur != NULL; cur = cur->next) {
    fprintf(fp, "%zu ", cur->value);
  }
  fprintf(fp, "]\n");
}