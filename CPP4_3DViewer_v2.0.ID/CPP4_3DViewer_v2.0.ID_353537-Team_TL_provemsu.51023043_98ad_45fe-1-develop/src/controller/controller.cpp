#include "controller.h"

#include <QColorDialog>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QPushButton>

#include "../facade.h"
#include "../glwidget/glwidget.h"
#include "../scope_timer/scope_timer.h"

using namespace s21;

Controller::Controller(View* view, QObject* parent)
    : QObject(parent), view(view), facade(s21::Facade::getInstance()) {
  connect(view, &View::openFileRequested, this,
          &Controller::onOpenFileRequested);
  connect(view, &View::resetCameraRequested, this,
          &Controller::onResetCameraRequested);
  connect(view, &View::rotationSpeedChanged, this,
          &Controller::onRotationSpeedChanged);
  connect(view, &View::backgroundColorChanged, this,
          &Controller::onBackgroundColorChanged);
  connect(view, &View::edgeColorChanged, this, &Controller::onEdgeColorChanged);
  connect(view, &View::vertexColorChanged, this,
          &Controller::onVertexColorChanged);
  connect(view, &View::edgeThicknessChanged, this,
          &Controller::onEdgeThicknessChanged);
  connect(view, &View::vertexSizeChanged, this,
          &Controller::onVertexSizeChanged);
  connect(view, &View::toggleWireframe, this, &Controller::onWireframeToggled);
  connect(view, &View::lineTypeChanged, this, &Controller::onLineTypeChanged);

  connect(view, &View::transformRequested, this,
          &Controller::onTransformRequested);
  connect(view, &View::undoRequested, this, &Controller::onUndoRequested);
  connect(view, &View::redoRequested, this, &Controller::onRedoRequested);
  connect(view, &View::projectionTypeChanged, this,
          &Controller::onProjectionTypeChanged);
  connect(view, &View::vertexDisplayTypeChanged, this,
          &Controller::onVertexDisplayTypeChanged);
  connect(view, &View::cameraViewChanged, this,
          &Controller::onCameraViewChanged);
  connect(view->getGLWidget(), &GLWidget::rotationTimeUpdated, this,
          &Controller::onRotationTimeUpdated);

  view->getGLWidget()->setFacade(facade);
  updateModelInfo();
  updateControlsState();
}

Controller::~Controller() {}

void Controller::onOpenFileRequested() {
  QString relative_path = "./objects";
  QString absolute_path = QDir(relative_path).absolutePath();

  if (!QDir(absolute_path).exists()) {
    absolute_path = "";
  }

  QString filename =
      QFileDialog::getOpenFileName(view, "Открыть OBJ файл", absolute_path,
                                   "OBJ Files (*.obj);;All Files (*)");
  if (!filename.isEmpty()) {
    qDebug() << "Loading file:" << filename;
    if (facade.loadModel(filename.toStdString())) {
      view->getGLWidget()->updateModel();
      view->getGLWidget()->update();
      view->getGLWidget()->setCurrentPath(filename);
      view->getGLWidget()->recalcAllModelMetrics();
      updateModelInfo();
      updateControlsState();
    } else {
      QMessageBox::warning(view, "Ошибка", "Не удалось загрузить модель!");
      view->setStatus("Ошибка загрузки модели");
    }
  }
}

void Controller::onResetCameraRequested() {
  view->getGLWidget()->resetCamera();
  view->setStatus("Камера сброшена");
}

void Controller::onRotationSpeedChanged(int value) {
  float speed = value / 10.0f;
  view->getGLWidget()->setRotationSpeed(speed);
  view->setStatus(QString("Скорость: %1").arg(speed, 0, 'f', 1));
}

void Controller::onBackgroundColorChanged() {
  QColor color = QColorDialog::getColor(
      view->getGLWidget()->getBackgroundColor(), view, "Выберите цвет фона");
  if (color.isValid()) {
    view->getGLWidget()->setBackgroundColor(color);
    view->setStatus("Цвет фона изменен");
  }
}

void Controller::onEdgeColorChanged() {
  QVector3D currentColor = view->getGLWidget()->getEdgeColor();
  QColor color = QColorDialog::getColor(
      QColor(currentColor.x() * 255, currentColor.y() * 255,
             currentColor.z() * 255),
      view, "Выберите цвет рёбер");
  if (color.isValid()) {
    view->getGLWidget()->setEdgeColor(
        QVector3D(color.redF(), color.greenF(), color.blueF()));
    view->setStatus("Цвет рёбер изменен");
  }
}

void Controller::onVertexColorChanged() {
  QVector3D currentColor = view->getGLWidget()->getVertexColor();
  QColor color = QColorDialog::getColor(
      QColor(currentColor.x() * 255, currentColor.y() * 255,
             currentColor.z() * 255),
      view, "Выберите цвет вершин");
  if (color.isValid()) {
    view->getGLWidget()->setVertexColor(
        QVector3D(color.redF(), color.greenF(), color.blueF()));
    view->setStatus("Цвет вершин изменен");
  }
}

void Controller::onEdgeThicknessChanged(int value) {
  float thickness = value / 10.0f;
  view->getGLWidget()->setEdgeThickness(thickness);
  view->setStatus(QString("Толщина: %1").arg(thickness, 0, 'f', 1));
}

void Controller::onVertexSizeChanged(int value) {
  float size = value / 2.0f;
  view->getGLWidget()->setVertexSize(size);
  view->setStatus(QString("Размер: %1").arg(size, 0, 'f', 1));
}

