// model.h
#ifndef CPP4_3DVIEWER_V2_0_MODEL_MODEL_H_
#define CPP4_3DVIEWER_V2_0_MODEL_MODEL_H_

/// \file
/// \brief Модель уровня приложения: хранит загруженную геометрию и применяет
/// аффинные преобразования. \details
///  - Паттерн Одиночка: единый экземпляр на процесс.\n
///  - Внутри держит \c ObjModel (сырые данные .obj/.mtl) и историю команд
///  (Undo/Redo).\n
///  - Трансформации выполняются через Command-паттерн и обновляют вершины
///  модели.

#include <QDebug>
#include <iostream>
#include <string>
#include <vector>

#include "../affine_transformer/affine_transformer.h"
#include "../command/command.h"
#include "../geometry_types.h"
#include "../integration_adapter.h"
#include "../objloader/objloader.h"
#include "../parser/parser.h"
#include "../scope_timer/scope_timer.h"
#include "../user_action.h"

namespace s21 {

/// \brief Хранилище данных 3D-модели уровня приложения и точка применения
/// аффинных преобразований. \details
///  - Загружает модель из файла через \c ObjLoader и хранит в \c ObjModel.\n
///  - Преобразования применяются к вершинам через командный интерфейс (\c
///  CommandHistory, \c CommandFactory).\n
///  - Обновление буферов для рендера инициируется вызовом \c
///  ObjModel::prepareRenderData().
/// \invariant Если \c objModel_.vertices пуст, трансформации не применяются.
/// \thread_safety Не потокобезопасен: предполагается использование из
/// GUI-потока.
class Model {
 private:
  static constexpr size_t max_vertices_ =
      1e6;  ///< Внутренний константный лимит (для справки; проверка
            ///< фактического лимита выполняется на стороне загрузки/парсинга).
  std::string filename_;  ///< Полный путь к исходному файлу модели.
  ObjModel objModel_;  ///< Сырые и производные данные модели (.obj/.mtl).
  AffineTransformer affineTransformer_;  ///< Выполнение аффинных операций.
  Parser parser_;  ///< Парсер .obj/.mtl (использование зависит от загрузчика).
  std::vector<Point3D>
      vertices_;  ///< Рабочее представление вершин под команды.
  CommandHistory history_;  ///< История команд для Undo/Redo.
  CommandFactory factory_;  ///< Фабрика команд по \c UserAction_t.

 public:
  /// \brief Создаёт модель и регистрирует доступные команды трансформаций.
  Model();

  /// \brief Доступ к единственному экземпляру (Singleton).
  static Model& get_model_instance();

  Model(const Model&) = delete;  ///< Нет копирования (Singleton).
  Model& operator=(const Model&) = delete;  ///< Нет присваивания (Singleton).

  ~Model();

  /// \brief Загружает модель из файла .obj.
  /// \param filepath Абсолютный/относительный путь к .obj.
  /// \note При пустом пути загрузка не выполняется.
  void LoadModel(const std::string& filepath);

  /// \brief Полная очистка текущей модели и истории команд.
  void clear_model();

  /// \brief Обработчик внешнего действия пользователя.
  /// \param action Тип действия (движение/поворот/масштаб).
  /// \param value Параметр действия (смещения, угол в градусах, коэффициент).
  void userInput(UserAction_t action, float value = 0.0f);

  /// \brief Применяет аффинную трансформацию через Command-паттерн.
  /// \return \c true при успешном создании и выполнении команды; иначе \c
  /// false.
  bool applyTransformation(UserAction_t action, float value = 0.0f);

  /// \brief Текущее состояние \c ObjModel (константный доступ).
  const ObjModel& getObjModel() const;

  /// \brief Заменяет \c ObjModel целиком.
  /// \details Пересобирает внутреннее представление \c vertices_ для команд.
  void setObjModel(const ObjModel& model);

  /// \brief Доступ к трансформеру (для сложных сценариев).
  AffineTransformer& getTransformer() { return affineTransformer_; }

  /// \brief Прямой доступ к вектору вершин \c ObjModel.
  /// \warning Меняя данные напрямую, не забывайте вызвать \c
  /// ObjModel::prepareRenderData().
  QVector<QVector3D>& getVertices() { return objModel_.vertices; }

  /// \brief Доступ к UV-координатам.
  const QVector<QVector2D>& getTextures() const { return objModel_.texCoords; }

  /// \brief Доступ к нормалям.
  const QVector<QVector3D>& getNormals() const { return objModel_.normals; }

  /// \brief Доступ к граням.
  const QVector<Face>& getFaces() const { return objModel_.faces; }

  /// \brief Доступ к рёбрам.
  const QVector<Edge>& getEdges() const { return objModel_.edges; }

  /// \brief Исходное имя файла.
  const std::string& getFilename() const { return filename_; }

  /// \name Агрегированные размеры
  ///@{
  size_t getTotalModelVertices() const { return objModel_.vertices.size(); }
  size_t getTotalModelTextures() const { return getTextures().size(); }
  size_t getTotalModelNormals() const { return getNormals().size(); }
  size_t getTotalModelFaces() const { return getFaces().size(); }
  size_t getTotalModelEdges() const { return getEdges().size(); }
  size_t getMaxModelVertices() const { return max_vertices_; }
  ///@}

  /// \brief Есть ли что откатывать.
  bool canUndo() const { return history_.CanUndo(); }

  /// \brief Есть ли что повторять.
  bool canRedo() const { return history_.CanRedo(); }

  /// \brief Откат последней команды.
  void undo();

  /// \brief Повтор отменённой команды.
  void redo();

  /// \brief Обновляет вершины модели внешним набором и пересобирает данные
  /// рендера. \param newVertices Новый массив вершин (в системе координат
  /// модели). \post \c objModel_.prepareRenderData() вызван.
  void updateVertices(const QVector<QVector3D>& newVertices);
};

}  // namespace s21

#endif  // CPP4_3DVIEWER_V2_0_MODEL_MODEL_H_
