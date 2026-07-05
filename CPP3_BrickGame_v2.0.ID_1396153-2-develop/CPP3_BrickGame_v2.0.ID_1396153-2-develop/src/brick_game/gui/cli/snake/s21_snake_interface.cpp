#include "s21_snake_interface.h"

namespace s21 {
WINDOW* game_win = nullptr;
WINDOW* score_win = nullptr;

void cli_init() {
  initscr();
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
  init_pair(1, COLOR_GREEN, COLOR_BLACK);  // Snake
  init_pair(2, COLOR_RED, COLOR_BLACK);    // Food
  init_pair(3, COLOR_WHITE, COLOR_BLACK);  // Normal text
  init_pair(4, COLOR_CYAN, COLOR_BLACK);   // Borders
  refresh();
}

void cli_cleanup() {
  if (game_win) {
    delwin(game_win);
    game_win = nullptr;
  }
  if (score_win) {
    delwin(score_win);
    score_win = nullptr;
  }
  endwin();
}

void create_game_windows() {
  if (!game_win) {
    game_win = newwin(22, 22, FIELD_OFFSET_Y, FIELD_OFFSET_X);
    if (!game_win) {
      endwin();
      printf("Failed to create game window\n");
      exit(1);
    }
    wbkgd(game_win, COLOR_PAIR(3));
  }

  if (!score_win) {
    score_win = newwin(17, 25, FIELD_OFFSET_Y, FIELD_OFFSET_X + 24);
    if (!score_win) {
      endwin();
      printf("Failed to create score window\n");
      exit(1);
    }
    wbkgd(score_win, COLOR_PAIR(3));
  }
}

void destroy_game_windows() {
  if (game_win) {
    delwin(game_win);
    game_win = nullptr;
  }
  if (score_win) {
    delwin(score_win);
    score_win = nullptr;
  }
}

void draw_field(WINDOW* win, const GameInfo_t* game) {
  if (!win) return;
  werase(win);
  wattron(win, COLOR_PAIR(4));
  box(win, 0, 0);
  wattroff(win, COLOR_PAIR(4));
  wattron(win, COLOR_PAIR(3));
  for (int y = 0; y < FieldSizes::height; ++y) {
    for (int x = 0; x < FieldSizes::width; ++x) {
      mvwaddch(win, y + 1, x * 2 + 1, '.');
      mvwaddch(win, y + 1, x * 2 + 2, '.');
    }
  }
  wattroff(win, COLOR_PAIR(3));
  if (game->field) {
    for (int y = 0; y < FieldSizes::height; ++y) {
      for (int x = 0; x < FieldSizes::width; ++x) {
        if (game->field[y][x] == 1) {  // Змея
          wattron(win, COLOR_PAIR(1));
          mvwaddch(win, y + 1, x * 2 + 1, ACS_BLOCK);
          mvwaddch(win, y + 1, x * 2 + 2, ACS_BLOCK);
          wattroff(win, COLOR_PAIR(1));
        } else if (game->field[y][x] == 2) {  // Еда
          wattron(win, COLOR_PAIR(2));
          mvwaddch(win, y + 1, x * 2 + 1, 'O');
          mvwaddch(win, y + 1, x * 2 + 2, ' ');
          wattroff(win, COLOR_PAIR(2));
        }
      }
    }
  } else {
    mvwprintw(win, 10, 5, "Field is NULL");
  }
  wnoutrefresh(win);
}

void draw_sidebar(WINDOW* win, const GameInfo_t* game) {
  if (!win) return;
  werase(win);
  wattron(win, COLOR_PAIR(4));
  box(win, 0, 0);
  wattroff(win, COLOR_PAIR(4));
  wattron(win, COLOR_PAIR(3));
  mvwprintw(win, 1, 2, "Score: %d", game->score);
  mvwprintw(win, 2, 2, "High Score: %d", game->high_score);
  mvwprintw(win, 3, 2, "Level: %d", game->level);
  mvwprintw(win, 4, 2, "Speed: %dms", game->speed);
  mvwprintw(win, 5, 2, "Boundaries: %s", get_boundary_mode() ? "Hard" : "Soft");
  mvwprintw(win, 7, 2, "          ");
  if (game->pause) {
    wattron(win, COLOR_PAIR(2));
    mvwprintw(win, 7, 2, "PAUSED");
    wattroff(win, COLOR_PAIR(2));
  } else if (is_terminated) {
    wattron(win, COLOR_PAIR(2));
    mvwprintw(win, 7, 2, "GAME OVER");
    wattroff(win, COLOR_PAIR(2));
  } else {
    wattron(win, COLOR_PAIR(1));
    mvwprintw(win, 7, 2, "PLAYING");
    wattroff(win, COLOR_PAIR(1));
  }
  mvwprintw(win, 9, 2, "Controls:");
  mvwprintw(win, 10, 2, "s - Start/Restart");
  mvwprintw(win, 11, 2, "p - Pause");
  mvwprintw(win, 12, 2, "b - Toggle bounds");
  mvwprintw(win, 13, 2, "q - Quit");
  wattroff(win, COLOR_PAIR(3));
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
    case 'b':
      toggle_boundary_mode();
      break;
    case KEY_LEFT:
      FsmTransition(Left);
      break;
    case KEY_RIGHT:
      FsmTransition(Right);
      break;
    case KEY_UP:
      FsmTransition(Up);
      break;
    case KEY_DOWN:
      FsmTransition(Down);
      break;
    case ' ':
      FsmTransition(Action);
      break;
  }
}

