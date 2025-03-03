#include "tile_game.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

int *get_compressed(int *written, int max) {
  int *compressed = calloc(max, sizeof(int));
  int value = 0;
  for (int i = 0; i < max; i++) {
    compressed[i] = value;
    if (written[i] == 0)
      value++;
  }
  return compressed;
}

uint64_t serialize(struct game_state state) {
  uint64_t buffer = 0;
  int positions[16] = {0};
  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      positions[state.tiles[row][col]] = (row << 2) | col;
    }
  }

  int positions_written[16] = {0};
  for (int i = 1; i <= 8; i++) {
    buffer <<= 4;
    buffer |= (positions[i] & 0b1111);
    positions_written[positions[i]] = 1;
  }

  int *compressed = get_compressed(positions_written, 16);
  for (int i = 9; i <= 12; i++) {
    buffer <<= 3;
    buffer |= compressed[positions[i]] & 0b111;
    positions_written[positions[i]] = 1;
  }

  free(compressed);
  compressed = get_compressed(positions_written, 16);
  for (int i = 13; i <= 14; i++) {
    buffer <<= 2;
    buffer |= compressed[positions[i]] & 0b11;
    positions_written[positions[i]] = 1;
  }

  free(compressed);
  compressed = get_compressed(positions_written, 16);
  for (int i = 15; i <= 15; i++) {
    buffer <<= 1;
    buffer |= compressed[positions[i]] & 0b1;
    positions_written[positions[i]] = 1;
  }

  buffer <<= 15;
  buffer |= state.num_steps;

  free(compressed);
  return buffer;
}

int *get_decompressed(int *read, int max) {
  int *decompressed = calloc(max, sizeof(int));
  int out_idx = 0;
  for (int i = 0; i < max; i++) {
    if (read[i] == 0) {
      decompressed[out_idx++] = i;
    }
  }

  return decompressed;
}

int read_top(uint64_t buf, int sz) {
  uint64_t mask = (1 << sz) - 1;
  return (buf & (mask << (64 - sz))) >> (64 - sz);
}

struct game_state deserialize(uint64_t buffer) {
  struct game_state state = {0};
  int positions_read[16] = {0};

  for (int i = 1; i <= 8; i++) {
    uint8_t pos = read_top(buffer, 4);
    buffer <<= 4;

    positions_read[pos] = 1;
    state.tiles[pos >> 2][pos & 0b11] = i;
  }

  int *decompressed = get_decompressed(positions_read, 16);
  for (int i = 9; i <= 12; i++) {
    uint8_t pos = read_top(buffer, 3);
    buffer <<= 3;

    pos = decompressed[pos];
    positions_read[pos] = 1;
    state.tiles[pos >> 2][pos & 0b11] = i;
  }

  free(decompressed);
  decompressed = get_decompressed(positions_read, 16);
  for (int i = 13; i <= 14; i++) {
    uint8_t pos = read_top(buffer, 2);
    buffer <<= 2;

    pos = decompressed[pos];
    positions_read[pos] = 1;
    state.tiles[pos >> 2][pos & 0b11] = i;
  }

  free(decompressed);
  decompressed = get_decompressed(positions_read, 16);
  for (int i = 15; i <= 15; i++) {
    uint8_t pos = read_top(buffer, 1);
    buffer <<= 1;

    pos = decompressed[pos];
    positions_read[pos] = 1;
    state.tiles[pos >> 2][pos & 0b11] = i;
  }

  free(decompressed);

  bool found_zero = false;
  for (uint8_t i = 0; i < 16; i++) {
    if (positions_read[i] == 0) {
      assert(!found_zero);
      found_zero = true;
      state.empty_row = i >> 2;
      state.empty_col = i & 0b11;
    }
  }

  state.num_steps = read_top(buffer, 15);
  return state;
}

void move_up(struct game_state *state) {
  if (state->empty_row == 3)
    return;
  state->tiles[state->empty_row][state->empty_col] =
      state->tiles[state->empty_row + 1][state->empty_col];
  state->tiles[state->empty_row + 1][state->empty_col] = 0;
  state->empty_row++;
  state->num_steps++;
}

void move_down(struct game_state *state) {
  if (state->empty_row == 0)
    return;
  state->tiles[state->empty_row][state->empty_col] =
      state->tiles[state->empty_row - 1][state->empty_col];
  state->tiles[state->empty_row - 1][state->empty_col] = 0;
  state->empty_row--;
  state->num_steps++;
}

void move_left(struct game_state *state) {
  if (state->empty_col == 3)
    return;
  state->tiles[state->empty_row][state->empty_col] =
      state->tiles[state->empty_row][state->empty_col + 1];
  state->tiles[state->empty_row][state->empty_col + 1] = 0;
  state->empty_col++;
  state->num_steps++;
}

void move_right(struct game_state *state) {
  if (state->empty_col == 0)
    return;
  state->tiles[state->empty_row][state->empty_col] =
      state->tiles[state->empty_row][state->empty_col - 1];
  state->tiles[state->empty_row][state->empty_col - 1] = 0;
  state->empty_col--;
  state->num_steps++;
}