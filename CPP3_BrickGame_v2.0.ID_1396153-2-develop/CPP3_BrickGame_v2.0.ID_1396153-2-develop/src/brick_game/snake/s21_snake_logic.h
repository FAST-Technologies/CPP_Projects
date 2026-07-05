/*
  This is a backend (Logic) module of BrickGame (Snake) Game
  @author: norreyll (aka F.A.S.T), a student of 21 School from Novosibirsk
*/

#ifndef S21_LOGIC_H_
#define S21_LOGIC_H_

#include <unistd.h>

#include <cstdbool>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>

namespace s21 {

typedef enum {
  Start,
  Pause,
  Terminate,
  Left,
  Right,
  Up,
  Down,
  Action
} UserAction_t;

typedef struct {
  int **field;
  int **next;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
} GameInfo_t;

struct FieldSizes {
  static constexpr int width = 10;
  static constexpr int height = 20;
};

extern int snake_x[200];
extern int snake_y[200];
extern int snake_length;
extern int food_x;
extern int food_y;
extern int direction;
extern int is_terminated;
extern struct timespec last_move_time;
extern GameInfo_t game_state;
extern bool hard_boundaries;
extern int base_speed;
extern bool is_speed_boosted;
extern struct timespec speed_boost_end_time;
extern const std::string HIGH_SCORE_FILE;

void init_game();
void cleanup_game();
void generate_food();
void move_snake();
void userInput(UserAction_t action, bool hold);
FieldSizes returnSizes();
void resetGameState();
GameInfo_t updateCurrentState();
void toggle_boundary_mode();
bool get_boundary_mode();
void forceUpdate();

void set_interface_mode(bool desktop);
std::string get_high_score_file();

}  // namespace s21

#endif  // S21_LOGIC_H_