#include "s21_snake_model.h"

#include <QDebug>

namespace s21 {

SnakeModel::SnakeModel(QObject* parent) : QObject(parent) {
  initializeGame();
  updatePreviousState();
}

SnakeModel::~SnakeModel() { cleanup_game(); }

void SnakeModel::initializeGame() {
  FsmInit();
  init_game();
  qDebug() << "Initialized game state:" << game_state.score << game_state.level
           << game_state.speed;
  for (int y = 0; y < FieldSizes::height; ++y) {
    for (int x = 0; x < FieldSizes::width; ++x) {
      if (game_state.field[y][x] != 0) {
        qDebug() << "Non-zero initial value at" << x << y << ":"
                 << game_state.field[y][x];
      }
    }
  }
}

GameInfo_t SnakeModel::getGameState() const { return game_state; }

void SnakeModel::processInput(UserAction_t action, bool hold) {
  FsmTransition(action);
  emit stateChanged();
  qDebug() << "Processed input:" << static_cast<int>(action) << "hold:" << hold;
}

bool SnakeModel::updateGame() {
  updateCurrentState();
  bool changed = hasStateChanged();
  if (changed) {
    updatePreviousState();
    emit stateChanged();
    qDebug() << "Updated game state - Score:" << game_state.score;
    for (int y = 0; y < FieldSizes::height; ++y) {
      QString row;
      for (int x = 0; x < FieldSizes::width; ++x) {
        row += QString::number(game_state.field[y][x]) + " ";
      }
      qDebug() << "Row" << y << ":" << row;
    }
  }
  return changed;
}

void SnakeModel::toggleBoundaries() { toggle_boundary_mode(); }

bool SnakeModel::isGameRunning() const {
  return current_state == GameState::kStateMoving && !is_terminated;
}

bool SnakeModel::isGamePaused() const {
  return current_state == GameState::kStatePaused;
}

bool SnakeModel::isGameOver() const {
  return current_state == GameState::kStateGameOver || is_terminated;
}

bool SnakeModel::hasStateChanged() const {
  const GameInfo_t& current = game_state;
  const GameInfo_t& previous = previousState_;
  return (current.score != previous.score ||
          current.high_score != previous.high_score ||
          current.level != previous.level || current.speed != previous.speed ||
          current.pause != previous.pause ||
          is_terminated != (previous.field == nullptr));
}

void SnakeModel::updatePreviousState() { previousState_ = game_state; }

}  // namespace s21