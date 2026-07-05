#ifndef CPP4_3DVIEWER_V2_0_COMMAND_COMMAND_H_
#define CPP4_3DVIEWER_V2_0_COMMAND_COMMAND_H_

/// \file
/// \brief Командная подсистема: интерфейс команд трансформации,
///        фабрика и история (Undo/Redo).

#include <memory>
#include <stack>
#include <unordered_map>

#include "../user_action.h"  // UserAction_t

namespace s21 {

class Model;

/// \brief Интерфейс базовой команды трансформации.
class ICommand {
 public:
  /// \brief Выполнить команду (применить трансформацию к модели).
  virtual void Execute(Model& model) = 0;
  /// \brief Отменить ранее выполненную команду.
  virtual void Undo(Model& model) = 0;
  virtual ~ICommand() = default;
};

/// \brief Сдвиг модели по оси X.
class MoveXCommand : public ICommand {
 public:
  explicit MoveXCommand(float dx);
  void Execute(Model& model) override;
  void Undo(Model& model) override;

 private:
  float dx_;
};

/// \brief Сдвиг модели по оси Y.
class MoveYCommand : public ICommand {
 public:
  explicit MoveYCommand(float dy);
  void Execute(Model& model) override;
  void Undo(Model& model) override;

 private:
  float dy_;
};

/// \brief Сдвиг модели по оси Z.
class MoveZCommand : public ICommand {
 public:
  explicit MoveZCommand(float dz);
  void Execute(Model& model) override;
  void Undo(Model& model) override;

 private:
  float dz_;
};

/// \brief Поворот вокруг оси X.
class RotateXCommand : public ICommand {
 public:
  explicit RotateXCommand(float angle_deg);
  void Execute(Model& model) override;
  void Undo(Model& model) override;

 private:
  float angle_deg_;
};

/// \brief Поворот вокруг оси Y.
class RotateYCommand : public ICommand {
 public:
  explicit RotateYCommand(float angle_deg);
  void Execute(Model& model) override;
  void Undo(Model& model) override;

 private:
  float angle_deg_;
};

/// \brief Поворот вокруг оси Z.
class RotateZCommand : public ICommand {
 public:
  explicit RotateZCommand(float angle_deg);
  void Execute(Model& model) override;
  void Undo(Model& model) override;

 private:
  float angle_deg_;
};

/// \brief Равномерный масштаб по всем осям.
class ScaleCommand : public ICommand {
 public:
  explicit ScaleCommand(float scale_factor);
  void Execute(Model& model) override;
  void Undo(Model& model) override;

 private:
  float scale_factor_;
};

/// \brief Абстрактный создатель команды (Factory Method).
class CommandCreator {
 public:
  /// \brief Создать команду с параметром value.
  virtual std::unique_ptr<ICommand> create(float value) = 0;
  virtual ~CommandCreator() = default;
};

// Конкретные создатели:

class MoveXCreator : public CommandCreator {
 public:
  std::unique_ptr<ICommand> create(float value) override;
};

class MoveYCreator : public CommandCreator {
 public:
  std::unique_ptr<ICommand> create(float value) override;
};

class MoveZCreator : public CommandCreator {
 public:
  std::unique_ptr<ICommand> create(float value) override;
};

class RotateXCreator : public CommandCreator {
 public:
  std::unique_ptr<ICommand> create(float value) override;
};

class RotateYCreator : public CommandCreator {
 public:
  std::unique_ptr<ICommand> create(float value) override;
};

class RotateZCreator : public CommandCreator {
 public:
  std::unique_ptr<ICommand> create(float value) override;
};

class ScaleCreator : public CommandCreator {
 public:
  std::unique_ptr<ICommand> create(float value) override;
};

/// \brief Фабрика команд: сопоставляет UserAction_t и создатель команды.
class CommandFactory {
 public:
  /// \brief Зарегистрировать создатель для действия.
  void registerCreator(UserAction_t action,
                       std::unique_ptr<CommandCreator> creator);

  /// \brief Создать команду для действия и параметра value.
  std::unique_ptr<ICommand> createCommand(UserAction_t action, float value);

 private:
  std::unordered_map<UserAction_t, std::unique_ptr<CommandCreator>> creators_;
};

/// \brief История команд: стеки undo/redo.
class CommandHistory {
 public:
  /// \brief Выполнить команду и сохранить в истории (очищает redo-стек).
  void ExecuteAndStore(std::unique_ptr<ICommand> command, Model& model);

  /// \brief Отменить последнюю выполненную команду.
  void Undo(Model& model);

  /// \brief Повторить последнюю отменённую команду.
  void Redo(Model& model);

  /// \brief Очистить историю (undo/redo).
  void Clear();

  /// \brief Доступна ли отмена.
  bool CanUndo() const;

  /// \brief Доступен ли повтор.
  bool CanRedo() const;

 private:
  std::stack<std::unique_ptr<ICommand>> undo_stack_;
  std::stack<std::unique_ptr<ICommand>> redo_stack_;
};

}  // namespace s21

#endif  // CPP4_3DVIEWER_V2_0_COMMAND_COMMAND_H_
