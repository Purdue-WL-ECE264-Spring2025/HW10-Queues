#include "queue.h"

void test_serialize_tiles();
int main() {

  struct game_state start = {.tiles = {{0x1, 0x2, 0x3, 0x4},
                                       {0x5, 0x6, 0x7, 0x8},
                                       {0x9, 0xa, 0xb, 0xc},
                                       {0xd, 0xe, 0xf, 0x0}},
                             .empty_col = 3,
                             .empty_row = 3,
                             .num_steps = 0};

  move_down(&start);
  move_down(&start);
  move_right(&start);
  move_right(&start);
  move_down(&start);
  move_left(&start);
  move_up(&start);
  move_up(&start);
  move_up(&start);

  start.num_steps = 0;

  printf("Should be zero: %d\n", number_of_moves(start));

  return 0;
}