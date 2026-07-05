/// \file
/// \brief Реализация главного окна и панели управления.

#include "view.h"

#include <QCloseEvent>
#include <QComboBox>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QScrollArea>
#include <QShortcut>
#include <QSlider>
#include <QSplitter>
#include <QTabWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include "../glwidget/glwidget.h"
#include "../settings_store/settings_store.h"

using namespace s21;

View::View(QWidget* parent) : QMainWindow(parent) { setupUI(); }

View::~View() = default;

void View::setupUI() {
  setWindowTitle("3D Viewer");
  setMinimumSize(1200, 1000);

  QWidget* centralWidget = new QWidget(this);
  setCentralWidget(centralWidget);

  QSplitter* mainSplitter = new QSplitter(Qt::Vertical, centralWidget);
  QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
  mainLayout->addWidget(mainSplitter);

  glWidget = new GLWidget(this);
  glWidget->setMinimumSize(800, 350);
  mainSplitter->addWidget(glWidget);

  QWidget* controlWidget = new QWidget();
  controlWidget->setMaximumHeight(480);
  controlWidget->setMinimumHeight(400);
  mainSplitter->addWidget(controlWidget);

  QVBoxLayout* controlMainLayout = new QVBoxLayout(controlWidget);
  controlMainLayout->setSpacing(5);
  controlMainLayout->setContentsMargins(5, 5, 5, 5);

  // === Верхняя панель управления ===
  QGroupBox* fileGroup = new QGroupBox("Управление", controlWidget);
  fileGroup->setMaximumHeight(100);
  QGridLayout* fileLayout = new QGridLayout(fileGroup);
  fileLayout->setSpacing(5);

  openButton = new QPushButton("Открыть OBJ", fileGroup);
  resetButton = new QPushButton("Сброс камеры", fileGroup);
  wireframeButton = new QPushButton("Каркасная модель", fileGroup);
  colorButton = new QPushButton("Цвет фона", fileGroup);
  edgeColorButton = new QPushButton("Цвет рёбер", fileGroup);
  vertexColorButton = new QPushButton("Цвет вершин", fileGroup);
  resetAllButton = new QPushButton("Сброс настроек", fileGroup);
  axesButton = new QPushButton("Оси: ВЫКЛ", fileGroup);

  fileLayout->addWidget(openButton, 0, 0);
  fileLayout->addWidget(resetButton, 0, 1);
  fileLayout->addWidget(wireframeButton, 0, 2);
  fileLayout->addWidget(axesButton, 0, 3);
  fileLayout->addWidget(colorButton, 1, 0);
  fileLayout->addWidget(edgeColorButton, 1, 1);
  fileLayout->addWidget(vertexColorButton, 1, 2);
  fileLayout->addWidget(resetAllButton, 1, 3);

  connect(openButton, &QPushButton::clicked, this,
          [this]() { emit openFileRequested(); });
  connect(resetButton, &QPushButton::clicked, this,
          [this]() { emit resetCameraRequested(); });
  connect(wireframeButton, &QPushButton::clicked, this,
          [this]() { emit toggleWireframe(); });
  connect(colorButton, &QPushButton::clicked, this,
          [this]() { emit backgroundColorChanged(); });
  connect(edgeColorButton, &QPushButton::clicked, this,
          [this]() { emit edgeColorChanged(); });
  connect(vertexColorButton, &QPushButton::clicked, this,
          [this]() { emit vertexColorChanged(); });
  connect(resetAllButton, &QPushButton::clicked, this,
          [this]() { resetAllSettings(); });
  connect(axesButton, &QPushButton::clicked, this, [this]() {
    glWidget->toggleAxes();
    setAxesButtonText(glWidget->isAxesVisible());
  });

  controlMainLayout->addWidget(fileGroup);

  QTabWidget* tabWidget = new QTabWidget(controlWidget);
  tabWidget->setMaximumHeight(450);

  // === Вкладка "Настройки" ===
  QWidget* settingsTab = new QWidget();
  QScrollArea* settingsScrollArea = new QScrollArea();
  settingsScrollArea->setWidget(settingsTab);
  settingsScrollArea->setWidgetResizable(true);
  settingsScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  settingsScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

  QGridLayout* settingsLayout = new QGridLayout(settingsTab);
  settingsLayout->setSpacing(3);

  QLabel* speedTitle = new QLabel("Скорость вращения:", settingsTab);
  rotationSpeedSlider = new QSlider(Qt::Horizontal, settingsTab);
  rotationSpeedSlider->setRange(0, 50);  // хранение: float, UI: *10
  rotationSpeedSlider->setValue(10);
  rotationSpeedSlider->setMinimumWidth(120);
  speedLabel = new QLabel("Скорость: 1.0", settingsTab);
  speedLabel->setMinimumWidth(100);

  settingsLayout->addWidget(speedTitle, 0, 0);
  settingsLayout->addWidget(rotationSpeedSlider, 0, 1);
  settingsLayout->addWidget(speedLabel, 0, 2);

  QLabel* edgeThicknessTitle = new QLabel("Толщина рёбер:", settingsTab);
  edgeThicknessSlider = new QSlider(Qt::Horizontal, settingsTab);
  edgeThicknessSlider->setRange(1, 50);  // хранение: float, UI: *10
  edgeThicknessSlider->setValue(10);
  edgeThicknessSlider->setMinimumWidth(120);
  edgeThicknessLabel = new QLabel("Толщина: 1.0", settingsTab);
  edgeThicknessLabel->setMinimumWidth(100);

  settingsLayout->addWidget(edgeThicknessTitle, 1, 0);
  settingsLayout->addWidget(edgeThicknessSlider, 1, 1);
  settingsLayout->addWidget(edgeThicknessLabel, 1, 2);

  QLabel* vertexSizeTitle = new QLabel("Размер вершин:", settingsTab);
  vertexSizeSlider = new QSlider(Qt::Horizontal, settingsTab);
  vertexSizeSlider->setRange(2, 40);  // хранение: float, UI: *2
  vertexSizeSlider->setValue(10);
  vertexSizeSlider->setMinimumWidth(120);
  vertexSizeLabel = new QLabel("Размер: 5.0", settingsTab);
  vertexSizeLabel->setMinimumWidth(100);

  settingsLayout->addWidget(vertexSizeTitle, 2, 0);
  settingsLayout->addWidget(vertexSizeSlider, 2, 1);
  settingsLayout->addWidget(vertexSizeLabel, 2, 2);

  lineTypeLabel = new QLabel("Тип линий:", settingsTab);
  lineTypeCombo = new QComboBox(settingsTab);
  lineTypeCombo->addItem("Сплошные (скрытые пунктир)",
                         static_cast<int>(LineType::SOLID));
  lineTypeCombo->addItem("Пунктирные", static_cast<int>(LineType::DASHED));
  lineTypeCombo->addItem("Штрихпунктирные",
                         static_cast<int>(LineType::DOTDASH));
  lineTypeCombo->setCurrentIndex(0);
  lineTypeCombo->setMaximumWidth(250);
  lineTypeCombo->setMinimumWidth(200);

  settingsLayout->addWidget(lineTypeLabel, 3, 0);
  settingsLayout->addWidget(lineTypeCombo, 3, 1);

  projectionLabel = new QLabel("Тип проекции:", settingsTab);
  projectionCombo = new QComboBox(settingsTab);
  projectionCombo->addItem("Центральная (перспективная)",
                           static_cast<int>(ProjectionType::PERSPECTIVE));
  projectionCombo->addItem("Параллельная (ортогональная)",
                           static_cast<int>(ProjectionType::ORTHOGONAL));
  projectionCombo->setCurrentIndex(0);
  projectionCombo->setMaximumWidth(250);

  settingsLayout->addWidget(projectionLabel, 4, 0);
  settingsLayout->addWidget(projectionCombo, 4, 1);

  vertexDisplayLabel = new QLabel("Отображение вершин:", settingsTab);
  vertexDisplayCombo = new QComboBox(settingsTab);
  vertexDisplayCombo->addItem("Отсутствует",
                              static_cast<int>(VertexDisplayType::NONE));
  vertexDisplayCombo->addItem("Круг",
                              static_cast<int>(VertexDisplayType::CIRCLE));
  vertexDisplayCombo->addItem("Квадрат",
                              static_cast<int>(VertexDisplayType::SQUARE));
  vertexDisplayCombo->setCurrentIndex(0);
  vertexDisplayCombo->setMaximumWidth(250);

  settingsLayout->addWidget(vertexDisplayLabel, 5, 0);
  settingsLayout->addWidget(vertexDisplayCombo, 5, 1);

  rotationTimeLabel = new QLabel("Время: 0.00 сек", settingsTab);
  settingsLayout->addWidget(rotationTimeLabel, 5, 2);

  connect(rotationSpeedSlider, &QSlider::valueChanged, this, [this](int value) {
    float speed = value / 10.0f;
    speedLabel->setText(QString("Скорость: %1").arg(speed, 0, 'f', 1));
    emit rotationSpeedChanged(value);
  });

  connect(edgeThicknessSlider, &QSlider::valueChanged, this, [this](int value) {
    float thickness = value / 10.0f;
    edgeThicknessLabel->setText(
        QString("Толщина: %1").arg(thickness, 0, 'f', 1));
    emit edgeThicknessChanged(value);
  });

  connect(vertexSizeSlider, &QSlider::valueChanged, this, [this](int value) {
    float size = value / 2.0f;
    vertexSizeLabel->setText(QString("Размер вершин: %1").arg(size, 0, 'f', 1));
    emit vertexSizeChanged(value);
  });

  connect(lineTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, [this](int index) { emit lineTypeChanged(index); });

  connect(projectionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, [this](int index) {
            ProjectionType type = static_cast<ProjectionType>(
                projectionCombo->itemData(index).toInt());
            glWidget->setProjectionType(type);
            emit projectionTypeChanged(type);
          });

  connect(vertexDisplayCombo,
          QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          [this](int index) {
            VertexDisplayType type = static_cast<VertexDisplayType>(
                vertexDisplayCombo->itemData(index).toInt());
            glWidget->setVertexDisplayType(type);
            emit vertexDisplayTypeChanged(type);
          });

  tabWidget->addTab(settingsScrollArea, "Настройки");

  // === Вкладка "Виды камеры" ===
  QWidget* viewTab = new QWidget();
  QGridLayout* viewLayout = new QGridLayout(viewTab);
  viewLayout->setSpacing(3);

  frontViewButton = new QPushButton("Спереди", viewTab);
  backViewButton = new QPushButton("Сзади", viewTab);
  leftViewButton = new QPushButton("Слева", viewTab);
  rightViewButton = new QPushButton("Справа", viewTab);
  topViewButton = new QPushButton("Сверху", viewTab);
  bottomViewButton = new QPushButton("Снизу", viewTab);

  viewLayout->addWidget(frontViewButton, 0, 0);
  viewLayout->addWidget(backViewButton, 0, 1);
  viewLayout->addWidget(leftViewButton, 0, 2);
  viewLayout->addWidget(rightViewButton, 1, 0);
  viewLayout->addWidget(topViewButton, 1, 1);
  viewLayout->addWidget(bottomViewButton, 1, 2);

  connect(frontViewButton, &QPushButton::clicked, this, [this]() {
    glWidget->setViewFront();
    emit cameraViewChanged("front");
  });
  connect(backViewButton, &QPushButton::clicked, this, [this]() {
    glWidget->setViewBack();
    emit cameraViewChanged("back");
  });
  connect(leftViewButton, &QPushButton::clicked, this, [this]() {
    glWidget->setViewLeft();
    emit cameraViewChanged("left");
  });
  connect(rightViewButton, &QPushButton::clicked, this, [this]() {
    glWidget->setViewRight();
    emit cameraViewChanged("right");
  });
  connect(topViewButton, &QPushButton::clicked, this, [this]() {
    glWidget->setViewTop();
    emit cameraViewChanged("top");
  });
  connect(bottomViewButton, &QPushButton::clicked, this, [this]() {
    glWidget->setViewBottom();
    emit cameraViewChanged("bottom");
  });

  tabWidget->addTab(viewTab, "Виды камер");

  // === Вкладка "Трансформации" ===
  QWidget* transformTab = new QWidget();
  QScrollArea* transformScrollArea = new QScrollArea();
  transformScrollArea->setWidget(transformTab);
  transformScrollArea->setWidgetResizable(true);
  transformScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

  QGridLayout* transformLayout = new QGridLayout(transformTab);
  transformLayout->setSpacing(2);

  QLabel* moveLabel = new QLabel("Перемещение:", transformTab);
  moveLabel->setStyleSheet("font-weight: bold;");
  transformLayout->addWidget(moveLabel, 0, 0, 1, 3);

  moveXButton = new QPushButton("Переместить X", transformTab);
  moveYButton = new QPushButton("Переместить Y", transformTab);
  moveZButton = new QPushButton("Переместить Z", transformTab);
  moveXEdit = new QLineEdit("1.0", transformTab);
  moveYEdit = new QLineEdit("1.0", transformTab);
  moveZEdit = new QLineEdit("1.0", transformTab);

  moveXButton->setMaximumWidth(120);
  moveYButton->setMaximumWidth(120);
  moveZButton->setMaximumWidth(120);
  moveXEdit->setMaximumWidth(90);
  moveYEdit->setMaximumWidth(90);
  moveZEdit->setMaximumWidth(90);

  transformLayout->addWidget(moveXButton, 1, 0);
  transformLayout->addWidget(moveXEdit, 1, 1);
  transformLayout->addWidget(moveYButton, 1, 2);
  transformLayout->addWidget(moveYEdit, 1, 3);
  transformLayout->addWidget(moveZButton, 1, 4);
  transformLayout->addWidget(moveZEdit, 1, 5);

  QLabel* rotateLabel = new QLabel("Поворот:", transformTab);
  rotateLabel->setStyleSheet("font-weight: bold;");
  transformLayout->addWidget(rotateLabel, 2, 0, 1, 3);

  rotateXButton = new QPushButton("Повернуть X", transformTab);
  rotateYButton = new QPushButton("Повернуть Y", transformTab);
  rotateZButton = new QPushButton("Повернуть Z", transformTab);
  rotateXEdit = new QLineEdit("45.0", transformTab);
  rotateYEdit = new QLineEdit("45.0", transformTab);
  rotateZEdit = new QLineEdit("45.0", transformTab);

  rotateXButton->setMaximumWidth(120);
  rotateYButton->setMaximumWidth(120);
  rotateZButton->setMaximumWidth(120);
  rotateXEdit->setMaximumWidth(90);
  rotateYEdit->setMaximumWidth(90);
  rotateZEdit->setMaximumWidth(90);

  transformLayout->addWidget(rotateXButton, 3, 0);
  transformLayout->addWidget(rotateXEdit, 3, 1);
  transformLayout->addWidget(rotateYButton, 3, 2);
  transformLayout->addWidget(rotateYEdit, 3, 3);
  transformLayout->addWidget(rotateZButton, 3, 4);
  transformLayout->addWidget(rotateZEdit, 3, 5);

  QLabel* scaleLabel = new QLabel("Масштаб:", transformTab);
  scaleLabel->setStyleSheet("font-weight: bold;");
  transformLayout->addWidget(scaleLabel, 4, 0, 1, 6);

  QWidget* scaleContainer = new QWidget(transformTab);
  QHBoxLayout* scaleLayout = new QHBoxLayout(scaleContainer);
  scaleLayout->setContentsMargins(0, 0, 0, 0);
  scaleLayout->setSpacing(5);

  scaleButton = new QPushButton("Масштаб", scaleContainer);
  scaleEdit = new QLineEdit("1.1", scaleContainer);
  scaleButton->setMaximumWidth(120);
  scaleEdit->setMaximumWidth(90);

  scaleLayout->addWidget(scaleButton);
  scaleLayout->addWidget(scaleEdit);
  scaleLayout->addStretch();

  transformLayout->addWidget(scaleContainer, 5, 0, 1, 6);

  connect(moveXButton, &QPushButton::clicked, this, [this]() {
    bool ok;
    float value = moveXEdit->text().toFloat(&ok);
    if (ok) emit transformRequested(UserAction_t::MoveModel_X, value);
  });
  connect(moveYButton, &QPushButton::clicked, this, [this]() {
    bool ok;
    float value = moveYEdit->text().toFloat(&ok);
    if (ok) emit transformRequested(UserAction_t::MoveModel_Y, value);
  });
  connect(moveZButton, &QPushButton::clicked, this, [this]() {
    bool ok;
    float value = moveZEdit->text().toFloat(&ok);
    if (ok) emit transformRequested(UserAction_t::MoveModel_Z, value);
  });
  connect(rotateXButton, &QPushButton::clicked, this, [this]() {
    bool ok;
    float value = rotateXEdit->text().toFloat(&ok);
    if (ok) emit transformRequested(UserAction_t::RotateModel_X, value);
  });
  connect(rotateYButton, &QPushButton::clicked, this, [this]() {
    bool ok;
    float value = rotateYEdit->text().toFloat(&ok);
    if (ok) emit transformRequested(UserAction_t::RotateModel_Y, value);
  });
  connect(rotateZButton, &QPushButton::clicked, this, [this]() {
    bool ok;
    float value = rotateZEdit->text().toFloat(&ok);
    if (ok) emit transformRequested(UserAction_t::RotateModel_Z, value);
  });
  connect(scaleButton, &QPushButton::clicked, this, [this]() {
    bool ok;
    float value = scaleEdit->text().toFloat(&ok);
    if (ok) emit transformRequested(UserAction_t::ScaleModel, value);
  });

  tabWidget->addTab(transformScrollArea, "Трансформации");

  // === Вкладка "Экспорт и История" ===
  QWidget* exportTab = new QWidget();
  QVBoxLayout* exportLayout = new QVBoxLayout(exportTab);
  exportLayout->setSpacing(5);

  // Экспорт изображений
  QGroupBox* renderGroup = new QGroupBox("Экспорт изображений", exportTab);
  renderGroup->setMaximumHeight(60);
  QHBoxLayout* renderLayout = new QHBoxLayout(renderGroup);
  renderLayout->setSpacing(3);

  takeJpegButton = new QPushButton("JPEG", renderGroup);
  takeJpgButton = new QPushButton("JPG", renderGroup);
  takeBmpButton = new QPushButton("BMP", renderGroup);
  takePngButton = new QPushButton("PNG", renderGroup);
  takeGifButton = new QPushButton("GIF", renderGroup);

  takeJpegButton->setMaximumWidth(60);
  takeJpgButton->setMaximumWidth(60);
  takeBmpButton->setMaximumWidth(60);
  takePngButton->setMaximumWidth(60);
  takeGifButton->setMaximumWidth(60);

  renderLayout->addWidget(takeJpegButton);
  renderLayout->addWidget(takeJpgButton);
  renderLayout->addWidget(takeBmpButton);
  renderLayout->addWidget(takePngButton);
  renderLayout->addWidget(takeGifButton);
  renderLayout->addStretch();

  connect(takeJpegButton, &QPushButton::clicked, this,
          [this]() { emit takeImageRequest("JPEG"); });
  connect(takeJpgButton, &QPushButton::clicked, this,
          [this]() { emit takeImageRequest("JPG"); });
  connect(takeBmpButton, &QPushButton::clicked, this,
          [this]() { emit takeImageRequest("BMP"); });
  connect(takePngButton, &QPushButton::clicked, this,
          [this]() { emit takeImageRequest("PNG"); });
  connect(takeGifButton, &QPushButton::clicked, this, &View::recordGifRequest);

  exportLayout->addWidget(renderGroup);

  // История действий
  QGroupBox* historyGroup = new QGroupBox("История действий", exportTab);
  historyGroup->setMaximumHeight(60);
  QHBoxLayout* historyLayout = new QHBoxLayout(historyGroup);

  undoButton = new QPushButton("Отменить (Ctrl + Z)", historyGroup);
  redoButton = new QPushButton("Повторить (Ctrl + Y)", historyGroup);
  undoButton->setMaximumWidth(140);
  redoButton->setMaximumWidth(140);

  historyLayout->addWidget(undoButton);
  historyLayout->addWidget(redoButton);
  historyLayout->addStretch();

  connect(undoButton, &QPushButton::clicked, this, &View::undoRequested);
  connect(redoButton, &QPushButton::clicked, this, &View::redoRequested);

  exportLayout->addWidget(historyGroup);

  // Прогресс записи GIF
  gifStatusLabel_ = new QLabel("Запись GIF...");
  gifProgressBar_ = new QProgressBar();
  gifProgressBar_->setRange(0, 100);
  gifProgressBar_->setValue(0);
  gifStatusLabel_->setVisible(false);
  gifProgressBar_->setVisible(false);

  exportLayout->addWidget(gifStatusLabel_);
  exportLayout->addWidget(gifProgressBar_);
  exportLayout->addStretch();

  connect(this, &View::takeImageRequest, glWidget, &GLWidget::takeJpeg);
  connect(this, &View::recordGifRequest, glWidget, &GLWidget::recordGif);
  connect(glWidget, &GLWidget::saveStatus, this, &View::setStatus);

  tabWidget->addTab(exportTab, "Экспорт");

  controlMainLayout->addWidget(tabWidget);

  // Статус-бар
  statusLabel = new QLabel("Готов к загрузке модели", centralWidget);
  statusLabel->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
  statusLabel->setMargin(3);
  statusLabel->setMaximumHeight(25);
  mainLayout->addWidget(statusLabel);

  mainSplitter->setSizes({700, 400});
  mainSplitter->setStretchFactor(0, 3);
  mainSplitter->setStretchFactor(1, 1);

  // === Синхронизация контролов с сохранёнными настройками ===
  {
    const s21::UiSettings u = s21::SettingsStore::load();

    // Слайдеры: преобразуем реальные значения в шкалу UI
    rotationSpeedSlider->setValue(static_cast<int>(u.rotationSpeed * 10.0f));
    edgeThicknessSlider->setValue(static_cast<int>(u.edgeThickness * 10.0f));
    vertexSizeSlider->setValue(static_cast<int>(u.vertexSize * 2.0f));

    // Подписи
    speedLabel->setText(
        QString("Скорость: %1").arg(u.rotationSpeed, 0, 'f', 1));
    edgeThicknessLabel->setText(
        QString("Толщина: %1").arg(u.edgeThickness, 0, 'f', 1));
    vertexSizeLabel->setText(
        QString("Размер вершин: %1").arg(u.vertexSize, 0, 'f', 1));

    // combobox по itemData, а не по индексу
    auto setComboByData = [](QComboBox* box, int data) {
      for (int i = 0; i < box->count(); ++i)
        if (box->itemData(i).toInt() == data) {
          box->setCurrentIndex(i);
          break;
        }
    };
    setComboByData(lineTypeCombo, u.lineType);
    setComboByData(projectionCombo, u.projectionType);
    setComboByData(vertexDisplayCombo, u.vertexDisplayType);

    setWireframeButtonText(u.wireframe ? "Сплошная модель"
                                       : "Каркасная модель");
  }

  updateControlsState();
  setupKeyboardShortcuts();
}