void draw_initial_screen() {
  clear();
  attron(COLOR_PAIR(1));
  mvprintw(8, 20, "+==========================+");
  mvprintw(9, 20, "|       SNAKE v2.0         |");
  mvprintw(10, 20, "+==========================+");
  attroff(COLOR_PAIR(1));
  attron(COLOR_PAIR(3));
  mvprintw(12, 18, "Press 's' to start the game");
  mvprintw(13, 18, "Press 'q' to quit");

  mvprintw(15, 18, "Controls:");
  mvprintw(16, 20, "^v<> keys - Move snake");
  mvprintw(17, 20, "Space - Speed boost");
  mvprintw(18, 20, "p - Pause/Resume");
  mvprintw(19, 20, "b - Toggle boundaries");
  mvprintw(20, 20, "s - Start/Restart");

  mvprintw(22, 18, "Boundaries: %s",
           get_boundary_mode() ? "Hard (walls kill)" : "Soft (teleport)");
  attroff(COLOR_PAIR(3));

  refresh();
}

void draw_game_screen(const GameInfo_t* game) {
  create_game_windows();
  clear();
  draw_field(game_win, game);
  draw_sidebar(score_win, game);
  doupdate();
}

void cli_run() {
  FsmInit();
  struct timespec ts = {0, 16666666L};  // ~60 FPS

  while (1) {
    int ch = getch();
    if (ch != ERR) {
      process_input(ch);
    }

    GameInfo_t game = updateCurrentState();

    if (current_state == kStateStart && !game.field) {
      destroy_game_windows();
      draw_initial_screen();
    } else if (game.field) {
      draw_game_screen(&game);
      if (is_terminated) {
        attron(COLOR_PAIR(2));
        mvprintw(24, 5, "GAME OVER! Press 's' to restart or 'q' to quit");
        attroff(COLOR_PAIR(2));
        refresh();
      }
    } else {
      destroy_game_windows();
      draw_initial_screen();
    }
    if (current_state == kStateGameOver && is_terminated == 1 &&
        game_state.field == nullptr) {
      break;
    }

    nanosleep(&ts, NULL);
  }
}

}  // namespace s21

int main() {
  s21::set_interface_mode(false);
  s21::cli_init();
  s21::cli_run();
  s21::cli_cleanup();
  return 0;
}