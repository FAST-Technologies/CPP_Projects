#ifndef S21_TETRIS_VIEW_H_
#define S21_TETRIS_VIEW_H_

#include <QFrame>
#include <QKeyEvent>
#include <QMainWindow>
#include <QPainter>
#include <QPushButton>
#include <QResizeEvent>
#include <QString>
#include <QTimer>
#include <QWidget>

#include "s21_tetris_controller.h"
#include "s21_tetris_model.h"

namespace Ui {
class TetrisView;
}

namespace s21 {

class GameFieldWidget : public QWidget {
  Q_OBJECT

 public:
  explicit GameFieldWidget(QWidget* parent = nullptr);
  void setModel(TetrisModel* model);

 protected:
  void paintEvent(QPaintEvent* event) override;
  QSize sizeHint() const override;
  QSize minimumSizeHint() const override;

 private:
  TetrisModel* model_;
  void drawField(QPainter& painter);
  void drawGrid(QPainter& painter, int cellWidth, int cellHeight);
  QRect getGameArea() const;
};

class TetrisView : public QMainWindow {
  Q_OBJECT

 public:
  explicit TetrisView(QWidget* parent = nullptr);
  ~TetrisView();

 protected:
  void keyPressEvent(QKeyEvent* event) override;
  void keyReleaseEvent(QKeyEvent* event) override;
  void resizeEvent(QResizeEvent* event) override;

 private slots:
  void updateDisplay();
  void on_startButton_clicked();
  void on_pauseButton_clicked();
  void on_quitButton_clicked();

 private:
  Ui::TetrisView* ui;
  TetrisModel* model_;
  TetrisController* controller_;
  GameFieldWidget* gameField_;
  QTimer* gameTimer_;

  void drawField();
  void drawSidebar();
  void updateStatusStyle();
  void drawNextPiece();

  QString getStatusText() const;
  QString getStatusColor() const;

  TetrisView(const TetrisView&) = delete;
  TetrisView& operator=(const TetrisView&) = delete;
};

}  // namespace s21

#endif  // S21_TETRIS_VIEW_H_