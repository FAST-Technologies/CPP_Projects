#ifndef S21_SNAKE_VIEW_H_
#define S21_SNAKE_VIEW_H_

#include <QFrame>
#include <QKeyEvent>
#include <QMainWindow>
#include <QPainter>
#include <QPushButton>
#include <QResizeEvent>
#include <QString>
#include <QWidget>

#include "s21_snake_controller.h"
#include "s21_snake_model.h"

namespace Ui {
class SnakeView;
}

namespace s21 {

class GameFieldWidget : public QWidget {
  Q_OBJECT

 public:
  explicit GameFieldWidget(QWidget* parent = nullptr);
  void setModel(SnakeModel* model);

 protected:
  void paintEvent(QPaintEvent* event) override;
  QSize sizeHint() const override;
  QSize minimumSizeHint() const override;

 private:
  SnakeModel* model_;
  void drawField(QPainter& painter);
  void drawGrid(QPainter& painter, int cellWidth, int cellHeight);
  QRect getGameArea() const;
};

class SnakeView : public QMainWindow {
  Q_OBJECT

 public:
  explicit SnakeView(QWidget* parent = nullptr);
  ~SnakeView();

 protected:
  void keyPressEvent(QKeyEvent* event) override;
  void keyReleaseEvent(QKeyEvent* event) override;
  void resizeEvent(QResizeEvent* event) override;

 private slots:
  void updateDisplay();
  void on_startButton_clicked();
  void on_pauseButton_clicked();
  void on_boundaryButton_clicked();
  void on_quitButton_clicked();

 private:
  Ui::SnakeView* ui;
  SnakeModel* model_;
  SnakeController* controller_;
  GameFieldWidget* gameField_;

  void drawField();
  void drawSidebar();
  void updateStatusStyle();

  QString getStatusText() const;
  QString getStatusColor() const;

  SnakeView(const SnakeView&) = delete;
  SnakeView& operator=(const SnakeView&) = delete;
};

}  // namespace s21

#endif  // S21_SNAKE_VIEW_H_