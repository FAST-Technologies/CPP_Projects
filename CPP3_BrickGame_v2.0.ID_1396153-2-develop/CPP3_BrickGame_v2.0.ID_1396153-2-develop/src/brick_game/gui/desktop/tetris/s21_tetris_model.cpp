// s21_tetris_model.cpp
#include "s21_tetris_model.h"

#include <QDebug>

namespace s21 {

TetrisModel::TetrisModel(QObject* parent) : QObject(parent) {
  initializeGame();
  updatePreviousState();
}

TetrisModel::~TetrisModel() { cleanup_game(); }

void TetrisModel::initializeGame() {
  FsmInit();
  init_game();
  qDebug() << "Initialized game state:" << game_state.score << game_state.level
           << game_state.speed;
}

GameInfo_t TetrisModel::get_game_state() const {
  GameInfo_t state = game_state;
  if (isGameOver()) {
    state.field = nullptr;
    state.next = nullptr;
  }
  return state;
}

void TetrisModel::processInput(UserAction_t action, bool hold) {
  if (action == UserAction_t::Start || !isGameOver()) {
    FsmTransition(action);
    emit stateChanged();
    qDebug() << "Processed input:" << static_cast<int>(action)
             << "hold:" << hold;
  }
}

bool TetrisModel::updateGame() {
  if (!isGameOver()) {
    updateCurrentState();
    bool changed = hasStateChanged();
    if (changed) {
      updatePreviousState();
      emit stateChanged();
      qDebug() << "Updated game state - Score:" << game_state.score;
    }
    return changed;
  }
  if (isGameOver() && !hasStateChanged()) {
    updatePreviousState();
    emit stateChanged();
    qDebug() << "Game Over detected - Score:" << game_state.score;
  }
  return false;
}

bool TetrisModel::isGameRunning() const {
  return current_state == GameState::kStateMoving && !is_terminated;
}

bool TetrisModel::isGamePaused() const {
  return current_state == GameState::kStatePaused;
}

bool TetrisModel::isGameOver() const {
  return current_state == GameState::kStateGameOver || is_terminated;
}

bool TetrisModel::hasStateChanged() const {
  const GameInfo_t& current2 = game_state;
  const GameInfo_t& previous = previousState_;
  return (current2.score != previous.score ||
          current2.high_score != previous.high_score ||
          current2.level != previous.level ||
          current2.speed != previous.speed ||
          current2.pause != previous.pause ||
          is_terminated != (previous.field == nullptr));
}

void TetrisModel::updatePreviousState() { previousState_ = get_game_state(); }

}  // namespace s21