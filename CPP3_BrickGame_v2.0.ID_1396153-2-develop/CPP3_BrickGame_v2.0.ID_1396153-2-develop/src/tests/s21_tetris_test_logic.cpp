#include <gtest/gtest.h>

#include <cstdlib>

#include "../brick_game/tetris/s21_tetris_finite_state_machine.h"
#include "../brick_game/tetris/s21_tetris_logic.h"

using namespace s21;

void ResetStateForTest() {
  cleanup_game();
  init_game();
  FsmInit();
}

TEST(TetrisLogicTest, UserInputStart) {
  ResetStateForTest();
  userInput(Start, false);
  GameInfo_t game = updateCurrentState();
  EXPECT_NE(game.field, nullptr);
}

TEST(TetrisLogicTest, UserInputPause) {
  ResetStateForTest();
  userInput(Start, false);
  userInput(Pause, false);
  GameInfo_t game = updateCurrentState();
  EXPECT_EQ(game.pause, 1);
}

TEST(TetrisLogicTest, ScoreIncrease) {
  ResetStateForTest();
  userInput(Start, false);
  GameInfo_t game = updateCurrentState();
  int initial_score = game.score;

  for (int i = 0; i < 10; ++i) {
    game.field[19][i] = 1;
  }
  fix_tetromino();
  game = updateCurrentState();
  EXPECT_EQ(game.score, initial_score + 100);
}

TEST(FiniteStateMachineTest, FsmInit) {
  ResetStateForTest();
  FsmInit();
  GameInfo_t game = updateCurrentState();
  EXPECT_NE(game.field, nullptr);
}

TEST(TetrisLogicTest, UserInputMoveLeft) {
  ResetStateForTest();
  userInput(Start, false);
  Tetromino initial = get_current_tetromino();
  userInput(Left, false);
  Tetromino after = get_current_tetromino();
  if (!collides(after)) {
    EXPECT_EQ(after.x, initial.x - 1);
  }
}

TEST(TetrisLogicTest, UserInputMoveRight) {
  ResetStateForTest();
  userInput(Start, false);
  Tetromino initial = get_current_tetromino();
  userInput(Right, false);
  Tetromino after = get_current_tetromino();
  if (!collides(after)) {
    EXPECT_EQ(after.x, initial.x + 1);
  }
}

TEST(TetrisLogicTest, UserInputRotate) {
  ResetStateForTest();
  userInput(Start, false);
  Tetromino initial = get_current_tetromino();
  userInput(Action, false);
  Tetromino after = get_current_tetromino();
  int changed = 0;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (initial.shape[i][j] != after.shape[i][j]) {
        changed = 1;
        break;
      }
    }
    if (changed) break;
  }
}

TEST(TetrisLogicTest, UserInputHardDrop) {
  ResetStateForTest();
  userInput(Start, false);
  Tetromino temp = {
      4,
      0,
      0,
      {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}}  // shape
  };
  current = temp;
  get_current_tetromino();
  for (int y = 0; y < FIELD_HEIGHT; y++) {
    for (int x = 0; x < FIELD_WIDTH; x++) {
      game_state.field[y][x] = 0;
    }
  }
  userInput(Down, true);
  bool placed = false;
  for (int y = FIELD_HEIGHT - 2; y < FIELD_HEIGHT; y++) {
    for (int x = 4; x < 6; x++) {
      if (game_state.field[y][x] == 1) {
        placed = true;
      }
    }
  }
  EXPECT_TRUE(placed) << "Hard drop did not place tetromino at the bottom";
  GameInfo_t game = updateCurrentState();
  EXPECT_NE(game.field, nullptr);
}

TEST(TetrisLogicTest, FixTetrominoMultiLines) {
  ResetStateForTest();
  userInput(Start, false);
  GameInfo_t game = updateCurrentState();
  int initial_score = game.score;
  for (int i = 0; i < 10; ++i) {
    game.field[17][i] = 1;
    game.field[18][i] = 1;
    game.field[19][i] = 1;
  }
  fix_tetromino();
  game = updateCurrentState();
  EXPECT_EQ(game.score, initial_score + 700);
}

