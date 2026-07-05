#include "s21_snake_view.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "ui_snake.h"

namespace s21 {

// ============================================================================
// GameFieldWidget Implementation
// ============================================================================

GameFieldWidget::GameFieldWidget(QWidget* parent)
    : QWidget(parent), model_(nullptr) {
  setMinimumSize(400, 800);  // 10x20 поле, соотношение 1:2
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  setStyleSheet("background-color: black; border: 2px solid cyan;");
}

void GameFieldWidget::setModel(SnakeModel* model) {
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

  if (!model_) {
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 16, QFont::Bold));
    painter.drawText(rect(), Qt::AlignCenter, "Press Start\nto begin!");
    return;
  }

  drawField(painter);
}

QRect GameFieldWidget::getGameArea() const {
  int availableWidth = width();
  int availableHeight = height();

  // Соотношение поля 10:20 (ширина:высота)
  double fieldRatio =
      static_cast<double>(FieldSizes::width) / FieldSizes::height;
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
  GameInfo_t state = model_->getGameState();
  if (!state.field) return;

  QRect gameArea = getGameArea();
  int cellWidth = gameArea.width() / FieldSizes::width;
  int cellHeight = gameArea.height() / FieldSizes::height;

  drawGrid(painter, cellWidth, cellHeight);

  for (int y = 0; y < FieldSizes::height; ++y) {
    for (int x = 0; x < FieldSizes::width; ++x) {
      int xPos = gameArea.x() + x * cellWidth;
      int yPos = gameArea.y() + y * cellHeight;
      QRect cellRect(xPos, yPos, cellWidth, cellHeight);

      switch (state.field[y][x]) {
        case 1:  // Snake
          painter.fillRect(cellRect, QColor(0, 255, 0));
          painter.setPen(QPen(QColor(0, 200, 0), 1));
          painter.drawRect(cellRect);
          break;

        case 2:  // Food
          painter.setBrush(QColor(255, 0, 0));
          painter.setPen(QPen(QColor(200, 0, 0), 1));
          painter.drawEllipse(cellRect.adjusted(2, 2, -2, -2));
          painter.setBrush(Qt::NoBrush);
          break;

        default:  // Empty
          break;
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

  for (int x = 1; x < FieldSizes::width; ++x) {
    int xPos = gameArea.x() + x * cellWidth;
    painter.drawLine(xPos, gameArea.y(), xPos,
                     gameArea.y() + gameArea.height());
  }

  for (int y = 1; y < FieldSizes::height; ++y) {
    int yPos = gameArea.y() + y * cellHeight;
    painter.drawLine(gameArea.x(), yPos, gameArea.x() + gameArea.width(), yPos);
  }
}

// ============================================================================
// SnakeView Implementation
// ============================================================================

SnakeView::SnakeView(QWidget* parent)
    : QMainWindow(parent),
      ui(new Ui::SnakeView),
      model_(new SnakeModel(this)),
      controller_(new SnakeController(model_, this)),
      gameField_(new GameFieldWidget(this)) {
  ui->setupUi(this);

  ui->gameLayout->removeWidget(ui->fieldLabel);
  delete ui->fieldLabel;
  ui->gameLayout->addWidget(gameField_);

  gameField_->setModel(model_);

  connect(controller_, &SnakeController::gameStateChanged, this,
          &SnakeView::updateDisplay);

  setFocusPolicy(Qt::StrongFocus);

  updateDisplay();
}

SnakeView::~SnakeView() { delete ui; }

void SnakeView::keyPressEvent(QKeyEvent* event) {
  if (!event->isAutoRepeat()) {
    switch (event->key()) {
      case Qt::Key_Left:
        controller_->handleLeft();
        break;
      case Qt::Key_Right:
        controller_->handleRight();
        break;
      case Qt::Key_Up:
        controller_->handleUp();
        break;
      case Qt::Key_Down:
        controller_->handleDown();
        break;
      case Qt::Key_Space:
        controller_->handleAction(false);
        break;
      case Qt::Key_S:
        controller_->handleStart();
        break;
      case Qt::Key_P:
        controller_->handlePause();
        break;
      case Qt::Key_Q:
        controller_->handleTerminate();
        close();
        break;
      case Qt::Key_B:
        controller_->handleToggleBoundaries();
        break;
    }
  }
  setFocus();
  QMainWindow::keyPressEvent(event);
}

void SnakeView::keyReleaseEvent(QKeyEvent* event) {
  if (event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
  }
  QMainWindow::keyReleaseEvent(event);
}

void SnakeView::updateDisplay() {
  drawSidebar();
  gameField_->update();
}

void SnakeView::on_startButton_clicked() {
  controller_->handleStart();
  setFocus();
}

void SnakeView::on_pauseButton_clicked() {
  controller_->handlePause();
  setFocus();
}

void SnakeView::on_boundaryButton_clicked() {
  controller_->handleToggleBoundaries();
  setFocus();
}

void SnakeView::on_quitButton_clicked() {
  controller_->handleTerminate();
  close();
}

void SnakeView::drawSidebar() {
  GameInfo_t state = model_->getGameState();

  ui->scoreLabel->setText(QString("Score: %1").arg(state.score));
  ui->highScoreLabel->setText(QString("High Score: %1").arg(state.high_score));
  ui->levelLabel->setText(QString("Level: %1").arg(state.level));
  ui->speedLabel->setText(QString("Speed: %1ms").arg(state.speed));
  ui->boundariesLabel->setText(
      QString("Boundaries: %1").arg(get_boundary_mode() ? "Hard" : "Soft"));

  ui->statusLabel->setText(getStatusText());
  updateStatusStyle();
}

void SnakeView::updateStatusStyle() {
  QString color = getStatusColor();
  ui->statusLabel->setStyleSheet(
      QString("color: %1; font-weight: bold;").arg(color));
}

QString SnakeView::getStatusText() const {
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

QString SnakeView::getStatusColor() const {
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

void SnakeView::resizeEvent(QResizeEvent* event) {
  updateDisplay();
  QMainWindow::resizeEvent(event);
}

}  // namespace s21