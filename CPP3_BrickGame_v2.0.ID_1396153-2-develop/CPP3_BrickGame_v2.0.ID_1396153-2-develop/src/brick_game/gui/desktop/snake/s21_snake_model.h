#ifndef S21_SNAKE_MODEL_H_
#define S21_SNAKE_MODEL_H_

#include <QObject>

#include "./../../../snake/s21_snake_finite_state_machine.h"
#include "./../../../snake/s21_snake_logic.h"

namespace s21 {

class SnakeModel : public QObject {
  Q_OBJECT

 public:
  explicit SnakeModel(QObject* parent = nullptr);
  ~SnakeModel();
  GameInfo_t getGameState() const;
  void processInput(UserAction_t action, bool hold = false);
  bool updateGame();
  void toggleBoundaries();
  bool isGameRunning() const;
  bool isGamePaused() const;
  bool isGameOver() const;

 signals:
  void stateChanged();

 private:
  GameInfo_t previousState_;
  void initializeGame();
  bool hasStateChanged() const;
  void updatePreviousState();

  SnakeModel(const SnakeModel&) = delete;
  SnakeModel& operator=(const SnakeModel&) = delete;
};

}  // namespace s21

#endif  // S21_SNAKE_MODEL_H_