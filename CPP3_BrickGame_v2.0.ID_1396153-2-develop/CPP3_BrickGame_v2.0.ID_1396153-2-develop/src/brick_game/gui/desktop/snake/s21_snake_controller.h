#ifndef S21_SNAKE_CONTROLLER_H_
#define S21_SNAKE_CONTROLLER_H_

#include <QObject>
#include <QTimer>

#include "s21_snake_model.h"

namespace s21 {

class SnakeController : public QObject {
  Q_OBJECT

 public:
  explicit SnakeController(SnakeModel* model, QObject* parent = nullptr);
  ~SnakeController();
  void handleStart();
  void handlePause();
  void handleTerminate();
  void handleLeft();
  void handleRight();
  void handleUp();
  void handleDown();
  void handleAction(bool hold = false);
  void handleToggleBoundaries();
  void startGameLoop();
  void stopGameLoop();

 signals:
  void gameStateChanged();
  void restoreSpeed();

 private slots:
  void updateGame();
  void restoreOriginalSpeed();

 private:
  SnakeModel* model_;
  QTimer* gameTimer_;
  SnakeController(const SnakeController&) = delete;
  SnakeController& operator=(const SnakeController&) = delete;
};

}  // namespace s21

#endif  // S21_SNAKE_CONTROLLER_H_