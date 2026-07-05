/*
  This is a frontend (CLI) module of BrickGame (Tetris) Game
  @author: norreyll (aka F.A.S.T), a student of 21 School from Novosibirsk
*/

#ifndef S21_INTERFACE_H_
#define S21_INTERFACE_H_

#include <ncurses.h>
#include <unistd.h>

#include <cstdlib>
#include <ctime>
#include <vector>

#include "./../../../tetris/s21_tetris_finite_state_machine.h"
#include "./../../../tetris/s21_tetris_logic.h"

namespace s21 {

constexpr int FIELD_OFFSET_X = 2;
constexpr int FIELD_OFFSET_Y = 1;

void cli_init();
void cli_cleanup();
void draw_field(WINDOW* win, const GameInfo_t& game);
void draw_sidebar(WINDOW* win, const GameInfo_t& game);
void draw_next(WINDOW* win, const GameInfo_t& game);

void process_input(int ch);
void draw_initial_screen();
void draw_game_screen(const GameInfo_t& game);
void cli_run();

extern WINDOW* game_win;
extern WINDOW* score_win;
extern WINDOW* next_win;
extern SCREEN* game_screen;

}  // namespace s21

#endif  // S21_INTERFACE_H_