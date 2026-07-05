#define _POSIX_C_SOURCE 200809L
#include "s21_tetris_interface.h"

#include <iostream>

namespace s21 {

WINDOW* game_win = nullptr;
WINDOW* score_win = nullptr;
WINDOW* next_win = nullptr;
SCREEN* game_screen = nullptr;

void cli_init() {
  game_screen = newterm(nullptr, stdout, stdin);
  if (!game_screen)
    throw std::runtime_error("Failed to initialize ncurses screen");
  set_term(game_screen);
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  nodelay(stdscr, TRUE);
  if (LINES < 25 || COLS < 50) {
    endwin();
    printf("Terminal too small! Need at least 50x25 characters.\n");
    exit(1);
  }
  start_color();
  init_pair(1, COLOR_CYAN, COLOR_BLACK);
  init_pair(2, COLOR_RED, COLOR_BLACK);
  game_win = newwin(22, 22, FIELD_OFFSET_Y, FIELD_OFFSET_X);
  if (!game_win) {
    endwin();
    printf("Failed to create game window\n");
    exit(1);
  }
  score_win = newwin(16, 23, FIELD_OFFSET_Y, FIELD_OFFSET_X + 24);
  if (!score_win) {
    endwin();
    printf("Failed to create score window\n");
    exit(1);
  }
  next_win = newwin(6, 10, FIELD_OFFSET_Y + 16, FIELD_OFFSET_X + 24);
  if (!next_win) {
    endwin();
    printf("Failed to create next window\n");
    exit(1);
  }
  if (!game_win || !score_win || !next_win)
    throw std::runtime_error("Failed to create windows");
  wbkgd(game_win, COLOR_PAIR(0) | ' ');
}

void cli_cleanup() {
  cleanup_game();
  if (game_win) {
    delwin(game_win);
    game_win = nullptr;
  }
  if (score_win) {
    delwin(score_win);
    score_win = nullptr;
  }
  if (next_win) {
    delwin(next_win);
    next_win = nullptr;
  }
  if (stdscr) {
    delwin(stdscr);
  }
  if (game_screen) {
    endwin();
    delscreen(game_screen);
    game_screen = nullptr;
  }
}

void draw_field(WINDOW* win, const GameInfo_t& game) {
  werase(win);
  box(win, 0, 0);
  wattron(win, COLOR_PAIR(1));
  for (int y = 0; y < FIELD_HEIGHT; ++y) {
    for (int x = 0; x < FIELD_WIDTH; ++x) {
      if (game.field[y][x]) {
        mvwaddch(win, y + 1, x * 2 + 1, ACS_BLOCK);
        mvwaddch(win, y + 1, x * 2 + 2, ACS_BLOCK);
      }
    }
  }
  wattroff(win, COLOR_PAIR(1));
  Tetromino active = get_current_tetromino();
  wattron(win, COLOR_PAIR(2));
  for (int y = 0; y < 4; ++y) {
    for (int x = 0; x < 4; ++x) {
      if (active.shape[y][x]) {
        int fx = active.x + x;
        int fy = active.y + y;
        if (fx >= 0 && fx < FIELD_WIDTH && fy >= 0 && fy < FIELD_HEIGHT) {
          mvwaddch(win, fy + 1, fx * 2 + 1, ACS_BLOCK);
          mvwaddch(win, fy + 1, fx * 2 + 2, ACS_BLOCK);
        }
      }
    }
  }
  wattroff(win, COLOR_PAIR(2));
  wnoutrefresh(win);
}

void draw_sidebar(WINDOW* win, const GameInfo_t& game) {
  werase(win);
  box(win, 0, 0);
  mvwprintw(win, 1, 2, "Score: %d", game.score);
  mvwprintw(win, 2, 2, "High Score: %d", game.high_score);
  mvwprintw(win, 3, 2, "Level: %d", game.level);
  mvwprintw(win, 4, 2, "Speed: %dms", game.speed);
  if (game.pause) {
    mvwprintw(win, 6, 2, "PAUSED");
  }
  mvwprintw(win, 7, 2, "Controls:");
  mvwprintw(win, 8, 2, "s - Start");
  mvwprintw(win, 9, 2, "p - Pause");
  mvwprintw(win, 10, 2, "q - Quit");
  mvwprintw(win, 11, 2, "ARROW keys:Left(<-)");
  mvwprintw(win, 12, 2, "Right(->) or Down");
  mvwprintw(win, 13, 2, "Space - Rotate");
  wnoutrefresh(win);
}

void draw_next(WINDOW* win, const GameInfo_t& game) {
  werase(win);
  box(win, 0, 0);
  mvwprintw(win, 1, 2, "Next:");
  wattron(win, COLOR_PAIR(2));
  for (int y = 0; y < 4; ++y) {
    for (int x = 0; x < 4; ++x) {
      if (game.next[y][x]) {
        mvwaddch(win, y + 2, x * 2 + 1, ACS_BLOCK);
        mvwaddch(win, y + 2, x * 2 + 2, ACS_BLOCK);
      }
    }
  }
  wattroff(win, COLOR_PAIR(2));
  wnoutrefresh(win);
}

void process_input(int ch) {
  switch (ch) {
    case 'q':
      FsmTransition(Terminate);
      break;
    case 's':
      FsmTransition(Start);
      break;
    case 'p':
      FsmTransition(Pause);
      break;
    case KEY_LEFT:
      FsmTransition(Left);
      break;
    case KEY_RIGHT:
      FsmTransition(Right);
      break;
    case KEY_DOWN:
      FsmTransition(Down);
      break;
    case KEY_UP:
      FsmTransition(Up);
      break;
    case ' ':
      FsmTransition(Action);
      break;
  }
}

void draw_initial_screen() {
  werase(stdscr);
  mvprintw(10, 15, "TETRIS v1.0");
  mvprintw(12, 15, "Press 's' to start the game");
  mvprintw(13, 15, "Press 'q' to quit");
  mvprintw(15, 15, "Controls:");
  mvprintw(16, 15, "Arrow keys - Move");
  mvprintw(17, 15, "Space - Rotate");
  mvprintw(18, 15, "p - Pause");
  wnoutrefresh(stdscr);
}

void draw_game_screen(const GameInfo_t& game) {
  draw_field(game_win, game);
  draw_sidebar(score_win, game);
  draw_next(next_win, game);
  if (is_terminated) {
    mvprintw(24, 0, "Game Over! Press 's' to restart or 'q' to quit");
    wnoutrefresh(stdscr);
  }
}

void cli_run() {
  FsmInit();
  struct timespec ts {
    0, 16666666L
  };  // ~60 FPS (16.67ms)
  while (true) {
    int ch = getch();
    if (ch != ERR) {
      process_input(ch);
    }

    GameInfo_t game = updateCurrentState();
    if (is_terminated && !game.field) break;

    werase(stdscr);
    if (!game.field) {
      draw_initial_screen();
    } else {
      draw_game_screen(game);
    }
    doupdate();
    nanosleep(&ts, nullptr);
  }
}

}  // namespace s21

int main() {
  try {
    s21::set_interface_mode(false);
    s21::cli_init();
    s21::cli_run();
    s21::cli_cleanup();
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    s21::cli_cleanup();
    return 1;
  }
  return 0;
}