void View::updateControlsState() {
  bool wireframeMode = glWidget->wireframeChosen();
  bool hasModel = glWidget->hasModel();

  rotationSpeedSlider->setEnabled(true);
  speedLabel->setEnabled(true);
  colorButton->setEnabled(true);
  axesButton->setEnabled(true);

  lineTypeCombo->setEnabled(wireframeMode && hasModel);
  lineTypeLabel->setEnabled(wireframeMode && hasModel);
  edgeThicknessSlider->setEnabled(hasModel);
  edgeThicknessLabel->setEnabled(hasModel);
  vertexSizeSlider->setEnabled(hasModel);
  vertexSizeLabel->setEnabled(hasModel);
  resetButton->setEnabled(hasModel);
  edgeColorButton->setEnabled(hasModel);
  vertexColorButton->setEnabled(hasModel);
  wireframeButton->setEnabled(hasModel);

  // Трансформации
  moveXButton->setEnabled(hasModel);
  moveYButton->setEnabled(hasModel);
  moveZButton->setEnabled(hasModel);
  rotateXButton->setEnabled(hasModel);
  rotateYButton->setEnabled(hasModel);
  rotateZButton->setEnabled(hasModel);
  scaleButton->setEnabled(hasModel);
  moveXEdit->setEnabled(hasModel);
  moveYEdit->setEnabled(hasModel);
  moveZEdit->setEnabled(hasModel);
  rotateXEdit->setEnabled(hasModel);
  rotateYEdit->setEnabled(hasModel);
  rotateZEdit->setEnabled(hasModel);
  scaleEdit->setEnabled(hasModel);

  // Виды
  frontViewButton->setEnabled(hasModel);
  backViewButton->setEnabled(hasModel);
  leftViewButton->setEnabled(hasModel);
  rightViewButton->setEnabled(hasModel);
  topViewButton->setEnabled(hasModel);
  bottomViewButton->setEnabled(hasModel);

  // Проекция / вершины
  projectionCombo->setEnabled(hasModel);
  projectionLabel->setEnabled(hasModel);
  vertexDisplayCombo->setEnabled(hasModel);
  vertexDisplayLabel->setEnabled(hasModel);

  wireframeButton->setText(wireframeMode ? "Сплошная модель"
                                         : "Каркасная модель");
}

