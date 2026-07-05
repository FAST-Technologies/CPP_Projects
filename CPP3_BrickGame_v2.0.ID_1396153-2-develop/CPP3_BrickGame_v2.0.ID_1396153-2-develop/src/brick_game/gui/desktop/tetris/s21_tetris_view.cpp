// s21_tetris_view.cpp
#include "s21_tetris_view.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QPainter>
#include <QVBoxLayout>

#include "./../../../tetris/s21_tetris_logic.h"
#include "ui_tetris.h"

namespace s21 {

// ============================================================================
// GameFieldWidget Implementation
// ============================================================================

GameFieldWidget::GameFieldWidget(QWidget* parent)
    : QWidget(parent), model_(nullptr) {
  setMinimumSize(400, 800);  // 10x20 поле
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  setStyleSheet("background-color: black; border: 2px solid cyan;");
}

void GameFieldWidget::setModel(TetrisModel* model) {
  model_ = model;
  update();
}

QSize GameFieldWidget::sizeHint() const { return QSize(500, 1000); }

QSize GameFieldWidget::minimumSizeHint() const { return QSize(300, 600); }

void GameFieldWidget::paintEvent(QPaintEvent* event) {
  Q_UNUSED(event);

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  painter.fillRect(rect(), Qt::black);

  if (!model_ || !model_->get_game_state().field) {
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 16, QFont::Bold));
    painter.drawText(
        rect(), Qt::AlignCenter,
        model_ ? "Game not initialized" : "Press Start\nto begin!");
    return;
  }

  drawField(painter);
}

QRect GameFieldWidget::getGameArea() const {
  int availableWidth = width();
  int availableHeight = height();
  double fieldRatio = 10.0 / 20.0;
  double widgetRatio = static_cast<double>(availableWidth) / availableHeight;

  int gameWidth, gameHeight;
  int offsetX = 0, offsetY = 0;

  if (widgetRatio > fieldRatio) {
    gameHeight = availableHeight;
    gameWidth = static_cast<int>(gameHeight * fieldRatio);
    offsetX = (availableWidth - gameWidth) / 2;
  } else {
    gameWidth = availableWidth;
    gameHeight = static_cast<int>(gameWidth / fieldRatio);
    offsetY = (availableHeight - gameHeight) / 2;
  }

  return QRect(offsetX, offsetY, gameWidth, gameHeight);
}

void GameFieldWidget::drawField(QPainter& painter) {
  GameInfo_t state = model_->get_game_state();
  if (!state.field) return;

  QRect gameArea = getGameArea();
  int cellWidth = gameArea.width() / 10;
  int cellHeight = gameArea.height() / 20;

  drawGrid(painter, cellWidth, cellHeight);

  // Отрисовка заполненных клеток поля
  for (int y = 0; y < 20; ++y) {
    for (int x = 0; x < 10; ++x) {
      int xPos = gameArea.x() + x * cellWidth;
      int yPos = gameArea.y() + y * cellHeight;
      QRect cellRect(xPos, yPos, cellWidth, cellHeight);

      if (state.field[y][x]) {
        painter.fillRect(cellRect, QColor(150, 150, 150));
        painter.setPen(QPen(Qt::gray, 1));
        painter.drawRect(cellRect);
      }
    }
  }

  // Отрисовка текущего тетромино
  Tetromino current = get_current_tetromino();
  for (int y = 0; y < 4; ++y) {
    for (int x = 0; x < 4; ++x) {
      if (current.shape[y][x]) {
        int field_x = current.x + x;
        int field_y = current.y + y;
        if (field_x >= 0 && field_x < 10 && field_y >= 0 && field_y < 20) {
          int xPos = gameArea.x() + field_x * cellWidth;
          int yPos = gameArea.y() + field_y * cellHeight;
          QRect cellRect(xPos, yPos, cellWidth, cellHeight);
          painter.fillRect(cellRect, QColor(0, 0, 255));
          painter.setPen(QPen(Qt::blue, 1));
          painter.drawRect(cellRect);
        }
      }
    }
  }

  painter.setPen(QPen(Qt::cyan, 2));
  painter.drawRect(gameArea);
}

void GameFieldWidget::drawGrid(QPainter& painter, int cellWidth,
                               int cellHeight) {
  QRect gameArea = getGameArea();
  painter.setPen(QPen(QColor(40, 40, 40), 1));

  for (int x = 1; x < 10; ++x) {
    int xPos = gameArea.x() + x * cellWidth;
    painter.drawLine(xPos, gameArea.y(), xPos,
                     gameArea.y() + gameArea.height());
  }

  for (int y = 1; y < 20; ++y) {
    int yPos = gameArea.y() + y * cellHeight;
    painter.drawLine(gameArea.x(), yPos, gameArea.x() + gameArea.width(), yPos);
  }
}

// ============================================================================
// TetrisView Implementation
// ============================================================================

TetrisView::TetrisView(QWidget* parent)
    : QMainWindow(parent),
      ui(new Ui::TetrisView),
      model_(new TetrisModel(this)),
      controller_(new TetrisController(model_, this)),
      gameField_(new GameFieldWidget(this)) {
  ui->setupUi(this);

  ui->gameLayout->removeWidget(ui->fieldLabel);
  delete ui->fieldLabel;
  ui->gameLayout->addWidget(gameField_);

  gameField_->setModel(model_);

  connect(controller_, &TetrisController::gameStateChanged, this,
          &TetrisView::updateDisplay);

  setFocusPolicy(Qt::StrongFocus);
  setFocus();

  updateDisplay();
}