TEST(TetrisLogicTest, CollidesTopBoundary) {
  ResetStateForTest();
  userInput(Start, false);
  Tetromino t = get_current_tetromino();
  t.y = -2;
  EXPECT_TRUE(collides(t));
}

TEST(TetrisLogicTest, ReturnSizes) {
  FieldSizes sizes = return_sizes();
  EXPECT_EQ(sizes.width, 10);
  EXPECT_EQ(sizes.height, 20);
}

TEST(TetrisLogicTest, SetTetrominoPosition) {
  ResetStateForTest();
  userInput(Start, false);
  set_tetromino_position(5, 5);
  Tetromino t = get_current_tetromino();
  EXPECT_EQ(t.x, 5);
  EXPECT_EQ(t.y, 5);
}

TEST(TetrisLogicTest, CleanupGame) {
  ResetStateForTest();
  userInput(Start, false);
  cleanup_game();
  GameInfo_t game = get_game_state();
  EXPECT_EQ(game.field, nullptr);
}

TEST(TetrisLogicTest, UserInputMoveDown) {
  ResetStateForTest();
  userInput(Start, false);
  Tetromino initial = get_current_tetromino();
  userInput(Down, false);
  Tetromino after = get_current_tetromino();
  if (!collides(after)) {
    EXPECT_EQ(after.y, initial.y + 1);
  }
}

TEST(TetrisLogicTest, UserInputUp) {
  ResetStateForTest();
  userInput(Start, false);
  userInput(Up, false);
  GameInfo_t game = updateCurrentState();
  EXPECT_NE(game.field, nullptr);
}

TEST(TetrisLogicTest, UpdateCurrentStateFallCollision) {
  ResetStateForTest();
  userInput(Start, false);
  current.y = FIELD_HEIGHT - 1;
  last_fall_time.tv_sec -= 1;
  updateCurrentState();
  EXPECT_EQ(is_terminated, 0);
}

TEST(TetrisLogicTest, UpdateCurrentStateNoField) {
  if (game_state.field) cleanup_game();
  EXPECT_EQ(game_state.field, nullptr);
  GameInfo_t game = updateCurrentState();
  EXPECT_EQ(is_terminated, 0);
  EXPECT_EQ(game.score, 0);
  EXPECT_EQ(game.field, nullptr);
}

TEST(FiniteStateMachineTest, TransitionGameOverRestart) {
  ResetStateForTest();
  FsmInit();
  FsmTransition(Start);
  is_terminated = 0;
  FsmTransition(Start);
  EXPECT_EQ(current_state, kStateMoving);
  GameInfo_t game = get_game_state();
  EXPECT_NE(game.field, nullptr);
}

TEST(TetrisLogicTest, FixTetrominoFourLines) {
  ResetStateForTest();
  userInput(Start, false);
  GameInfo_t game = updateCurrentState();
  int initial_score = game.score;
  for (int i = 0; i < 10; ++i) {
    game.field[16][i] = 1;
    game.field[17][i] = 1;
    game.field[18][i] = 1;
    game.field[19][i] = 1;
  }
  fix_tetromino();
  game = updateCurrentState();
  EXPECT_EQ(game.score, initial_score + 1500);
}

TEST(TetrisLogicTest, UpdateCurrentStateFall) {
  ResetStateForTest();
  userInput(Start, false);
  last_fall_time.tv_sec -= 1;
  updateCurrentState();
  Tetromino after = get_current_tetromino();
  EXPECT_EQ(after.y, 1);
}

