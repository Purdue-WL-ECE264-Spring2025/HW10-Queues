#include "queue.h"
#include "linked_list.h"
#include "tile_game.h"

#include <stdbool.h>
#include <stdio.h>

void enqueue(struct queue *q, struct game_state state) {
  uint64_t serialized = serialize(state);
  if (!exists(q->data, serialized))
    insert_at_head(&(q->data), serialized);
}

struct game_state dequeue(struct queue *q) {
  return deserialize(remove_from_tail(&(q->data)));
}

bool is_endgame(struct game_state state) {
  if (state.tiles[3][3] != 0)
    return false;
  int expected = 1;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (i == 3 && j == 3)
        continue;
      if (state.tiles[i][j] != expected++)
        return false;
    }
  }
  return true;
}

int number_of_moves(struct game_state start) {
  struct queue q = {.data = {.head = NULL}};
  enqueue(&q, start);

  while (q.data.head != NULL) {
    struct game_state cur = dequeue(&q);
    if (is_endgame(cur))
      return cur.num_steps;

    if (cur.empty_row != 0) {
      struct game_state new = cur;
      move_down(&new);
      enqueue(&q, new);
    }

    if (cur.empty_row != 3) {
      struct game_state new = cur;
      move_up(&new);
      enqueue(&q, new);
    }

    if (cur.empty_col != 0) {
      struct game_state new = cur;
      move_right(&new);
      enqueue(&q, new);
    }

    if (cur.empty_col != 3) {
      struct game_state new = cur;
      move_left(&new);
      enqueue(&q, new);
    }
  }

  return -1;
}