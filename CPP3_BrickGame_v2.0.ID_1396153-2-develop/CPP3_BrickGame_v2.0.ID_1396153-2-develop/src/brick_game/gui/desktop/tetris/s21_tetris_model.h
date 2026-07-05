#ifndef S21_TETRIS_MODEL_H_
#define S21_TETRIS_MODEL_H_

#include <QObject>

#include "./../../../tetris/s21_tetris_finite_state_machine.h"
#include "./../../../tetris/s21_tetris_logic.h"

namespace s21 {

class TetrisModel : public QObject {
  Q_OBJECT

 public:
  explicit TetrisModel(QObject* parent = nullptr);
  ~TetrisModel();
  GameInfo_t get_game_state() const;
  void processInput(UserAction_t action, bool hold = false);
  bool updateGame();
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

  TetrisModel(const TetrisModel&) = delete;
  TetrisModel& operator=(const TetrisModel&) = delete;
};

}  // namespace s21

#endif  // S21_TETRIS_MODEL_H_