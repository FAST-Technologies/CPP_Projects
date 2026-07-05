#ifndef CPP4_3DVIEWER_V2_0_FACADE_FACADE_H_
#define CPP4_3DVIEWER_V2_0_FACADE_FACADE_H_

/// \file
/// \brief Фасад для работы с моделью: загрузка, трансформации, доступ к данным.

#include <string>

#include "command/command.h"
#include "model/model.h"
#include "scope_timer/scope_timer.h"

namespace s21 {

class Facade {
 public:
  static Facade& getInstance() {
    SCOPE_TIMER("Facade::getInstance");
    static Facade instance;
    return instance;
  }

  bool loadModel(const std::string& filepath) {
    model_.LoadModel(filepath);
    return !model_.getObjModel().renderVertices.isEmpty();
  }

  void applyAction(UserAction_t action, float value = 0.0f) {
    model_.userInput(action, value);
  }

  const ObjModel& getObjModel() const { return model_.getObjModel(); }

  bool canUndo() const { return model_.canUndo(); }
  bool canRedo() const { return model_.canRedo(); }

  void undo() { model_.undo(); }
  void redo() { model_.redo(); }

 private:
  Model model_;
};

}  // namespace s21

#endif  // CPP4_3DVIEWER_V2_0_FACADE_FACADE_H_