TEST(TetrisLogicTest, MultiLineClear) {
  ResetStateForTest();
  userInput(Start, false);
  GameInfo_t game = updateCurrentState();
  int initial_score = game.score;
  for (int i = 0; i < 10; ++i) {
    game.field[18][i] = 1;
    game.field[19][i] = 1;
  }
  fix_tetromino();
  game = updateCurrentState();
  EXPECT_EQ(game.score, initial_score + 300);
}

TEST(TetrisLogicTest, UserInputTerminate) {
  ResetStateForTest();
  userInput(Start, false);
  GameInfo_t game = get_game_state();
  EXPECT_NE(game.field, nullptr);
  userInput(Terminate, false);
  game = get_game_state();
  EXPECT_EQ(is_terminated, 1);
  EXPECT_EQ(game.field, nullptr);
}

TEST(TetrisLogicTest, LevelUp) {
  ResetStateForTest();
  userInput(Start, false);
  GameInfo_t game = updateCurrentState();
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      game.field[19][j] = 1;
    }
    fix_tetromino();
    game = updateCurrentState();
  }
  EXPECT_EQ(game.level, 2);
  EXPECT_EQ(game.speed, 450);
}

TEST(FiniteStateMachineTest, TransitionMovingActions) {
  ResetStateForTest();
  FsmInit();
  FsmTransition(Start);
  Tetromino initial = get_current_tetromino();
  FsmTransition(Left);
  Tetromino after_left = get_current_tetromino();
  if (!collides(after_left)) {
    EXPECT_EQ(after_left.x, initial.x - 1);
  }
  FsmTransition(Right);
  Tetromino after_right = get_current_tetromino();
  if (!collides(after_right)) {
    EXPECT_EQ(after_right.x, initial.x);
  }
  FsmTransition(Down);
  Tetromino after_down = get_current_tetromino();
  if (!collides(after_down)) {
    EXPECT_EQ(after_down.y, initial.y + 1);
  }
  FsmTransition(Action);
  Tetromino after_rotate = get_current_tetromino();
  int changed = 0;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (initial.shape[i][j] != after_rotate.shape[i][j]) {
        changed = 1;
        break;
      }
    }
    if (changed) break;
  }
}

TEST(TetrisLogicTest, FixTetrominoHighScore) {
  ResetStateForTest();
  userInput(Start, false);
  GameInfo_t game = updateCurrentState();
  int initial_high_score = game.high_score;
  game_state.score = initial_high_score + 100;
  for (int i = 0; i < 10; ++i) {
    game_state.field[19][i] = 1;
  }
  fix_tetromino();
  game = updateCurrentState();
  EXPECT_EQ(game.high_score, game.score);
}

TEST(FiniteStateMachineTest, TransitionStart) {
  FsmInit();
  FsmTransition(Start);
  EXPECT_EQ(current_state, kStateMoving);
  GameInfo_t game = get_game_state();
  EXPECT_NE(game.field, nullptr);
}

TEST(FiniteStateMachineTest, TransitionPause) {
  ResetStateForTest();
  FsmInit();
  FsmTransition(Start);
  FsmTransition(Pause);
  GameInfo_t game = get_game_state();
  EXPECT_EQ(game.pause, 1);
  EXPECT_EQ(current_state, kStatePaused);
  FsmTransition(Pause);
  game = get_game_state();
  EXPECT_EQ(game.pause, 0);
  EXPECT_EQ(current_state, kStateMoving);
}

TEST(FiniteStateMachineTest, TransitionTerminate) {
  ResetStateForTest();
  FsmInit();
  FsmTransition(Start);
  FsmTransition(Terminate);
  EXPECT_EQ(current_state, kStateGameOver);
  EXPECT_EQ(is_terminated, 1);
}

TEST(FiniteStateMachineTest, TransitionMovingTermination) {
  ResetStateForTest();
  FsmInit();
  FsmTransition(Start);
  is_terminated = 1;
  FsmTransition(Left);
  EXPECT_EQ(current_state, kStateGameOver);
}

