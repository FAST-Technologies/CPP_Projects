#include "command.h"

#include <QDebug>

#include "../integration_adapter.h"  // TypeAdapter
#include "../model/model.h"
#include "../scope_timer/scope_timer.h"

namespace s21 {

MoveXCommand::MoveXCommand(float dx) : dx_(dx) {}

void MoveXCommand::Execute(Model& model) {
  SCOPE_TIMER("MoveXCommand::Execute");
  if (model.getVertices().isEmpty()) {
    qDebug() << "Cannot execute: model is empty";
    return;
  }
  if (dx_ == 0.0f) {
    qDebug() << "Warning: MoveX with zero value ignored";
    return;
  }
  auto vertices = TypeAdapter::convertVertices(model.getObjModel().vertices);
  model.getTransformer().Move(vertices, dx_, 0.0f, 0.0f);
  model.updateVertices(TypeAdapter::convertVerticesBack(vertices));
}

void MoveXCommand::Undo(Model& model) {
  if (model.getVertices().isEmpty()) {
    qDebug() << "Cannot undo: model is empty";
    return;
  }
  if (dx_ == 0.0f) {
    qDebug() << "Warning: Undo MoveX with zero value ignored";
    return;
  }
  auto vertices = TypeAdapter::convertVertices(model.getObjModel().vertices);
  model.getTransformer().Move(vertices, -dx_, 0.0f, 0.0f);
  model.updateVertices(TypeAdapter::convertVerticesBack(vertices));
}

MoveYCommand::MoveYCommand(float dy) : dy_(dy) {}

void MoveYCommand::Execute(Model& model) {
  if (model.getVertices().isEmpty()) {
    qDebug() << "Cannot execute: model is empty";
    return;
  }
  if (dy_ == 0.0f) {
    qDebug() << "Warning: MoveY with zero value ignored";
    return;
  }
  auto vertices = TypeAdapter::convertVertices(model.getObjModel().vertices);
  model.getTransformer().Move(vertices, 0.0f, dy_, 0.0f);
  model.updateVertices(TypeAdapter::convertVerticesBack(vertices));
}

void MoveYCommand::Undo(Model& model) {
  if (model.getVertices().isEmpty()) {
    qDebug() << "Cannot undo: model is empty";
    return;
  }
  if (dy_ == 0.0f) {
    qDebug() << "Warning: Undo MoveY with zero value ignored";
    return;
  }
  auto vertices = TypeAdapter::convertVertices(model.getObjModel().vertices);
  model.getTransformer().Move(vertices, 0.0f, -dy_, 0.0f);
  model.updateVertices(TypeAdapter::convertVerticesBack(vertices));
}

MoveZCommand::MoveZCommand(float dz) : dz_(dz) {}

void MoveZCommand::Execute(Model& model) {
  if (model.getVertices().isEmpty()) {
    qDebug() << "Cannot execute: model is empty";
    return;
  }
  if (dz_ == 0.0f) {
    qDebug() << "Warning: MoveZ with zero value ignored";
    return;
  }
  auto vertices = TypeAdapter::convertVertices(model.getObjModel().vertices);
  model.getTransformer().Move(vertices, 0.0f, 0.0f, dz_);
  model.updateVertices(TypeAdapter::convertVerticesBack(vertices));
}

void MoveZCommand::Undo(Model& model) {
  if (model.getVertices().isEmpty()) {
    qDebug() << "Cannot undo: model is empty";
    return;
  }
  if (dz_ == 0.0f) {
    qDebug() << "Warning: Undo MoveZ with zero value ignored";
    return;
  }
  auto vertices = TypeAdapter::convertVertices(model.getObjModel().vertices);
  model.getTransformer().Move(vertices, 0.0f, 0.0f, -dz_);
  model.updateVertices(TypeAdapter::convertVerticesBack(vertices));
}

RotateXCommand::RotateXCommand(float angle_deg) : angle_deg_(angle_deg) {}

void RotateXCommand::Execute(Model& model) {
  if (model.getVertices().isEmpty()) {
    qDebug() << "Cannot execute: model is empty";
    return;
  }
  auto vertices = TypeAdapter::convertVertices(model.getObjModel().vertices);
  model.getTransformer().RotateX(vertices, angle_deg_);
  model.updateVertices(TypeAdapter::convertVerticesBack(vertices));
}

void RotateXCommand::Undo(Model& model) {
  if (model.getVertices().isEmpty()) {
    qDebug() << "Cannot undo: model is empty";
    return;
  }
  auto vertices = TypeAdapter::convertVertices(model.getObjModel().vertices);
  model.getTransformer().RotateX(vertices, -angle_deg_);
  model.updateVertices(TypeAdapter::convertVerticesBack(vertices));
}

RotateYCommand::RotateYCommand(float angle_deg) : angle_deg_(angle_deg) {}

void RotateYCommand::Execute(Model& model) {
  if (model.getVertices().isEmpty()) {
    qDebug() << "Cannot execute: model is empty";
    return;
  }
  auto vertices = TypeAdapter::convertVertices(model.getObjModel().vertices);
  model.getTransformer().RotateY(vertices, angle_deg_);
  model.updateVertices(TypeAdapter::convertVerticesBack(vertices));
}

void RotateYCommand::Undo(Model& model) {
  if (model.getVertices().isEmpty()) {
    qDebug() << "Cannot undo: model is empty";
    return;
  }
  auto vertices = TypeAdapter::convertVertices(model.getObjModel().vertices);
  model.getTransformer().RotateY(vertices, -angle_deg_);
  model.updateVertices(TypeAdapter::convertVerticesBack(vertices));
}

RotateZCommand::RotateZCommand(float angle_deg) : angle_deg_(angle_deg) {}

void RotateZCommand::Execute(Model& model) {
  if (model.getVertices().isEmpty()) {
    qDebug() << "Cannot execute: model is empty";
    return;
  }
  auto vertices = TypeAdapter::convertVertices(model.getObjModel().vertices);
  model.getTransformer().RotateZ(vertices, angle_deg_);
  model.updateVertices(TypeAdapter::convertVerticesBack(vertices));
}

void RotateZCommand::Undo(Model& model) {
  if (model.getVertices().isEmpty()) {
    qDebug() << "Cannot undo: model is empty";
    return;
  }
  auto vertices = TypeAdapter::convertVertices(model.getObjModel().vertices);
  model.getTransformer().RotateZ(vertices, -angle_deg_);
  model.updateVertices(TypeAdapter::convertVerticesBack(vertices));
}

ScaleCommand::ScaleCommand(float scale_factor) : scale_factor_(scale_factor) {}

void ScaleCommand::Execute(Model& model) {
  if (model.getVertices().isEmpty()) {
    qDebug() << "Cannot execute: model is empty";
    return;
  }
  if (scale_factor_ <= 0.0f) {
    qDebug() << "Warning: Scale with non-positive value ignored";
    return;
  }
  auto vertices = TypeAdapter::convertVertices(model.getObjModel().vertices);
  model.getTransformer().Scale(vertices, scale_factor_, scale_factor_,
                               scale_factor_);
  model.updateVertices(TypeAdapter::convertVerticesBack(vertices));
}

void ScaleCommand::Undo(Model& model) {
  if (model.getVertices().isEmpty()) {
    qDebug() << "Cannot undo: model is empty";
    return;
  }
  if (scale_factor_ <= 0.0f) {
    qDebug() << "Warning: Undo Scale with non-positive value ignored";
    return;
  }
  auto vertices = TypeAdapter::convertVertices(model.getObjModel().vertices);
  float inv = 1.0f / scale_factor_;
  model.getTransformer().Scale(vertices, inv, inv, inv);
  model.updateVertices(TypeAdapter::convertVerticesBack(vertices));
}

// --------- Создатели команд ---------

std::unique_ptr<ICommand> MoveXCreator::create(float value) {
  return std::make_unique<MoveXCommand>(value);
}

std::unique_ptr<ICommand> MoveYCreator::create(float value) {
  return std::make_unique<MoveYCommand>(value);
}

std::unique_ptr<ICommand> MoveZCreator::create(float value) {
  return std::make_unique<MoveZCommand>(value);
}

std::unique_ptr<ICommand> RotateXCreator::create(float value) {
  return std::make_unique<RotateXCommand>(value);
}

std::unique_ptr<ICommand> RotateYCreator::create(float value) {
  return std::make_unique<RotateYCommand>(value);
}

std::unique_ptr<ICommand> RotateZCreator::create(float value) {
  return std::make_unique<RotateZCommand>(value);
}

std::unique_ptr<ICommand> ScaleCreator::create(float value) {
  return std::make_unique<ScaleCommand>(value);
}

void CommandFactory::registerCreator(UserAction_t action,
                                     std::unique_ptr<CommandCreator> creator) {
  creators_[action] = std::move(creator);
}

std::unique_ptr<ICommand> CommandFactory::createCommand(UserAction_t action,
                                                        float value) {
  SCOPE_TIMER("CommandFactory::createCommand");
  auto it = creators_.find(action);
  if (it != creators_.end()) {
    return it->second->create(value);
  }
  return nullptr;
}

void CommandHistory::ExecuteAndStore(std::unique_ptr<ICommand> command,
                                     Model& model) {
  if (command) {
    SCOPE_TIMER("CommandHistory::ExecuteAndStore");
    command->Execute(model);
    undo_stack_.push(std::move(command));
    while (!redo_stack_.empty()) redo_stack_.pop();
    qDebug() << "Command executed and stored. Undo stack size:"
             << undo_stack_.size() << " Redo stack size:" << redo_stack_.size();
  }
}

void CommandHistory::Undo(Model& model) {
  if (!undo_stack_.empty()) {
    auto command = std::move(undo_stack_.top());
    undo_stack_.pop();
    command->Undo(model);
    redo_stack_.push(std::move(command));
  }
}

void CommandHistory::Redo(Model& model) {
  if (!redo_stack_.empty()) {
    auto command = std::move(redo_stack_.top());
    redo_stack_.pop();
    command->Execute(model);
    undo_stack_.push(std::move(command));
  }
}

void CommandHistory::Clear() {
  while (!undo_stack_.empty()) undo_stack_.pop();
  while (!redo_stack_.empty()) redo_stack_.pop();
  qDebug() << "Command history cleared";
}

bool CommandHistory::CanUndo() const { return !undo_stack_.empty(); }
bool CommandHistory::CanRedo() const { return !redo_stack_.empty(); }

}  // namespace s21
