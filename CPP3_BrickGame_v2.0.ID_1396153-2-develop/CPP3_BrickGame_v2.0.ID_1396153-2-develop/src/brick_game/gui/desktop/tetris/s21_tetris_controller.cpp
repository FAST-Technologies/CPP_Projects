#include "s21_tetris_controller.h"

#include <QApplication>

#include "./../../../tetris/s21_tetris_logic.h"

namespace s21 {

TetrisController::TetrisController(TetrisModel* model, QObject* parent)
    : QObject(parent), model_(model), gameTimer_(new QTimer(this)) {
  connect(gameTimer_, &QTimer::timeout, this, &TetrisController::updateGame);
  connect(model_, &TetrisModel::stateChanged, this,
          &TetrisController::gameStateChanged);
  gameTimer_->start(17);  // ~60 FPS
}

TetrisController::~TetrisController() {
  if (gameTimer_) {
    gameTimer_->stop();
    delete gameTimer_;
    gameTimer_ = nullptr;
  }
}

void TetrisController::handleStart() {
  qDebug() << "Attempting to start game...";
  model_->processInput(UserAction_t::Start);
  if (gameTimer_) gameTimer_->start(17);
  emit gameStateChanged();
  qDebug() << "Game started - State:" << model_->get_game_state().score;
}

void TetrisController::handlePause() {
  model_->processInput(UserAction_t::Pause);
  emit gameStateChanged();
}

void TetrisController::handleTerminate() {
  if (gameTimer_) gameTimer_->stop();
  model_->processInput(UserAction_t::Terminate);
  emit gameStateChanged();
  if (QObject::parent()) {
    qDebug() << "Requesting application close...";
    qApp->quit();
  }
}

void TetrisController::handleLeft() {
  model_->processInput(UserAction_t::Left);
}

void TetrisController::handleRight() {
  model_->processInput(UserAction_t::Right);
}

void TetrisController::handleUp() { model_->processInput(UserAction_t::Up); }

void TetrisController::handleDown() {
  model_->processInput(UserAction_t::Down);
}

void TetrisController::handleAction(bool hold) {
  model_->processInput(UserAction_t::Action, hold);
}

void TetrisController::updateGame() {
  if (!model_->isGameOver()) {
    bool stateChanged = model_->updateGame();
    if (stateChanged) {
      emit gameStateChanged();
    }
  }
}

}  // namespace s21