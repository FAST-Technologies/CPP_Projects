#ifndef CPP4_3DVIEWER_V2_0_CONTROLLER_CONTROLLER_H_
#define CPP4_3DVIEWER_V2_0_CONTROLLER_CONTROLLER_H_

/// \file
/// \brief Контроллер связывает View и Facade: обрабатывает сигналы UI,
///        вызывает действия фасада и обновляет представление.

#include <QObject>
#include <QString>

#include "../user_action.h"  // UserAction_t
#include "../view/view.h"  // ProjectionType, VertexDisplayType (и тип View)

namespace s21 {

class Facade;  // вперёд, чтобы не тянуть заголовок в .h

/// \brief Qt-контроллер, координирующий View и бизнес-логику (Facade).
class Controller : public QObject {
  Q_OBJECT

 public:
  /// \brief Создать контроллер и подключить сигналы View.
  explicit Controller(View* view, QObject* parent = nullptr);

  /// \brief Виртуальный деструктор.
  ~Controller() override;

 private slots:
  /// \brief Открыть OBJ-файл через диалог.
  void onOpenFileRequested();
  /// \brief Сбросить камеру.
  void onResetCameraRequested();
  /// \brief Изменить скорость вращения (слайдер UI).
  void onRotationSpeedChanged(int value);
  /// \brief Выбрать цвет фона.
  void onBackgroundColorChanged();
  /// \brief Выбрать цвет рёбер.
  void onEdgeColorChanged();
  /// \brief Выбрать цвет вершин.
  void onVertexColorChanged();
  /// \brief Изменить толщину рёбер.
  void onEdgeThicknessChanged(int value);
  /// \brief Изменить размер вершин.
  void onVertexSizeChanged(int value);
  /// \brief Включить/выключить каркас.
  void onWireframeToggled();
  /// \brief Изменить тип линии (сплошные/пунктир/штрихпунктир).
  void onLineTypeChanged(int index);
  /// \brief Обновить отображаемое время вращения.
  void onRotationTimeUpdated(float timeSeconds);
  /// \brief Применить трансформацию (перемещение/поворот/масштаб).
  void onTransformRequested(UserAction_t action, float value);
  /// \brief Отмена последнего действия.
  void onUndoRequested();
  /// \brief Повтор отменённого действия.
  void onRedoRequested();
  /// \brief Сменить тип проекции.
  void onProjectionTypeChanged(ProjectionType type);
  /// \brief Сменить отображение вершин.
  void onVertexDisplayTypeChanged(VertexDisplayType type);
  /// \brief Сменить преднастроенный вид камеры.
  void onCameraViewChanged(const QString& viewName);

 private:
  View* view;  ///< Активное представление (UI).
  Facade& facade;  ///< Ссылка на фасад (бизнес-логика, синглтон).

  /// \brief Обновить строку статуса по данным модели.
  void updateModelInfo();

  /// \brief Обновить доступность/состояние элементов управления.
  void updateControlsState();
};

}  // namespace s21

#endif  // CPP4_3DVIEWER_V2_0_CONTROLLER_CONTROLLER_H_
