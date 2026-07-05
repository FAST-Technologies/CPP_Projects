/*
  This is a backend (Logic) module of BrickGame (Tetris) Game
  @author: norreyll (aka F.A.S.T), a student of 21 School from Novosibirsk
*/

#ifndef S21_LOGIC_H_
#define S21_LOGIC_H_

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
  int** field;
  int** next;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
} GameInfo_t;

typedef struct {
  int x, y;
  int rotation;
  int shape[4][4];
} Tetromino;

typedef struct {
  int width;
  int height;
} FieldSizes;

extern const int SHAPES[7][4][4][4];
extern Tetromino current;
extern Tetromino next;
extern GameInfo_t game_state;
extern const int FIELD_WIDTH;
extern const int FIELD_HEIGHT;
extern int is_terminated;
extern struct timespec last_fall_time;

void init_game(void);
void cleanup_game(void);
bool collides(const Tetromino& tetro);
void rotate_tetromino(Tetromino* tetro);

void place_tetromino(void);
int clear_full_lines(void);
void update_score(int lines_cleared);
void update_level(void);
void reset_tetrominos(void);
void check_termination(void);
void fix_tetromino(void);

void handle_start_action(void);
void handle_pause_action(void);
void handle_horizontal_move(int dx, bool hold);
void handle_down_action(bool hold);
void handle_action_rotate(bool hold);
void userInput(UserAction_t action, bool hold);

GameInfo_t updateCurrentState(void);
void reset_game_state(void);
Tetromino get_current_tetromino(void);
GameInfo_t get_game_state(void);
FieldSizes return_sizes(void);
void set_tetromino_position(int x, int y);
Tetromino generate_random_tetromino();

void set_interface_mode(bool desktop);
std::string get_high_score_file();
}  // namespace s21

#endif  // S21_LOGIC_H_