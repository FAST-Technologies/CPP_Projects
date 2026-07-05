#define _POSIX_C_SOURCE 200809L
#include "s21_tetris_logic.h"

#include <fstream>
#include <iostream>

#ifdef TEST_EXCEPTION_MODE
bool simulate_allocation_failure = false;
int failure_point =
    0;  // 1 for field, 2 for field row, 3 for next, 4 for next row
#endif

namespace s21 {

const int FIELD_HEIGHT = 20;
const int FIELD_WIDTH = 10;
const std::string HIGH_SCORE_FILE = "high_score_tetris_cli.txt";
const std::string HIGH_SCORE_DESKTOP_FILE = "high_score_tetris_desktop.txt";
bool timer_initialized = false;

const int SHAPES[7][4][4][4] = {
    {{{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},  // I
     {{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}},
     {{0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},
     {{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}}},
    {{{0, 0, 0, 0}, {1, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}},  // L
     {{0, 1, 1, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}},
     {{0, 1, 1, 1}, {0, 0, 0, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},
     {{0, 0, 1, 0}, {0, 0, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}}},
    {{{0, 0, 0, 0}, {0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}},  // J
     {{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},
     {{0, 1, 1, 1}, {0, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
     {{0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}}},
    {{{0, 0, 0, 0}, {0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}},  // S
     {{0, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}},
     {{0, 0, 1, 1}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
     {{0, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}}},
    {{{0, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},  // Z
     {{0, 0, 1, 0}, {0, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}},
     {{0, 1, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}},
     {{0, 0, 1, 0}, {0, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}}},
    {{{0, 0, 0, 0}, {0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}},  // T
     {{0, 1, 0, 0}, {0, 1, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}},
     {{0, 1, 1, 1}, {0, 0, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
     {{0, 0, 1, 0}, {0, 1, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}}},
    {{{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},  // O
     {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},
     {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},
     {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}}}};

Tetromino current{4, 0, 0, {{0}}};
Tetromino next{0, 0, 0, {{0}}};
GameInfo_t game_state{nullptr, nullptr, 0, 0, 1, 500, 0};
struct timespec last_fall_time {
  0, 0
};
int is_terminated = 0;
bool is_desktop_mode = false;

void set_interface_mode(bool desktop) { is_desktop_mode = desktop; }

std::string get_high_score_file() {
  return is_desktop_mode ? HIGH_SCORE_DESKTOP_FILE : HIGH_SCORE_FILE;
}

void init_game() {
  if (game_state.field) {
    cleanup_game();
  }

#ifdef TEST_EXCEPTION_MODE
  if (simulate_allocation_failure && failure_point == 1) {
    throw std::runtime_error("Memory allocation failed for field");
  }
#endif
  game_state.field = new int*[FIELD_HEIGHT];
#ifdef TEST_EXCEPTION_MODE
  if (simulate_allocation_failure && failure_point == 1) {
    delete[] game_state.field;
    throw std::runtime_error("Memory allocation failed for field");
  }
#endif
  if (!game_state.field)
    throw std::runtime_error("Memory allocation failed for field");
  for (int i = 0; i < FIELD_HEIGHT; ++i) {
#ifdef TEST_EXCEPTION_MODE
    if (simulate_allocation_failure && failure_point == 2) {
      for (int j = 0; j < i; ++j) delete[] game_state.field[j];
      delete[] game_state.field;
      throw std::runtime_error("Memory allocation failed for field row");
    }
#endif
    game_state.field[i] = new int[FIELD_WIDTH]();
    if (!game_state.field[i])
      throw std::runtime_error("Memory allocation failed for field row");
  }
#ifdef TEST_EXCEPTION_MODE
  if (simulate_allocation_failure && failure_point == 3) {
    throw std::runtime_error("Memory allocation failed for next");
  }
#endif
  game_state.next = new int*[4];
#ifdef TEST_EXCEPTION_MODE
  if (simulate_allocation_failure && failure_point == 3) {
    delete[] game_state.next;
    throw std::runtime_error("Memory allocation failed for next");
  }
#endif
  if (!game_state.next)
    throw std::runtime_error("Memory allocation failed for next");
  for (int i = 0; i < 4; ++i) {
#ifdef TEST_EXCEPTION_MODE
    if (simulate_allocation_failure && failure_point == 4) {
      for (int j = 0; j < i; ++j) delete[] game_state.next[j];
      delete[] game_state.next;
      throw std::runtime_error("Memory allocation failed for next row");
    }
#endif
    game_state.next[i] = new int[4]();
    if (!game_state.next[i])
      throw std::runtime_error("Memory allocation failed for next row");
  }
  game_state.score = 0;
  game_state.high_score = 0;
  std::ifstream fp(get_high_score_file());
  if (fp.is_open()) {
    fp >> game_state.high_score;
    fp.close();
  }
  game_state.level = 1;
  game_state.speed = 500;
  game_state.pause = 0;
  is_terminated = 0;
  srand(static_cast<unsigned>(time(nullptr)));
  current = generate_random_tetromino();
  next = generate_random_tetromino();
  clock_gettime(CLOCK_MONOTONIC, &last_fall_time);
  timer_initialized = true;
}

void cleanup_game() {
  if (game_state.field) {
    for (int i = 0; i < FIELD_HEIGHT; ++i) {
      delete[] game_state.field[i];
    }
    delete[] game_state.field;
    game_state.field = nullptr;
  }
  if (game_state.next) {
    for (int i = 0; i < 4; ++i) {
      delete[] game_state.next[i];
    }
    delete[] game_state.next;
    game_state.next = nullptr;
  }
  std::ofstream fp(get_high_score_file());
  if (fp.is_open()) {
    fp << game_state.high_score;
    fp.close();
  }
  timer_initialized = false;
}

bool collides(const Tetromino& tetro) {
  for (int y = 0; y < 4; ++y) {
    for (int x = 0; x < 4; ++x) {
      if (tetro.shape[y][x]) {
        int new_x = tetro.x + x;
        int new_y = tetro.y + y;
        if (new_x < 0 || new_x >= FIELD_WIDTH || new_y < 0 ||
            new_y >= FIELD_HEIGHT) {
          return true;
        }
        if (game_state.field && game_state.field[new_y][new_x]) {
          return true;
        }
      }
    }
  }
  return false;
}

void rotate_tetromino(Tetromino* tetro) {
  int shape_idx = -1;
  for (int i = 0; i < 7; ++i) {
    if (memcmp(tetro->shape, SHAPES[i][tetro->rotation],
               sizeof(tetro->shape)) == 0) {
      shape_idx = i;
      break;
    }
  }
  if (shape_idx != -1) {
    tetro->rotation = (tetro->rotation + 1) % 4;
    memcpy(tetro->shape, SHAPES[shape_idx][tetro->rotation],
           sizeof(tetro->shape));
  }
}

void place_tetromino() {
  for (int y = 0; y < 4; ++y) {
    for (int x = 0; x < 4; ++x) {
      if (current.shape[y][x]) {
        int field_y = current.y + y;
        int field_x = current.x + x;
        if (field_y >= 0 && field_y < FIELD_HEIGHT && game_state.field) {
          game_state.field[field_y][field_x] = 1;
        }
      }
    }
  }
}

int clear_full_lines() {
  if (!game_state.field) return 0;

  int lines_cleared = 0;
  for (int y = FIELD_HEIGHT - 1; y >= 0; --y) {
    bool full = true;
    for (int x = 0; x < FIELD_WIDTH; ++x) {
      if (!game_state.field[y][x]) {
        full = false;
        break;
      }
    }
    if (full) {
      ++lines_cleared;
      for (int move_y = y; move_y > 0; --move_y) {
        for (int x = 0; x < FIELD_WIDTH; ++x) {
          game_state.field[move_y][x] = game_state.field[move_y - 1][x];
        }
      }
      for (int x = 0; x < FIELD_WIDTH; ++x) {
        game_state.field[0][x] = 0;
      }
      ++y;
    }
  }
  return lines_cleared;
}

void update_score(int lines_cleared) {
  switch (lines_cleared) {
    case 1:
      game_state.score += 100;
      break;
    case 2:
      game_state.score += 300;
      break;
    case 3:
      game_state.score += 700;
      break;
    case 4:
      game_state.score += 1500;
      break;
  }
  if (game_state.score > game_state.high_score) {
    game_state.high_score = game_state.score;
  }
}

void update_level() {
  if (game_state.score >= game_state.level * 600 && game_state.level < 10) {
    game_state.level++;
    game_state.speed = 500 - (game_state.level - 1) * 50;
    if (game_state.speed < 50) game_state.speed = 50;
  }
}

void reset_tetrominos() {
  current = next;
  current.x = 4;
  current.y = 0;
  next = generate_random_tetromino();
}

void check_termination() {
  if (collides(current)) {
    is_terminated = 1;
  }
  clock_gettime(CLOCK_MONOTONIC, &last_fall_time);
}

void fix_tetromino() {
  place_tetromino();
  int lines_cleared = clear_full_lines();
  update_score(lines_cleared);
  update_level();
  reset_tetrominos();
  check_termination();
}

void handle_start_action() {
  if (is_terminated) {
    cleanup_game();
    is_terminated = 0;
    init_game();
  }
}

void handle_pause_action() { game_state.pause = !game_state.pause; }

void handle_horizontal_move(int dx, bool hold) {
  if (!hold && !game_state.pause && !is_terminated) {
    Tetromino temp = current;
    temp.x += dx;
    if (!collides(temp)) {
      current.x = temp.x;
    }
  }
}

void handle_down_action(bool hold) {
  if (game_state.pause || is_terminated) return;

  Tetromino temp = current;
  if (hold) {
    while (!collides(temp)) {
      current = temp;
      temp.y++;
    }
    fix_tetromino();
  } else {
    temp.y++;
    if (!collides(temp)) {
      current.y = temp.y;
    } else {
      fix_tetromino();
    }
  }
}

void handle_action_rotate(bool hold) {
  if (!hold && !game_state.pause && !is_terminated) {
    Tetromino temp = current;
    rotate_tetromino(&temp);
    if (!collides(temp)) {
      current = temp;
    }
  }
}

void userInput(UserAction_t action, bool hold) {
  if (!game_state.field && !is_terminated && action == Start) {
    init_game();
    return;
  }

  if (!game_state.field) return;

  switch (action) {
    case Start:
      handle_start_action();
      break;
    case Pause:
      if (!is_terminated) {
        handle_pause_action();
      }
      break;
    case Terminate:
      is_terminated = 1;
      cleanup_game();
      break;
    case Left:
      handle_horizontal_move(-1, hold);
      break;
    case Right:
      handle_horizontal_move(1, hold);
      break;
    case Down:
      handle_down_action(hold);
      break;
    case Up:
      break;
    case Action:
      handle_action_rotate(hold);
      break;
  }
}

GameInfo_t updateCurrentState() {
  if (!game_state.field) {
    return game_state;
  }

  if (!game_state.pause && !is_terminated && timer_initialized) {
    struct timespec current_time;
    clock_gettime(CLOCK_MONOTONIC, &current_time);
    long long elapsed_ms =
        (current_time.tv_sec - last_fall_time.tv_sec) * 1000LL +
        (current_time.tv_nsec - last_fall_time.tv_nsec) / 1000000LL;

    if (elapsed_ms >= game_state.speed) {
      Tetromino temp = current;
      temp.y++;
      if (!collides(temp)) {
        current.y = temp.y;
      } else {
        fix_tetromino();
      }
      last_fall_time = current_time;
    }
  }

  if (game_state.next) {
    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 4; ++j) {
        game_state.next[i][j] = next.shape[i][j];
      }
    }
  }

  return game_state;
}

void reset_game_state() {
  if (game_state.field) cleanup_game();
  is_terminated = 0;
  init_game();
}

Tetromino get_current_tetromino() { return current; }

GameInfo_t get_game_state() { return game_state; }

FieldSizes return_sizes() { return FieldSizes{FIELD_WIDTH, FIELD_HEIGHT}; }

void set_tetromino_position(int x, int y) {
  current.x = x;
  current.y = y;
}

Tetromino generate_random_tetromino() {
  Tetromino t;
  t.x = 4;
  t.y = 0;
  t.rotation = 0;
  int shape_idx = rand() % 7;
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      t.shape[i][j] = SHAPES[shape_idx][0][i][j];
    }
  }
  return t;
}

}  // namespace s21