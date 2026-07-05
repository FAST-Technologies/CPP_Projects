#include <gtest/gtest.h>
#include <unistd.h>

#include <fstream>

#include "../brick_game/snake/s21_snake_finite_state_machine.h"
#include "../brick_game/snake/s21_snake_logic.h"

#ifdef SIMULATE_BAD_ALLOC
#define new (throw std::bad_alloc(); nullptr)
#endif

namespace s21 {

void ResetStateForTest() {
  cleanup_game();
  init_game();
  FsmInit();
  direction = 0;
  is_terminated = 0;
  game_state.pause = 0;
  is_speed_boosted = false;
}

TEST(SnakeLogicTest, InitialState) {
  ResetStateForTest();
  updateCurrentState();
  EXPECT_EQ(snake_length, 4);
  EXPECT_EQ(game_state.score, 0);
  EXPECT_EQ(game_state.level, 1);
  EXPECT_EQ(game_state.speed, 500);
  EXPECT_FALSE(is_terminated);
  EXPECT_EQ(game_state.field[10][5], 1);
  EXPECT_EQ(game_state.field[food_y][food_x], 2);
}

TEST(SnakeLogicTest, MoveRight) {
  ResetStateForTest();
  direction = 0;
  forceUpdate();
  EXPECT_EQ(snake_x[0], 6);
  EXPECT_EQ(game_state.field[10][6], 1);
  EXPECT_EQ(snake_y[0], 10);
}

TEST(SnakeLogicTest, EatFood) {
  ResetStateForTest();
  food_x = snake_x[0] + 1;
  food_y = snake_y[0];
  game_state.field[snake_y[0]][snake_x[0]] = 1;
  game_state.field[food_y][food_x] = 2;
  int initial_length = snake_length;
  int initial_score = game_state.score;
  direction = 0;
  forceUpdate();
  EXPECT_EQ(snake_length, initial_length + 1);
  EXPECT_EQ(game_state.score, initial_score + 1);
  EXPECT_EQ(game_state.field[snake_y[0]][snake_x[0]], 1);
  EXPECT_EQ(game_state.field[food_y][food_x], 2);
}

TEST(SnakeLogicTest, CollisionWithBoundary) {
  ResetStateForTest();
  direction = 0;
  snake_x[0] = FieldSizes::width - 1;
  forceUpdate();
  EXPECT_TRUE(is_terminated);
}

TEST(SnakeLogicTest, CollisionWithSelf) {
  ResetStateForTest();
  snake_x[1] = snake_x[0] + 1;
  snake_y[1] = snake_y[0];
  direction = 0;
  forceUpdate();
  EXPECT_TRUE(is_terminated);
}

TEST(SnakeLogicTest, MaxSnakeLength) {
  ResetStateForTest();
  snake_length = 199;
  for (int i = 0; i < snake_length; ++i) {
    snake_x[i] = 5 - i;
    snake_y[i] = 10;
    if (snake_x[i] >= 0 && snake_x[i] < FieldSizes::width && snake_y[i] >= 0 &&
        snake_y[i] < FieldSizes::height) {
      game_state.field[snake_y[i]][snake_x[i]] = 1;
    }
  }
  food_x = snake_x[0] + 1;
  food_y = snake_y[0];
  game_state.field[food_y][food_x] = 2;
  direction = 0;
  forceUpdate();
  EXPECT_EQ(snake_length, 200);
  EXPECT_TRUE(is_terminated);
}

TEST(SnakeLogicTest, SpeedBoost) {
  ResetStateForTest();
  int original_speed = game_state.speed;
  EXPECT_FALSE(is_speed_boosted);
  userInput(Action, false);
  EXPECT_TRUE(is_speed_boosted);
  EXPECT_EQ(game_state.speed, original_speed / 2);
  struct timespec current_time;
  clock_gettime(CLOCK_MONOTONIC, &current_time);
  speed_boost_end_time.tv_sec = current_time.tv_sec - 2;
  updateCurrentState();
  EXPECT_FALSE(is_speed_boosted);
  EXPECT_EQ(game_state.speed, base_speed);
}

TEST(SnakeLogicTest, GenerateFood) {
  ResetStateForTest();
  game_state.field[food_y][food_x] = 1;
  generate_food();
  EXPECT_EQ(game_state.field[food_y][food_x], 2);
}

TEST(SnakeLogicTest, LevelProgression) {
  ResetStateForTest();
  game_state.score = 5;
  forceUpdate();
  EXPECT_EQ(game_state.level, 2);
  EXPECT_LT(game_state.speed, 500);
}

TEST(SnakeLogicTest, BoundaryModeToggle) {
  ResetStateForTest();
  EXPECT_TRUE(hard_boundaries);
  toggle_boundary_mode();
  EXPECT_FALSE(hard_boundaries);
  toggle_boundary_mode();
  EXPECT_TRUE(hard_boundaries);
}

TEST(SnakeLogicTest, HighScoreSave) {
  ResetStateForTest();
  std::ofstream clear_file(HIGH_SCORE_FILE, std::ofstream::trunc);
  clear_file.close();

  game_state.score = 10;
  game_state.high_score = 5;
  cleanup_game();
  init_game();
  EXPECT_EQ(game_state.high_score, 10);
}

TEST(FiniteStateMachineTest, TransitionStartToMoving) {
  FsmInit();
  EXPECT_EQ(current_state, GameState::kStateStart);
  FsmTransition(Start);
  EXPECT_EQ(current_state, GameState::kStateMoving);
  EXPECT_FALSE(is_terminated);
  EXPECT_TRUE(game_state.field != nullptr);
}

TEST(FiniteStateMachineTest, TransitionMovingToPaused) {
  FsmInit();
  FsmTransition(Start);
  EXPECT_EQ(current_state, GameState::kStateMoving);
  FsmTransition(Pause);
  EXPECT_EQ(current_state, GameState::kStatePaused);
  EXPECT_TRUE(game_state.pause);
}

TEST(FiniteStateMachineTest, TransitionPausedToMoving) {
  FsmInit();
  FsmTransition(Start);
  FsmTransition(Pause);
  EXPECT_EQ(current_state, GameState::kStatePaused);
  FsmTransition(Pause);
  EXPECT_EQ(current_state, GameState::kStateMoving);
  EXPECT_FALSE(game_state.pause);
}

TEST(FiniteStateMachineTest, TransitionMovingToGameOver) {
  ResetStateForTest();
  FsmInit();
  FsmTransition(Start);
  direction = 0;
  snake_x[0] = FieldSizes::width - 1;
  game_state.field[snake_y[0]][snake_x[0]] = 1;
  forceUpdate();
  FsmTransition(Right);
  EXPECT_EQ(current_state, GameState::kStateGameOver);
  EXPECT_TRUE(is_terminated);
}

TEST(FiniteStateMachineTest, TransitionGameOverToStart) {
  ResetStateForTest();
  FsmInit();
  FsmTransition(Start);
  direction = 0;
  snake_x[0] = FieldSizes::width - 1;
  forceUpdate();
  FsmTransition(Right);
  FsmTransition(Start);
  EXPECT_EQ(current_state, GameState::kStateStart);
  EXPECT_FALSE(is_terminated);
  EXPECT_TRUE(game_state.field != nullptr);
}

TEST(FiniteStateMachineTest, TransitionStartToGameOver) {
  FsmInit();
  EXPECT_EQ(current_state, GameState::kStateStart);
  FsmTransition(Terminate);
  EXPECT_EQ(current_state, GameState::kStateGameOver);
  EXPECT_TRUE(is_terminated);
  EXPECT_TRUE(game_state.field == nullptr);
}

TEST(FiniteStateMachineTest, TransitionMovingToGameOverViaTerminate) {
  FsmInit();
  FsmTransition(Start);
  EXPECT_EQ(current_state, GameState::kStateMoving);
  FsmTransition(Terminate);
  EXPECT_EQ(current_state, GameState::kStateGameOver);
  EXPECT_TRUE(is_terminated);
  EXPECT_TRUE(game_state.field == nullptr);
}

TEST(FiniteStateMachineTest, TransitionPausedToGameOver) {
  FsmInit();
  FsmTransition(Start);
  FsmTransition(Pause);
  EXPECT_EQ(current_state, GameState::kStatePaused);
  FsmTransition(Terminate);
  EXPECT_EQ(current_state, GameState::kStateGameOver);
  EXPECT_TRUE(is_terminated);
  EXPECT_TRUE(game_state.field == nullptr);
}

TEST(FiniteStateMachineTest, UserInputDirections) {
  ResetStateForTest();
  FsmInit();
  FsmTransition(Start);
  EXPECT_EQ(current_state, GameState::kStateMoving);
  direction = 3;
  FsmTransition(Left);
  EXPECT_EQ(direction, 2);
  direction = 1;
  FsmTransition(Right);
  EXPECT_EQ(direction, 0);
  direction = 0;
  FsmTransition(Up);
  EXPECT_EQ(direction, 3);
  direction = 2;
  FsmTransition(Down);
  EXPECT_EQ(direction, 1);
}

TEST(SnakeLogicTest, ReinitializeGameWithField) {
  ResetStateForTest();
  int** old_field = game_state.field;
  init_game();
  EXPECT_EQ(game_state.field, old_field);
  EXPECT_EQ(snake_length, 4);
  EXPECT_EQ(direction, 0);
}

TEST(SnakeLogicTest, SoftBoundaries) {
  ResetStateForTest();
  toggle_boundary_mode();
  direction = 0;
  snake_x[0] = FieldSizes::width - 1;
  snake_y[0] = 10;
  game_state.field[snake_y[0]][snake_x[0]] = 1;
  forceUpdate();
  EXPECT_EQ(snake_x[0], 0);
  EXPECT_EQ(snake_y[0], 10);
  EXPECT_FALSE(is_terminated);
  EXPECT_EQ(game_state.field[10][0], 1);
}

TEST(SnakeLogicTest, MoveInAllDirections) {
  ResetStateForTest();
  direction = 1;
  int old_y = snake_y[0];
  forceUpdate();
  EXPECT_EQ(snake_y[0], old_y + 1);
  EXPECT_EQ(game_state.field[old_y + 1][snake_x[0]], 1);
  ResetStateForTest();
  direction = 2;
  int old_x = snake_x[0];
  forceUpdate();
  EXPECT_EQ(snake_x[0], old_x);
  EXPECT_EQ(game_state.field[snake_y[0]][old_x - 1], 1);
  ResetStateForTest();
  direction = 3;
  old_y = snake_y[0];
  forceUpdate();
  EXPECT_EQ(snake_y[0], old_y - 1);
  EXPECT_EQ(game_state.field[old_y - 1][snake_x[0]], 1);
}

TEST(SnakeLogicTest, MinSpeed) {
  ResetStateForTest();
  game_state.score = 50;
  game_state.level = 9;
  game_state.speed = 500;
  forceUpdate();
  EXPECT_EQ(game_state.level, 10);
  EXPECT_EQ(game_state.speed, 50);
}

TEST(SnakeLogicTest, CleanupGameWithNext) {
  ResetStateForTest();
  game_state.next = new int*[4];
  for (int i = 0; i < 4; ++i) {
    game_state.next[i] = new int[4]();
  }
  cleanup_game();
  EXPECT_EQ(game_state.next, nullptr);
  EXPECT_EQ(game_state.field, nullptr);
  EXPECT_TRUE(is_terminated);
}

TEST(SnakeLogicTest, UpdateCurrentStateWithMovement) {
  ResetStateForTest();
  game_state.speed = 0;
  int old_x = snake_x[0];
  updateCurrentState();
  EXPECT_EQ(snake_x[0], old_x + 1);
  EXPECT_EQ(game_state.field[snake_y[0]][old_x + 1], 1);
}

TEST(SnakeLogicTest, SpeedBoostExactTime) {
  ResetStateForTest();
  userInput(Action, false);
  struct timespec current_time;
  clock_gettime(CLOCK_MONOTONIC, &current_time);
  speed_boost_end_time = current_time;
  updateCurrentState();
  EXPECT_FALSE(is_speed_boosted);
  EXPECT_EQ(game_state.speed, base_speed);
}

TEST(SnakeLogicTest, ReturnSizesAndBoundaryMode) {
  FieldSizes sizes = returnSizes();
  EXPECT_EQ(sizes.width, FieldSizes::width);
  EXPECT_EQ(sizes.height, FieldSizes::height);
  EXPECT_FALSE(get_boundary_mode());
  toggle_boundary_mode();
  EXPECT_TRUE(get_boundary_mode());
}

TEST(FiniteStateMachineTest, GameOverWithTerminate) {
  ResetStateForTest();
  FsmInit();
  FsmTransition(Start);
  FsmTransition(Terminate);
  EXPECT_EQ(current_state, GameState::kStateGameOver);
  FsmTransition(Terminate);
  EXPECT_EQ(current_state, GameState::kStateGameOver);
  EXPECT_TRUE(is_terminated);
  EXPECT_EQ(game_state.field, nullptr);
}

TEST(SnakeLogicTest, ReinitializeGameWithTerminated) {
  ResetStateForTest();
  is_terminated = 1;
  init_game();
  EXPECT_EQ(snake_length, 4);
  EXPECT_EQ(direction, 0);
}

TEST(SnakeLogicTest, GenerateFoodNoFreeCells) {
  cleanup_game();
  game_state.field = new int*[FieldSizes::height];
  for (int i = 0; i < FieldSizes::height; ++i) {
    game_state.field[i] = new int[FieldSizes::width]();
  }
  for (int y = 0; y < FieldSizes::height; ++y) {
    for (int x = 0; x < FieldSizes::width; ++x) {
      game_state.field[y][x] = 1;
    }
  }
  is_terminated = 0;
  generate_food();
  EXPECT_TRUE(is_terminated);
  for (int i = 0; i < FieldSizes::height; ++i) {
    delete[] game_state.field[i];
  }
  delete[] game_state.field;
  game_state.field = nullptr;
}

TEST(SnakeLogicTest, MemoryAllocationFailureField) {
#ifdef SIMULATE_BAD_ALLOC
  cleanup_game();
  EXPECT_THROW(init_game(), std::runtime_error);
#else
  GTEST_SKIP()
      << "Memory allocation failure test skipped (requires SIMULATE_BAD_ALLOC)";
#endif
}

}  // namespace s21

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}