void View::setupKeyboardShortcuts() {
  QShortcut* undoShortcut = new QShortcut(QKeySequence("Ctrl+Z"), this);
  connect(undoShortcut, &QShortcut::activated, this, &View::undoRequested);

  QShortcut* redoShortcut = new QShortcut(QKeySequence("Ctrl+Y"), this);
  connect(redoShortcut, &QShortcut::activated, this, &View::redoRequested);

  QShortcut* frontViewShortcut = new QShortcut(QKeySequence("1"), this);
  connect(frontViewShortcut, &QShortcut::activated, this,
          [this]() { frontViewButton->click(); });

  QShortcut* backViewShortcut = new QShortcut(QKeySequence("2"), this);
  connect(backViewShortcut, &QShortcut::activated, this,
          [this]() { backViewButton->click(); });

  QShortcut* leftViewShortcut = new QShortcut(QKeySequence("3"), this);
  connect(leftViewShortcut, &QShortcut::activated, this,
          [this]() { leftViewButton->click(); });

  QShortcut* rightViewShortcut = new QShortcut(QKeySequence("4"), this);
  connect(rightViewShortcut, &QShortcut::activated, this,
          [this]() { rightViewButton->click(); });

  QShortcut* topViewShortcut = new QShortcut(QKeySequence("5"), this);
  connect(topViewShortcut, &QShortcut::activated, this,
          [this]() { topViewButton->click(); });

  QShortcut* bottomViewShortcut = new QShortcut(QKeySequence("6"), this);
  connect(bottomViewShortcut, &QShortcut::activated, this,
          [this]() { bottomViewButton->click(); });

  QShortcut* wireframeShortcut = new QShortcut(QKeySequence("W"), this);
  connect(wireframeShortcut, &QShortcut::activated, this,
          [this]() { wireframeButton->click(); });

  QShortcut* resetShortcut = new QShortcut(QKeySequence("R"), this);
  connect(resetShortcut, &QShortcut::activated, this,
          [this]() { resetButton->click(); });
}

