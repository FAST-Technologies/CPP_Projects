// model.cpp
/// \file
/// \brief Реализация уровня модели приложения: загрузка, команды, Undo/Redo.
/// \details Важные детали:
///  - Трансформации применяются через Command-паттерн поверх рабочего массива
///  vertices_.\n
///  - После Undo/Redo/внешнего обновления вызывается prepareRenderData().\n
///  - Порядок загрузки исправлен: сначала центрирование, затем подготовка
///  рендера.

#include "model.h"

using namespace s21;

Model& Model::get_model_instance() {
  static Model model;
  return model;
}

Model::Model() {
  factory_.registerCreator(UserAction_t::MoveModel_X,
                           std::make_unique<MoveXCreator>());
  factory_.registerCreator(UserAction_t::MoveModel_Y,
                           std::make_unique<MoveYCreator>());
  factory_.registerCreator(UserAction_t::MoveModel_Z,
                           std::make_unique<MoveZCreator>());
  factory_.registerCreator(UserAction_t::RotateModel_X,
                           std::make_unique<RotateXCreator>());
  factory_.registerCreator(UserAction_t::RotateModel_Y,
                           std::make_unique<RotateYCreator>());
  factory_.registerCreator(UserAction_t::RotateModel_Z,
                           std::make_unique<RotateZCreator>());
  factory_.registerCreator(UserAction_t::ScaleModel,
                           std::make_unique<ScaleCreator>());
}

Model::~Model() {}

void Model::LoadModel(const std::string& filepath) {
  SCOPE_TIMER_WARN("Model::LoadModel (IO+parse+center+prepare)", 500.0);
  if (filepath.empty()) {
    qWarning() << "Empty file path provided to LoadModel";
    return;
  }

  filename_ = filepath;
  objModel_.clear(false);

  qDebug() << "Loading model from file:" << QString::fromStdString(filepath);

  if (ObjLoader::loadFromFile(QString::fromStdString(filepath), objModel_)) {
    // 1) центрирование геометрии
    objModel_.centerModel();
    // 2) синхронизация рабочего представления для команд
    vertices_ = TypeAdapter::convertVertices(objModel_.vertices);
    // 3) подготовка буферов рендера
    objModel_.prepareRenderData();
    // 4) метаданные
    objModel_.filePath = QString::fromStdString(filepath);

    qDebug() << "Model loaded successfully:" << " vertices="
             << objModel_.vertices.size() << " faces=" << objModel_.faces.size()
             << " renderVertices=" << objModel_.renderVertices.size()
             << " renderIndices=" << objModel_.renderIndices.size()
             << " materials=" << objModel_.materials.size();
  } else {
    qWarning() << "Failed to load model from:"
               << QString::fromStdString(filepath);
  }
}

bool Model::applyTransformation(UserAction_t action, float value) {
  SCOPE_TIMER_WARN("Model::applyTransformation UserAction", 500.0);
  try {
    if (objModel_.vertices.isEmpty()) {
      qDebug() << "Cannot apply transformation: model is empty";
      return false;
    }

    auto command = factory_.createCommand(action, value);
    if (!command) {
      qDebug() << "Unsupported action or failed to create command:"
               << static_cast<int>(action);
      return false;
    }

    // рабочее представление -> выполнить команду -> вернуть в ObjModel
    vertices_ = TypeAdapter::convertVertices(objModel_.vertices);
    history_.ExecuteAndStore(std::move(command), *this);
    objModel_.vertices = TypeAdapter::convertVerticesBack(vertices_);
    objModel_.prepareRenderData();

    qDebug() << "Transformation applied: action=" << static_cast<int>(action)
             << " value=" << value << " CanUndo=" << history_.CanUndo()
             << " CanRedo=" << history_.CanRedo();
    return true;
  } catch (const std::exception& e) {
    qWarning() << "Exception during transformation:" << e.what();
    return false;
  } catch (...) {
    qWarning() << "Unknown exception during transformation";
    return false;
  }
}

void Model::clear_model() {
  objModel_.clear(false);
  vertices_.clear();
  filename_.clear();
  history_.Clear();
  qDebug() << "Model cleared";
}

void Model::userInput(UserAction_t action, float value) {
  (void)applyTransformation(action, value);
}

const ObjModel& Model::getObjModel() const {
  SCOPE_TIMER("Model::getObjModel");
  return objModel_;
}

void Model::setObjModel(const ObjModel& model) {
  objModel_ = model;
  vertices_ = TypeAdapter::convertVertices(objModel_.vertices);
}

void Model::undo() {
  if (history_.CanUndo()) {
    history_.Undo(*this);
    objModel_.vertices = TypeAdapter::convertVerticesBack(vertices_);
    objModel_.prepareRenderData();
    qDebug() << "Undo performed:" << " CanUndo=" << history_.CanUndo()
             << " CanRedo=" << history_.CanRedo();
  }
}

void Model::redo() {
  if (history_.CanRedo()) {
    history_.Redo(*this);
    objModel_.vertices = TypeAdapter::convertVerticesBack(vertices_);
    objModel_.prepareRenderData();
    qDebug() << "Redo performed:" << " CanUndo=" << history_.CanUndo()
             << " CanRedo=" << history_.CanRedo();
  }
}

void Model::updateVertices(const QVector<QVector3D>& newVertices) {
  objModel_.vertices = newVertices;
  vertices_ = TypeAdapter::convertVertices(newVertices);
  objModel_.prepareRenderData();
}
