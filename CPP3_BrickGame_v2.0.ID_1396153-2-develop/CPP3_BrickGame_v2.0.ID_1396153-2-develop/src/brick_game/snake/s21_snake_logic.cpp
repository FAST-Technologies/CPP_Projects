#include "s21_snake_logic.h"

#include <fstream>

namespace s21 {
GameInfo_t game_state = {nullptr, nullptr, 0, 0, 1, 500, 0};
int snake_x[200] = {5, 4, 3, 2};
int snake_y[200] = {10, 10, 10, 10};
int snake_length = 4;
int food_x = 0;
int food_y = 0;
int direction = 0;  // 0: right, 1: down, 2: left, 3: up
timespec last_move_time = {0, 0};
bool timer_initialized = false;
int is_terminated = 0;
const std::string HIGH_SCORE_FILE = "high_score_snake_cli.txt";
const std::string HIGH_SCORE_DESKTOP_FILE = "high_score_snake_desktop.txt";
bool hard_boundaries = true;
int base_speed = 500;
bool is_speed_boosted = false;
timespec speed_boost_end_time = {0, 0};
bool is_desktop_mode = false;

void set_interface_mode(bool desktop) { is_desktop_mode = desktop; }

std::string get_high_score_file() {
  return is_desktop_mode ? HIGH_SCORE_DESKTOP_FILE : HIGH_SCORE_FILE;
}

void init_game() {
  if (game_state.field && !is_terminated) {
    return;
  }
  if (game_state.field) {
    cleanup_game();
  }
  try {
    game_state.field = new int*[FieldSizes::height];
    for (int i = 0; i < FieldSizes::height; ++i) {
      game_state.field[i] = new int[FieldSizes::width]();
    }
  } catch (const std::bad_alloc& e) {
    throw std::runtime_error("Memory allocation failed: " +
                             std::string(e.what()));
  }
  game_state.next = nullptr;
  snake_length = 4;
  direction = 0;

  // Инициализация позиции змеи
  for (int i = 0; i < snake_length; ++i) {
    snake_x[i] = 5 - i;
    snake_y[i] = 10;
  }

  // Очистка поля
  for (int y = 0; y < FieldSizes::height; ++y) {
    for (int x = 0; x < FieldSizes::width; ++x) {
      game_state.field[y][x] = 0;
    }
  }

  // Размещение змеи на поле
  for (int i = 0; i < snake_length; ++i) {
    if (snake_x[i] >= 0 && snake_x[i] < FieldSizes::width && snake_y[i] >= 0 &&
        snake_y[i] < FieldSizes::height) {
      game_state.field[snake_y[i]][snake_x[i]] = 1;
    }
  }

  // Генерация еды
  srand(static_cast<unsigned>(time(nullptr)));
  do {
    food_x = rand() % FieldSizes::width;
    food_y = rand() % FieldSizes::height;
  } while (game_state.field[food_y][food_x] != 0);
  game_state.field[food_y][food_x] = 2;

  // Загрузка рекорда до сброса
  game_state.high_score = 0;
  std::ifstream fp(get_high_score_file());
  if (fp.is_open()) {
    int temp_score;
    if (fp >> temp_score) {
      game_state.high_score = temp_score;
    }
    fp.close();
  }
  game_state.score = 0;
  game_state.level = 1;
  base_speed = 500;
  game_state.speed = base_speed;
  is_speed_boosted = false;
  game_state.pause = 0;
  is_terminated = 0;
  clock_gettime(CLOCK_MONOTONIC, &last_move_time);
  timer_initialized = true;
  printf("Init game: field=%p, snake_length=%d, direction=%d\n",
         (void*)game_state.field, snake_length, direction);
}

void cleanup_game() {
  if (game_state.field) {
    for (int i = 0; i < FieldSizes::height; ++i) {
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
  game_state.high_score = std::max(game_state.score, game_state.high_score);
  std::ofstream fp(get_high_score_file());
  if (fp.is_open()) {
    fp << game_state.high_score;
    fp.close();
  }
  is_terminated = 1;
  timer_initialized = false;
}

void generate_food() {
  int free_cells = 0;
  for (int y = 0; y < FieldSizes::height; ++y) {
    for (int x = 0; x < FieldSizes::width; ++x) {
      if (game_state.field[y][x] == 0) ++free_cells;
    }
  }
  if (free_cells == 0) {
    is_terminated = 1;
    return;
  }
  do {
    food_x = rand() % FieldSizes::width;
    food_y = rand() % FieldSizes::height;
  } while (game_state.field[food_y][food_x] != 0);
  game_state.field[food_y][food_x] = 2;
}

void move_snake() {
  printf("Moving snake: head=(%d,%d), direction=%d\n", snake_x[0], snake_y[0],
         direction);
  int new_head_x = snake_x[0];
  int new_head_y = snake_y[0];

  switch (direction) {
    case 0:
      new_head_x++;
      break;  // right
    case 1:
      new_head_y++;
      break;  // down
    case 2:
      new_head_x--;
      break;  // left
    case 3:
      new_head_y--;
      break;  // up
  }

  if (hard_boundaries) {
    if (new_head_x < 0 || new_head_x >= FieldSizes::width || new_head_y < 0 ||
        new_head_y >= FieldSizes::height) {
      is_terminated = 1;
      return;
    }
  } else {
    if (new_head_x < 0) new_head_x = FieldSizes::width - 1;
    if (new_head_x >= FieldSizes::width) new_head_x = 0;
    if (new_head_y < 0) new_head_y = FieldSizes::height - 1;
    if (new_head_y >= FieldSizes::height) new_head_y = 0;
  }

  for (int i = 0; i < snake_length; ++i) {
    if (new_head_x == snake_x[i] && new_head_y == snake_y[i]) {
      is_terminated = 1;
      return;
    }
  }
  bool ate_food = (new_head_x == food_x && new_head_y == food_y);

  if (ate_food) {
    if (snake_length >= 199) {
      snake_length = 200;
      is_terminated = 1;
      return;
    }
    ++snake_length;
    game_state.score += 1;
    if (game_state.score > game_state.high_score) {
      game_state.high_score = game_state.score;
    }
    generate_food();
  } else {
    int tail_x = snake_x[snake_length - 1];
    int tail_y = snake_y[snake_length - 1];
    if (tail_x >= 0 && tail_x < FieldSizes::width && tail_y >= 0 &&
        tail_y < FieldSizes::height) {
      game_state.field[tail_y][tail_x] = 0;
    }
  }
  for (int i = snake_length - 1; i > 0; --i) {
    snake_x[i] = snake_x[i - 1];
    snake_y[i] = snake_y[i - 1];
  }
  snake_x[0] = new_head_x;
  snake_y[0] = new_head_y;
  if (new_head_x >= 0 && new_head_x < FieldSizes::width && new_head_y >= 0 &&
      new_head_y < FieldSizes::height) {
    game_state.field[new_head_y][new_head_x] = 1;
  }
  if (game_state.score >= game_state.level * 5 && game_state.level < 10) {
    game_state.level++;
    game_state.speed = 500 - (game_state.level - 1) * 50;
    if (game_state.speed < 50) {
      game_state.speed = 50;
    }
  }
}

void userInput(UserAction_t action, bool hold) {
  switch (action) {
    case Start:
      if (is_terminated || game_state.pause || !game_state.field) {
        resetGameState();
      } else {
        init_game();
      }
      break;
    case Pause:
      if (!is_terminated && game_state.field) {
        game_state.pause = !game_state.pause;
      }
      break;
    case Terminate:
      cleanup_game();
      break;
    case Left:
      if (!is_terminated && !game_state.pause && direction != 0) {
        direction = 2;
      }
      break;
    case Right:
      if (!is_terminated && !game_state.pause && direction != 2) {
        direction = 0;
      }
      break;
    case Down:
      if (!is_terminated && !game_state.pause && direction != 3) {
        direction = 1;
      }
      break;
    case Up:
      if (!is_terminated && !game_state.pause && direction != 1) {
        direction = 3;
      }
      break;
    case Action:
      if (!hold && !is_terminated && !game_state.pause && !is_speed_boosted) {
        game_state.speed /= 2;
        is_speed_boosted = true;
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        speed_boost_end_time = now;
        speed_boost_end_time.tv_sec += 1;
      }
      break;
  }
}

void forceUpdate() {
  move_snake();
  clock_gettime(CLOCK_MONOTONIC, &last_move_time);
}

GameInfo_t updateCurrentState() {
  if (!timer_initialized || is_terminated || game_state.pause)
    return game_state;
  struct timespec current_time;
  clock_gettime(CLOCK_MONOTONIC, &current_time);
  long long elapsed_ms =
      (current_time.tv_sec - last_move_time.tv_sec) * 1000LL +
      (current_time.tv_nsec - last_move_time.tv_nsec) / 1000000LL;
  if (is_speed_boosted) {
    if (current_time.tv_sec > speed_boost_end_time.tv_sec ||
        (current_time.tv_sec == speed_boost_end_time.tv_sec &&
         current_time.tv_nsec >= speed_boost_end_time.tv_nsec)) {
      game_state.speed = base_speed;
      is_speed_boosted = false;
    }
  }
  if (elapsed_ms >= game_state.speed && !game_state.pause) {
    move_snake();
    last_move_time = current_time;
  }
  return game_state;
}

void resetGameState() {
  cleanup_game();
  snake_length = 4;
  direction = 0;
  for (int i = 0; i < snake_length; ++i) {
    snake_x[i] = 5 - i;
    snake_y[i] = 10;
  }
  is_terminated = 0;
  init_game();
}

FieldSizes returnSizes() { return FieldSizes(); }

void toggle_boundary_mode() { hard_boundaries = !hard_boundaries; }

bool get_boundary_mode() { return hard_boundaries; }
}  // namespace s21