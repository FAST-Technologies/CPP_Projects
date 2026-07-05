
# Жизненный цикл команды в 3DViewer v2.0

Этот документ описывает **подробный жизненный цикл действия пользователя**, начиная с вызова `Model::userInput()` и заканчивая применением трансформации к 3D-модели. Используемые паттерны — **Фабричный метод** и **Команда**.

---

## 1. Пользователь вызывает команду

```cpp
model.userInput(UserAction_t::MoveModel_X, 10.0f);
```

## 2. `Model::userInput()`

```cpp
void Model::userInput(UserAction_t action, float value) {
  auto command = command_factory_.createCommand(action, value);
  if (command) {
    history_.ExecuteAndStore(std::move(command), *this);
  }
}
```

- `command_factory_` — объект типа `CommandFactory`, инициализируется в конструкторе `Model`.
- Вызывает `CommandFactory::createCommand()`.

---

## 3. `CommandFactory::createCommand()`

```cpp
std::unique_ptr<ICommand> CommandFactory::createCommand(UserAction_t action, float value) {
  auto it = creators_.find(action);
  if (it != creators_.end()) {
    return it->second->create(value);
  }
  return nullptr;
}
```

- Словарь `creators_` содержит соответствия между `UserAction_t` и `CommandCreator`.
- Если `action == MoveModel_X`, будет использован `MoveXCreator`.

---

## 4. Конкретный `CommandCreator`

```cpp
std::unique_ptr<ICommand> MoveXCreator::create(float value) {
  return std::make_unique<MoveXCommand>(value);
}
```

- Здесь используется **фабричный метод** — создаётся конкретный `MoveXCommand`.
- `MoveXCommand` реализует интерфейс `ICommand` и инкапсулирует значение `dx_`.

---

## 5. Конструктор `MoveXCommand`

```cpp
MoveXCommand::MoveXCommand(float dx) : dx_(dx) {}
```

- Сохраняет значение `dx_` для дальнейшего применения и отмены.

---

## 6. `CommandHistory::ExecuteAndStore()`

```cpp
void CommandHistory::ExecuteAndStore(std::unique_ptr<ICommand> command, Model& model) {
  if (command) {
    command->Execute(model);
    undo_stack_.push(std::move(command));
    while (!redo_stack_.empty()) redo_stack_.pop();
  }
}
```

- Вызывает `command->Execute(model)`.
- Помещает команду в `undo_stack_`.
- Очищает `redo_stack_` (начинается новая ветка изменений).

---

## 7. Вызов `MoveXCommand::Execute()`

```cpp
void MoveXCommand::Execute(Model& model) {
  model.get_transformer().Move(model.get_vertices(), dx_, 0.0f, 0.0f);
}
```

- `model.get_transformer()` возвращает ссылку на `AffineTransformer`.
- `model.get_vertices()` возвращает `std::vector<Point3D>&`.

---

## 8. Вызов `AffineTransformer::Move()`

```cpp
void AffineTransformer::Move(std::vector<Point3D>& points, float dx, float dy, float dz) {
  init_move_matrix(dx, dy, dz);
  for (auto& p : points) apply_matrix(p, move_matrix_);
}
```

- Создаётся матрица перемещения.
- Каждая вершина умножается на эту матрицу с помощью `apply_matrix()`.

---

## 9. Преобразование завершено

- Модель на сцене смещена.
- Команда находится в истории `undo_stack_`.

---

## 10. Возможность отката и повтора

### Откат (`Model::undo()` → `CommandHistory::Undo()`):

```cpp
command->Undo(model); // вызывает MoveXCommand::Undo()
```

```cpp
void MoveXCommand::Undo(Model& model) {
  model.get_transformer().Move(model.get_vertices(), -dx_, 0.0f, 0.0f);
}
```

- Инвертирует трансформацию.

### Повтор (`Model::redo()` → `CommandHistory::Redo()`):

```cpp
command->Execute(model);
```

---

## Используемые паттерны

- **Command (Команда):**
  - Интерфейс `ICommand` с методами `Execute` и `Undo`.
  - Конкретные реализации: `MoveXCommand`, `RotateYCommand`, `ScaleCommand` и т.д.
  - Поддержка Undo/Redo через `CommandHistory`.

- **Factory Method (Фабричный метод):**
  - Интерфейс `CommandCreator` с методом `create(value)`.
  - Конкретные фабрики: `MoveXCreator`, `RotateYCreator` и т.д.
  - `CommandFactory` хранит и вызывает нужного `Creator`.

---

## Заключение

Такой пайплайн:
- изолирует команды от модели;
- делает поведение расширяемым без изменения `Model`;
- даёт гибкий контроль (Undo/Redo);
- следует принципам SOLID (особенно OCP и SRP).
