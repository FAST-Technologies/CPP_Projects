#include "s21_tetris_finite_state_machine.h"

namespace s21 {

GameState current_state = GameState::kStateStart;

void FsmInit() { current_state = GameState::kStateStart; }

void handle_state_start(UserAction_t action) {
  if (action == Start) {
    current_state = GameState::kStateMoving;
    userInput(Start, false);
  } else if (action == Terminate) {
    current_state = GameState::kStateGameOver;
    userInput(Terminate, false);
  }
}

void handle_state_spawn() { current_state = GameState::kStateMoving; }

void handle_moving_action(UserAction_t action) {
  switch (action) {
    case Pause:
      current_state = GameState::kStatePaused;
      userInput(Pause, false);
      break;
    case Terminate:
      current_state = GameState::kStateGameOver;
      userInput(Terminate, false);
      break;
    case Left:
    case Right:
    case Down:
    case Action:
      userInput(action, false);
      if (is_terminated) {
        current_state = GameState::kStateGameOver;
      }
      break;
    default:
      break;
  }
}

void handle_state_moving(UserAction_t action) {
  if (is_terminated) {
    current_state = GameState::kStateGameOver;
    return;
  }
  handle_moving_action(action);
}

void handle_state_paused(UserAction_t action) {
  if (action == Pause) {
    current_state = GameState::kStateMoving;
    userInput(Pause, false);
  } else if (action == Terminate) {
    current_state = GameState::kStateGameOver;
    userInput(Terminate, false);
  }
}

void handle_state_game_over(UserAction_t action) {
  if (action == Start) {
    current_state = GameState::kStateStart;
    FsmTransition(Start);
  } else if (action == Terminate) {
    userInput(Terminate, false);
  }
}

void FsmTransition(UserAction_t action) {
  switch (current_state) {
    case GameState::kStateStart:
      handle_state_start(action);
      break;
    case GameState::kStateSpawn:
      handle_state_spawn();
      break;
    case GameState::kStateMoving:
      handle_state_moving(action);
      break;
    case GameState::kStatePaused:
      handle_state_paused(action);
      break;
    case GameState::kStateGameOver:
      handle_state_game_over(action);
      break;
  }
}

}  // namespace s21