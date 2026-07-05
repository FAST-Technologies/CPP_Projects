#ifndef CPP4_3DVIEWER_V2_0_VIEW_VIEW_H_
#define CPP4_3DVIEWER_V2_0_VIEW_VIEW_H_

/// \file
/// \brief Главное окно приложения и панель управления 3D-просмотрщиком.

#include <QMainWindow>
#include <QString>

#include "../user_action.h"

class QLabel;
class QSlider;
class QPushButton;
class QComboBox;
class QProgressBar;
class QScrollArea;
class QTabWidget;
class QLineEdit;

namespace s21 {

class GLWidget;

enum class ProjectionType : int;
enum class VertexDisplayType : int;

#ifndef slots
#define slots Q_SLOTS
#endif

/**
 * \brief Класс представления (View): главное окно + панель управления.
 *
 * Отвечает за создание и компоновку UI, маршрутизацию событий (сигналы),
 * а также синхронизацию состояния контролов с \ref GLWidget.
 *
 * \sa GLWidget, SettingsStore
 */
class View : public QMainWindow {
  Q_OBJECT

 public:
  /// \brief Создать окно.
  explicit View(QWidget* parent = nullptr);
  /// \brief Виртуальный деструктор.
  ~View() override;

  /// \brief Доступ к OpenGL-виджету.
  GLWidget* getGLWidget() const;

  /// \brief Кнопка Undo (для контроллера).
  QPushButton* getUndoButton() const { return undoButton; }
  /// \brief Кнопка Redo (для контроллера).
  QPushButton* getRedoButton() const { return redoButton; }

  /// \brief Установить текст строки состояния.
  void setStatus(const QString& text);
  /// \brief Обновить доступность контролов исходя из состояния модели.
  void updateControlsState();
  /// \brief Обновить подпись кнопки «Каркас/Сплошная».
  void setWireframeButtonText(const QString& text);
  /// \brief Обновить метку «Время вращения».
  void setRotationTimeLabelText(const QString& text);

  /// \name Индикация прогресса записи GIF
  ///@{
  void showGifProgress(int maxValue);
  void updateGifProgress(int value);
  void finishGifProgress();
  ///@}

  /// \brief Сбросить настройки UI к значениям по умолчанию (и сохранить).
  void resetAllSettings();

  /// \brief Обновить текст кнопки осей координат.
  void setAxesButtonText(bool on);

 signals:
  /// \name Сигналы пользовательских действий
  ///@{
  void openFileRequested();
  void resetCameraRequested();
  void rotationSpeedChanged(int value);
  void backgroundColorChanged();
  void edgeColorChanged();
  void vertexColorChanged();
  void edgeThicknessChanged(int value);
  void vertexSizeChanged(int value);
  void toggleWireframe();
  void lineTypeChanged(int index);

  /// \note Этот сигнал не используется напрямую в текущей связке
  ///       (контроллер слушает GLWidget). Сохранён для совместимости.
  void rotationTimeUpdated(float timeSeconds);

  void transformRequested(UserAction_t action, float value = 0.0f);
  void undoRequested();
  void redoRequested();
  void projectionTypeChanged(ProjectionType type);
  void vertexDisplayTypeChanged(VertexDisplayType type);
  void cameraViewChanged(const QString& viewName);

  /// \brief Запрос сохранить изображение в формате \p format.
  void takeImageRequest(const QString& format);
  /// \brief Запрос начать запись GIF.
  void recordGifRequest();
  ///@}

 private:
  /// OpenGL-виджет для визуализации модели.
  GLWidget* glWidget = nullptr;

  // Метки/подписи:
  QLabel* statusLabel = nullptr;
  QLabel* speedLabel = nullptr;
  QLabel* rotationTimeLabel = nullptr;
  QLabel* edgeThicknessLabel = nullptr;
  QLabel* vertexSizeLabel = nullptr;
  QLabel* lineTypeLabel = nullptr;
  QLabel* projectionLabel = nullptr;
  QLabel* vertexDisplayLabel = nullptr;
  QLabel* gifStatusLabel_ = nullptr;

  // Слайдеры:
  QSlider* rotationSpeedSlider = nullptr;
  QSlider* edgeThicknessSlider = nullptr;
  QSlider* vertexSizeSlider = nullptr;

  // Комбобоксы:
  QComboBox* lineTypeCombo = nullptr;
  QComboBox* projectionCombo = nullptr;
  QComboBox* vertexDisplayCombo = nullptr;

  // Кнопки верхней панели:
  QPushButton* openButton = nullptr;
  QPushButton* resetButton = nullptr;
  QPushButton* colorButton = nullptr;
  QPushButton* edgeColorButton = nullptr;
  QPushButton* vertexColorButton = nullptr;
  QPushButton* wireframeButton = nullptr;
  QPushButton* resetAllButton = nullptr;
  QPushButton* axesButton = nullptr;

  // Кнопки видов камеры:
  QPushButton* frontViewButton = nullptr;
  QPushButton* backViewButton = nullptr;
  QPushButton* leftViewButton = nullptr;
  QPushButton* rightViewButton = nullptr;
  QPushButton* topViewButton = nullptr;
  QPushButton* bottomViewButton = nullptr;

  // Кнопки трансформаций:
  QPushButton* moveXButton = nullptr;
  QPushButton* moveYButton = nullptr;
  QPushButton* moveZButton = nullptr;
  QPushButton* rotateXButton = nullptr;
  QPushButton* rotateYButton = nullptr;
  QPushButton* rotateZButton = nullptr;
  QPushButton* scaleButton = nullptr;

  // Экспорт:
  QPushButton* takeJpegButton = nullptr;
  QPushButton* takeJpgButton = nullptr;
  QPushButton* takeBmpButton = nullptr;
  QPushButton* takePngButton = nullptr;
  QPushButton* takeGifButton = nullptr;

  // Поля ввода:
  QLineEdit* moveXEdit = nullptr;
  QLineEdit* moveYEdit = nullptr;
  QLineEdit* moveZEdit = nullptr;
  QLineEdit* rotateXEdit = nullptr;
  QLineEdit* rotateYEdit = nullptr;
  QLineEdit* rotateZEdit = nullptr;
  QLineEdit* scaleEdit = nullptr;

  // История:
  QPushButton* undoButton = nullptr;
  QPushButton* redoButton = nullptr;

  // Прогресс GIF:
  QProgressBar* gifProgressBar_ = nullptr;

 private slots:
  /// \brief Построить UI (вызов из конструктора).
  void setupUI();
  /// \brief Настроить хоткеи (Ctrl+Z/Y, 1–6, W, R и т.п.).
  void setupKeyboardShortcuts();

 protected:
  /// \brief Сохранение настроек перед закрытием окна.
  void closeEvent(QCloseEvent* event) override;
};

}  // namespace s21

#endif  // CPP4_3DVIEWER_V2_0_VIEW_VIEW_H_