TEST(FiniteStateMachineTest, TransitionGameOverStart) {
  ResetStateForTest();
  FsmInit();
  FsmTransition(Start);
  is_terminated = 1;
  FsmTransition(Terminate);
  FsmTransition(Start);
  EXPECT_EQ(current_state, kStateMoving);
  GameInfo_t game = get_game_state();
  EXPECT_NE(game.field, nullptr);
}

TEST(FiniteStateMachineTest, Transition) {
  ResetStateForTest();
  FsmInit();
  FsmTransition(Start);
  current.y = FIELD_HEIGHT - 1;
  Tetromino temp = current;
  temp.y++;
  if (collides(temp)) {
    fix_tetromino();
  }
  EXPECT_EQ(is_terminated, 0);
  FsmTransition(Left);
  EXPECT_EQ(current_state, kStateMoving);
}

TEST(FiniteStateMachineTest, TransitionSpawn) {
  ResetStateForTest();
  FsmInit();
  current_state = kStateSpawn;
  FsmTransition((UserAction_t)0);
  EXPECT_EQ(current_state, kStateMoving);
  GameInfo_t game = get_game_state();
  EXPECT_NE(game.field, nullptr);
}

TEST(FiniteStateMachineTest, TransitionStartTerminate) {
  ResetStateForTest();
  FsmInit();
  FsmTransition(Terminate);
  EXPECT_EQ(current_state, kStateGameOver);
  EXPECT_EQ(is_terminated, 1);
}

TEST(FiniteStateMachineTest, TransitionPausedTerminate) {
  ResetStateForTest();
  FsmInit();
  FsmTransition(Start);
  FsmTransition(Pause);
  FsmTransition(Terminate);
  EXPECT_EQ(current_state, kStateGameOver);
  EXPECT_EQ(is_terminated, 1);
}

TEST(FiniteStateMachineTest, TransitionGameoverTerminate) {
  ResetStateForTest();
  FsmInit();
  FsmTransition(Start);
  is_terminated = 1;
  FsmTransition(Terminate);
  EXPECT_EQ(current_state, kStateGameOver);
  EXPECT_EQ(is_terminated, 1);
}

TEST(FiniteStateMachineTest, HandleMovingActionTermination) {
  ResetStateForTest();
  FsmInit();
  FsmTransition(Start);
  is_terminated = 1;
  FsmTransition(Left);
  EXPECT_EQ(current_state, kStateGameOver);
}

TEST(FiniteStateMachineTest, HandleMovingActionDefault) {
  ResetStateForTest();
  FsmInit();
  FsmTransition(Start);
  FsmTransition(static_cast<UserAction_t>(100));
  EXPECT_EQ(current_state, kStateMoving);
}

TEST(FiniteStateMachineTest, HandleGameOverStart) {
  ResetStateForTest();
  FsmInit();
  current_state = kStateGameOver;
  is_terminated = 1;
  FsmTransition(Start);
  EXPECT_EQ(current_state, kStateMoving);
  EXPECT_EQ(is_terminated, 0);
  EXPECT_NE(game_state.field, nullptr);
}

TEST(TetrisLogicTest, ForceCollisionAtBottom) {
  ResetStateForTest();
  userInput(Start, false);
  Tetromino test_tetro = current;
  test_tetro.y = FIELD_HEIGHT - 1;
  EXPECT_TRUE(collides(test_tetro));
}

TEST(TetrisLogicTest, FieldReinitAfterCleanup) {
  ResetStateForTest();
  userInput(Start, false);
  game_state.field[0][0] = 1;
  EXPECT_EQ(game_state.field[0][0], 1);
  cleanup_game();
  init_game();
  EXPECT_EQ(game_state.field[0][0], 0);
}

TEST(FiniteStateMachineTest, HandleGameOverTerminate) {
  ResetStateForTest();
  FsmInit();
  current_state = kStateGameOver;
  FsmTransition(Terminate);
  EXPECT_EQ(current_state, kStateGameOver);
}

