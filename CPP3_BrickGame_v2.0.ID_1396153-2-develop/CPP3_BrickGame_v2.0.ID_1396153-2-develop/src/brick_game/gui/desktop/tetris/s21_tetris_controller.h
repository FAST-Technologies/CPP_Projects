#ifndef S21_TETRIS_CONTROLLER_H_
#define S21_TETRIS_CONTROLLER_H_

#include <QDebug>
#include <QObject>
#include <QTimer>

#include "s21_tetris_model.h"

namespace s21 {

class TetrisController : public QObject {
  Q_OBJECT

 public:
  explicit TetrisController(TetrisModel* model, QObject* parent = nullptr);
  ~TetrisController();
  void handleStart();
  void handlePause();
  void handleTerminate();
  void handleLeft();
  void handleRight();
  void handleUp();
  void handleDown();
  void handleAction(bool hold = false);

 signals:
  void gameStateChanged();

 private slots:
  void updateGame();

 private:
  TetrisModel* model_;
  QTimer* gameTimer_;
  TetrisController(const TetrisController&) = delete;
  TetrisController& operator=(const TetrisController&) = delete;
};

}  // namespace s21

#endif  // S21_TETRIS_CONTROLLER_H_