TetrisView::~TetrisView() {
  delete ui;
  delete controller_;
  delete gameField_;
}

void TetrisView::keyPressEvent(QKeyEvent* event) {
  qDebug() << "Key pressed:" << event->key() << "Text:" << event->text();
  if (!event->isAutoRepeat()) {
    switch (event->key()) {
      case Qt::Key_Left:
        controller_->handleLeft();
        qDebug() << "Left key handled";
        break;
      case Qt::Key_Right:
        controller_->handleRight();
        qDebug() << "Right key handled";
        break;
      case Qt::Key_Up:
        controller_->handleAction(false);
        qDebug() << "Up key handled";
        break;
      case Qt::Key_Down:
        controller_->handleDown();
        qDebug() << "Down key handled";
        break;
      case Qt::Key_Space:
        controller_->handleAction(false);
        qDebug() << "Space key handled";
        break;
      case Qt::Key_S:
        controller_->handleStart();
        qDebug() << "S key handled";
        break;
      case Qt::Key_P:
        controller_->handlePause();
        qDebug() << "P key handled";
        break;
      case Qt::Key_Q:
        controller_->handleTerminate();
        qDebug() << "Q key handled";
        break;
    }
  }
  setFocus();
  QMainWindow::keyPressEvent(event);
}

void TetrisView::keyReleaseEvent(QKeyEvent* event) {
  QMainWindow::keyReleaseEvent(event);
}

void TetrisView::updateDisplay() {
  if (model_) {
    if (model_->isGameOver()) {
      ui->statusLabel->setText("GAME OVER");
      ui->statusLabel->setStyleSheet(
          "color: red; font-weight: bold; background-color: rgba(255, 0, 0, "
          "30); "
          "border: 1px solid red; border-radius: 5px; padding: 8px;");
      ui->startButton->setEnabled(true);
      ui->pauseButton->setEnabled(false);
      ui->quitButton->setEnabled(true);
    } else {
      drawSidebar();
      gameField_->update();
      drawNextPiece();
      ui->pauseButton->setEnabled(true);
    }
  }
}

void TetrisView::on_startButton_clicked() {
  controller_->handleStart();
  ui->pauseButton->setEnabled(true);
  setFocus();
}

void TetrisView::on_pauseButton_clicked() {
  controller_->handlePause();
  setFocus();
}

void TetrisView::on_quitButton_clicked() {
  controller_->handleTerminate();
  close();
}

void TetrisView::drawSidebar() {
  GameInfo_t state = model_->get_game_state();

  ui->scoreLabel->setText(QString("Score: %1").arg(state.score));
  ui->highScoreLabel->setText(QString("High Score: %1").arg(state.high_score));
  ui->levelLabel->setText(QString("Level: %1").arg(state.level));
  ui->speedLabel->setText(QString("Speed: %1ms").arg(state.speed));

  ui->statusLabel->setText(getStatusText());
  updateStatusStyle();
}

void TetrisView::drawNextPiece() {
  GameInfo_t state = model_->get_game_state();
  QPixmap pixmap(100, 100);
  pixmap.fill(Qt::black);

  QPainter painter(&pixmap);
  painter.setRenderHint(QPainter::Antialiasing);

  int cellWidth = pixmap.width() / 4;
  int cellHeight = pixmap.height() / 4;

  for (int x = 1; x < 4; ++x) {
    int xPos = x * cellWidth;
    painter.drawLine(xPos, 0, xPos, pixmap.height());
  }
  for (int y = 1; y < 4; ++y) {
    int yPos = y * cellHeight;
    painter.drawLine(0, yPos, pixmap.width(), yPos);
  }

  for (int y = 0; y < 4; ++y) {
    for (int x = 0; x < 4; ++x) {
      if (state.next[y][x]) {
        int xPos = x * cellWidth;
        int yPos = y * cellHeight;
        QRect cellRect(xPos, yPos, cellWidth, cellHeight);
        painter.fillRect(cellRect, QColor(0, 0, 255));
        painter.setPen(QPen(Qt::blue, 1));
        painter.drawRect(cellRect);
      }
    }
  }

  ui->nextLabel->setPixmap(
      pixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void TetrisView::updateStatusStyle() {
  QString color = getStatusColor();
  ui->statusLabel->setStyleSheet(
      QString("color: %1; font-weight: bold;").arg(color));
}

QString TetrisView::getStatusText() const {
  if (model_->isGamePaused()) {
    return "PAUSED";
  } else if (model_->isGameOver()) {
    return "GAME OVER";
  } else if (model_->isGameRunning()) {
    return "PLAYING";
  } else {
    return "READY";
  }
}

QString TetrisView::getStatusColor() const {
  if (model_->isGamePaused()) {
    return "orange";
  } else if (model_->isGameOver()) {
    return "red";
  } else if (model_->isGameRunning()) {
    return "green";
  } else {
    return "blue";
  }
}

void TetrisView::resizeEvent(QResizeEvent* event) {
  updateDisplay();
  QMainWindow::resizeEvent(event);
}

}  // namespace s21