TEST(TetrisLogicTest, InitGameCleanup) {
  ResetStateForTest();
  userInput(Start, false);
  EXPECT_NE(game_state.field, nullptr);
  init_game();
  EXPECT_NE(game_state.field, nullptr);
}

TEST(TetrisLogicTest, InitGameMemoryFailure) {
  ResetStateForTest();
  EXPECT_NO_THROW(init_game());
}

TEST(TetrisLogicTest, UserInputInitOnStart) {
  cleanup_game();
  EXPECT_EQ(game_state.field, nullptr);
  userInput(Start, false);
  EXPECT_NE(game_state.field, nullptr);
}

TEST(TetrisLogicTest, UpdateCurrentStateFallNoCollision) {
  ResetStateForTest();
  userInput(Start, false);
  last_fall_time.tv_sec -= 2;
  Tetromino initial = get_current_tetromino();
  updateCurrentState();
  Tetromino after = get_current_tetromino();
  if (!collides(after)) {
    EXPECT_EQ(after.y, initial.y + 1);
  }
}

TEST(TetrisLogicTest, ResetGameState) {
  ResetStateForTest();
  userInput(Start, false);
  game_state.score = 100;
  reset_game_state();
  EXPECT_EQ(game_state.score, 0);
  EXPECT_NE(game_state.field, nullptr);
}

TEST(TetrisLogicTest, InitGameNormal) {
  cleanup_game();
  EXPECT_EQ(game_state.field, nullptr);
  init_game();
  EXPECT_NE(game_state.field, nullptr);
  EXPECT_NE(game_state.next, nullptr);
  EXPECT_EQ(game_state.score, 0);
  EXPECT_EQ(game_state.level, 1);
  EXPECT_EQ(game_state.speed, 500);
  EXPECT_EQ(game_state.pause, 0);
  EXPECT_EQ(is_terminated, 0);
  EXPECT_NE(current.x, -1);
  EXPECT_NE(next.x, -1);
}

TEST(TetrisLogicTest, InitGameWithExistingField) {
  ResetStateForTest();
  EXPECT_NE(game_state.field, nullptr);
  init_game();
  EXPECT_NE(game_state.field, nullptr);
  EXPECT_EQ(game_state.score, 0);
  EXPECT_EQ(is_terminated, 0);
  GameInfo_t game = get_game_state();
  EXPECT_EQ(game.field[0][0], 0);
}

#ifdef TEST_EXCEPTION_MODE
TEST(TetrisLogicTest, InitGameFieldAllocationFailure) {
  cleanup_game();
  simulate_allocation_failure = true;
  failure_point = 1;
  EXPECT_THROW(init_game(), std::runtime_error);
  EXPECT_EQ(game_state.field, nullptr);
  simulate_allocation_failure = false;
}

TEST(TetrisLogicTest, InitGameFieldRowAllocationFailure) {
  cleanup_game();
  simulate_allocation_failure = true;
  failure_point = 2;
  EXPECT_THROW(init_game(), std::runtime_error);
  EXPECT_EQ(game_state.field, nullptr);
  simulate_allocation_failure = false;
}

TEST(TetrisLogicTest, InitGameNextAllocationFailure) {
  cleanup_game();
  simulate_allocation_failure = true;
  failure_point = 3;
  EXPECT_THROW(init_game(), std::runtime_error);
  EXPECT_EQ(game_state.next, nullptr);
  simulate_allocation_failure = false;
}

TEST(TetrisLogicTest, InitGameNextRowAllocationFailure) {
  cleanup_game();
  simulate_allocation_failure = true;
  failure_point = 4;
  EXPECT_THROW(init_game(), std::runtime_error);
  EXPECT_EQ(game_state.next, nullptr);
  simulate_allocation_failure = false;
}
#endif

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}