void View::setStatus(const QString& status) { statusLabel->setText(status); }

void View::setRotationTimeLabelText(const QString& text) {
  rotationTimeLabel->setText(text);
}

void View::setWireframeButtonText(const QString& text) {
  wireframeButton->setText(text);
}

GLWidget* View::getGLWidget() const { return glWidget; }

void View::showGifProgress(int maxValue) {
  gifStatusLabel_->setVisible(true);
  gifProgressBar_->setVisible(true);
  gifProgressBar_->setRange(0, maxValue);
  gifProgressBar_->setValue(0);
}

void View::updateGifProgress(int value) { gifProgressBar_->setValue(value); }

void View::finishGifProgress() {
  gifStatusLabel_->setText("GIF готов!");
  QTimer::singleShot(2000, this, [this]() {
    gifStatusLabel_->setVisible(false);
    gifProgressBar_->setVisible(false);
  });
}

void View::closeEvent(QCloseEvent* event) {
  // 1) Берём текущие значения из GLWidget
  s21::UiSettings u;
  u.background = glWidget->getBackgroundColor();
  u.edgeColor = glWidget->getEdgeColor();
  u.vertexColor = glWidget->getVertexColor();
  u.edgeThickness = glWidget->getEdgeThickness();
  u.vertexSize = glWidget->getVertexSize();
  u.rotationSpeed = glWidget->getRotationSpeed();
  u.wireframe = glWidget->wireframeChosen();

  // 2) Типы из комбобоксов (itemData соответствует enum-значениям)
  u.lineType = lineTypeCombo->currentData().toInt();
  u.projectionType = projectionCombo->currentData().toInt();
  u.vertexDisplayType = vertexDisplayCombo->currentData().toInt();

  // 3) Сохраняем
  s21::SettingsStore::save(u);

  QMainWindow::closeEvent(event);
}

