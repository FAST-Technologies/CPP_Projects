/*
  This is a frontend (CLI) module of BrickGame (Snake) Game
  @author: norreyll (aka F.A.S.T), a student of 21 School from Novosibirsk
*/

#ifndef S21_INTERFACE_H_
#define S21_INTERFACE_H_

#include <ncurses.h>
#include <unistd.h>

#include <cstdint>
#include <ctime>

#include "./../../../snake/s21_snake_finite_state_machine.h"
#include "./../../../snake/s21_snake_logic.h"

namespace s21 {

constexpr int FIELD_OFFSET_X = 5;
constexpr int FIELD_OFFSET_Y = 2;

void cli_init(void);
void cli_cleanup(void);
void draw_field(WINDOW *win, const GameInfo_t *game);
void draw_sidebar(WINDOW *win, const GameInfo_t *game);

void process_input(int ch);
void draw_initial_screen(void);
void draw_game_screen(const GameInfo_t *game);
void cli_run(void);

extern WINDOW *game_win;
extern WINDOW *score_win;
}  // namespace s21

#endif  // S21_INTERFACE_H_