/*
  This is a backend (Finite State Machine) module of BrickGame (Tetris) Game
  @author: norreyll (aka F.A.S.T), a student of 21 School from Novosibirsk
*/

#ifndef S21_FINITE_STATE_MACHINE_H_
#define S21_FINITE_STATE_MACHINE_H_
#include "s21_tetris_logic.h"

namespace s21 {

typedef enum {
  kStateStart,    // Initial state before game starts.
  kStateSpawn,    // Spawning a new tetromino.
  kStateMoving,   // Tetromino is moving and controlled by player.
  kStatePaused,   // Game is paused.
  kStateGameOver  // Game has ended.
} GameState;

void FsmInit(void);

void handle_state_start(UserAction_t action);
void handle_state_spawn(void);
void handle_moving_action(UserAction_t action);
void handle_state_moving(UserAction_t action);
void handle_state_paused(UserAction_t action);
void handle_state_game_over(UserAction_t action);
void FsmTransition(UserAction_t action);

extern GameState current_state;

}  // namespace s21

#endif  // S21_FINITE_STATE_MACHINE_H_