void View::resetAllSettings() {
  // 1) Слайдеры/комбо (тики)
  rotationSpeedSlider->setValue(10);  // => 1.0
  edgeThicknessSlider->setValue(10);  // => 1.0
  vertexSizeSlider->setValue(10);     // => 5.0

  lineTypeCombo->setCurrentIndex(0);       // Сплошные
  projectionCombo->setCurrentIndex(0);     // Перспектива
  vertexDisplayCombo->setCurrentIndex(0);  // Нет точек

  // Подписи
  speedLabel->setText(QString("Скорость: %1").arg(1.0f, 0, 'f', 1));
  edgeThicknessLabel->setText(QString("Толщина: %1").arg(1.0f, 0, 'f', 1));
  vertexSizeLabel->setText(QString("Размер вершин: %1").arg(5.0f, 0, 'f', 1));

  // 2) Реальные значения для сохранения
  s21::UiSettings u;
  u.background = QColor(255, 255, 255, 255);
  u.edgeColor = QVector3D(1.0f, 0.0f, 0.0f);
  u.vertexColor = QVector3D(0.0f, 0.0f, 1.0f);
  u.rotationSpeed = 1.0f;
  u.edgeThickness = 1.0f;
  u.vertexSize = 5.0f;
  u.lineType = static_cast<int>(LineType::SOLID);
  u.projectionType = static_cast<int>(ProjectionType::PERSPECTIVE);
  u.vertexDisplayType = static_cast<int>(VertexDisplayType::NONE);
  u.wireframe = false;

  // 3) Применить и сохранить
  glWidget->applySettings(u);
  s21::SettingsStore::save(u);

  setWireframeButtonText("Каркасная модель");
  updateControlsState();
  setStatus("Настройки сброшены к значениям по умолчанию");
}

void View::setAxesButtonText(bool on) {
  axesButton->setText(on ? "Оси: ВКЛ" : "Оси: ВЫКЛ");
}