void Controller::onWireframeToggled() {
  view->getGLWidget()->setWireframeMode(
      !view->getGLWidget()->wireframeChosen());
  updateControlsState();
  view->setStatus(view->getGLWidget()->wireframeChosen()
                      ? "Режим каркаса включен"
                      : "Режим каркаса выключен");
}

void Controller::onLineTypeChanged(int index) {
  view->getGLWidget()->setLineType(static_cast<LineType>(index));
  QString typeText;
  switch (static_cast<LineType>(index)) {
    case LineType::SOLID:
      typeText = "сплошные (скрытые пунктир)";
      break;
    case LineType::DASHED:
      typeText = "пунктирные";
      break;
    case LineType::DOTDASH:
      typeText = "штрихпунктирные";
      break;
  }
  view->setStatus(QString("Тип линий: %1").arg(typeText));
}

void Controller::onRotationTimeUpdated(float timeSeconds) {
  view->setRotationTimeLabelText(
      QString("Время: %1 сек").arg(timeSeconds, 0, 'f', 2));
}

void Controller::updateModelInfo() {
  SCOPE_TIMER("Controller::updateModelInfo");
  const ObjModel& model = facade.getObjModel();
  if (!model.renderVertices.isEmpty()) {
    if (view->getGLWidget()->getVerticesCountClassic() == 0 ||
        view->getGLWidget()->getFaceCount() == 0) {
      view->getGLWidget()->recalcAllModelMetrics();
    }
    QString info =
        QString(
            "Модель: %1 | Вершины (общие): %2 | Вершины (триангуляция): %3 | "
            "Грани (общие): %4 | Грани (триангуляция): %5 | Рёбра (общие): %6 "
            "| Рёбра (триангуляция): %7")
            .arg(QFileInfo(model.modelName).fileName())
            .arg(view->getGLWidget()->getVerticesCountClassic())
            .arg(view->getGLWidget()->getVerticesCountTriangulate())
            .arg(view->getGLWidget()->getFaceCountClassic())
            .arg(view->getGLWidget()->getFaceCount())
            .arg(view->getGLWidget()->getTrueEdgeCount())
            .arg(view->getGLWidget()->getEdgeCount());

    if (view->getGLWidget()->hasMaterials()) {
      info += " | Материалы: ЕСТЬ";
    } else {
      info += " | Материалы: НЕТ";
    }
    view->setStatus(info);
    updateControlsState();
  } else {
    view->setStatus("Готов к загрузке модели");
    updateControlsState();
  }
}

void Controller::onTransformRequested(UserAction_t action, float value) {
  facade.applyAction(action, value);
  view->getGLWidget()->updateModel();
  updateModelInfo();
  updateControlsState();

  QString actionName;
  switch (action) {
    case UserAction_t::MoveModel_X:
      actionName = "Перемещение X";
      break;
    case UserAction_t::MoveModel_Y:
      actionName = "Перемещение Y";
      break;
    case UserAction_t::MoveModel_Z:
      actionName = "Перемещение Z";
      break;
    case UserAction_t::RotateModel_X:
      actionName = "Поворот X";
      break;
    case UserAction_t::RotateModel_Y:
      actionName = "Поворот Y";
      break;
    case UserAction_t::RotateModel_Z:
      actionName = "Поворот Z";
      break;
    case UserAction_t::ScaleModel:
      actionName = "Масштаб";
      break;
    default:
      actionName = "Трансформация";
      break;
  }
  view->setStatus(QString("%1: %2").arg(actionName).arg(value));
}

void Controller::onUndoRequested() {
  if (facade.canUndo()) {
    facade.undo();
    view->getGLWidget()->updateModel();
    updateModelInfo();
    view->setStatus("Отмена выполнена");
    updateControlsState();
  } else {
    view->setStatus("Нет действий для отмены");
  }
}

void Controller::onRedoRequested() {
  if (facade.canRedo()) {
    facade.redo();
    view->getGLWidget()->updateModel();
    updateModelInfo();
    view->setStatus("Повтор выполнено");
    updateControlsState();
  } else {
    view->setStatus("Нет действий для повтора");
  }
}

void Controller::onProjectionTypeChanged(ProjectionType type) {
  QString typeName =
      (type == ProjectionType::PERSPECTIVE) ? "центральная" : "параллельная";
  view->setStatus(QString("Проекция изменена на %1").arg(typeName));
}

void Controller::onVertexDisplayTypeChanged(VertexDisplayType type) {
  QString typeName;
  switch (type) {
    case VertexDisplayType::NONE:
      typeName = "отсутствует";
      break;
    case VertexDisplayType::CIRCLE:
      typeName = "круг";
      break;
    case VertexDisplayType::SQUARE:
      typeName = "квадрат";
      break;
  }
  view->setStatus(QString("Отображение вершин: %1").arg(typeName));
}

void Controller::onCameraViewChanged(const QString& viewName) {
  view->setStatus(QString("Вид изменен: %1").arg(viewName));
}

void Controller::updateControlsState() {
  SCOPE_TIMER("Controller::updateControlsState");
  bool hasModel = view->getGLWidget()->hasModel();
  bool canUndo = facade.canUndo();
  bool canRedo = facade.canRedo();
  view->getUndoButton()->setEnabled(hasModel && canUndo);
  view->getRedoButton()->setEnabled(hasModel && canRedo);
  view->updateControlsState();
}
