#include "s21_snake_finite_state_machine.h"

namespace s21 {

GameState current_state = GameState::kStateStart;

// Inites state ising kStateStart.
void FsmInit() { current_state = GameState::kStateStart; }

// Inites state ising kStateStart.
void handle_state_start(UserAction_t action) {
  if (action == Start) {
    current_state = GameState::kStateMoving;
    userInput(Start, false);
  } else if (action == Terminate) {
    current_state = GameState::kStateGameOver;
    userInput(Terminate, false);
  }
}

// Handles transitions from kStateMoving.
void handle_state_moving(UserAction_t action) {
  userInput(action, false);
  updateCurrentState();
  if (is_terminated) {
    current_state = GameState::kStateGameOver;
    // return;
  }
  if (action == Pause) {
    current_state = GameState::kStatePaused;
  } else if (action == Terminate) {
    current_state = GameState::kStateGameOver;
  }
}

// Handles transitions from kStatePaused.
void handle_state_paused(UserAction_t action) {
  if (action == Pause) {
    current_state = GameState::kStateMoving;
    userInput(Pause, false);
  } else if (action == Terminate) {
    current_state = GameState::kStateGameOver;
    userInput(Terminate, false);
  }
}

// Handles transitions from kStateGameOver.
void handle_state_game_over(UserAction_t action) {
  if (action == Start) {
    current_state = GameState::kStateStart;
    userInput(Start, false);
  } else if (action == Terminate) {
    userInput(Terminate, false);
  }
}

// Main function to transition between states.
void FsmTransition(UserAction_t action) {
  switch (current_state) {
    case GameState::kStateStart:
      handle_state_start(action);
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