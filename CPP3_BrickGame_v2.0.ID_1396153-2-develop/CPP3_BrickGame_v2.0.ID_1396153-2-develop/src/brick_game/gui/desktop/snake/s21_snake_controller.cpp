#include "s21_snake_controller.h"

#include "./../../../snake/s21_snake_logic.h"

namespace s21 {

SnakeController::SnakeController(SnakeModel* model, QObject* parent)
    : QObject(parent), model_(model), gameTimer_(new QTimer(this)) {
  connect(gameTimer_, &QTimer::timeout, this, &SnakeController::updateGame);

  connect(model_, &SnakeModel::stateChanged, this,
          &SnakeController::gameStateChanged);

  connect(this, &SnakeController::restoreSpeed, this,
          &SnakeController::restoreOriginalSpeed);

  startGameLoop();
}

SnakeController::~SnakeController() { stopGameLoop(); }

void SnakeController::handleStart() {
  model_->processInput(UserAction_t::Start);
  emit gameStateChanged();
}

void SnakeController::handlePause() {
  model_->processInput(UserAction_t::Pause);
  emit gameStateChanged();
}

void SnakeController::handleTerminate() {
  model_->processInput(UserAction_t::Terminate);
  emit gameStateChanged();
}

void SnakeController::handleLeft() { model_->processInput(UserAction_t::Left); }

void SnakeController::handleRight() {
  model_->processInput(UserAction_t::Right);
}

void SnakeController::handleUp() { model_->processInput(UserAction_t::Up); }

void SnakeController::handleDown() { model_->processInput(UserAction_t::Down); }

void SnakeController::handleAction(bool hold) {
  model_->processInput(UserAction_t::Action, hold);
  if (!hold && !is_speed_boosted) {
    QTimer::singleShot(1000, this, &SnakeController::restoreSpeed);
  }
}

void SnakeController::handleToggleBoundaries() {
  model_->toggleBoundaries();
  emit gameStateChanged();
}

void SnakeController::startGameLoop() {
  // таймер с интервалом ~60 FPS (16.67ms)
  gameTimer_->start(17);
}

void SnakeController::stopGameLoop() { gameTimer_->stop(); }

void SnakeController::updateGame() {
  bool stateChanged = model_->updateGame();
  if (stateChanged) {
    emit gameStateChanged();
  }
}

void SnakeController::restoreOriginalSpeed() {
  game_state.speed = base_speed;
  is_speed_boosted = false;
  emit gameStateChanged();
}

}  // namespace s21