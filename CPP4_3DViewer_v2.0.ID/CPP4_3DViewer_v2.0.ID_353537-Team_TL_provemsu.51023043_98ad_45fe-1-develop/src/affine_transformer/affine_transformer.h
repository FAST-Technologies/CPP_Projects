#ifndef CPP4_3DVIEWER_V2_0_AFFINE_TRANSFORMER_AFFINE_TRANSFORMER_H_
#define CPP4_3DVIEWER_V2_0_AFFINE_TRANSFORMER_AFFINE_TRANSFORMER_H_

/// \file
/// \brief Аффинные преобразования модели: перенос, поворот, масштаб.

#include <QVector3D>
#include <QVector>
#include <vector>

#include "../geometry_types.h"  // Point3D
#include "../user_action.h"     // UserAction_t

namespace s21 {

class ObjModel;  // вперёд — чтобы не тянуть objloader.h в заголовок

/// \brief Выполняет аффинные преобразования над моделью и над произвольным
/// набором точек.
class AffineTransformer {
 private:
  float move_matrix_[4][4]{};
  float rotate_matrix_[4][4]{};
  float scale_matrix_[4][4]{};

  /// \brief Инициализация матрицы переноса.
  void init_move_matrix(float dx, float dy, float dz);
  /// \brief Инициализация матрицы поворота вокруг оси ('x'|'y'|'z'), угол — в
  /// градусах.
  void init_rotate_matrix(float angle_deg, char axis);
  /// \brief Инициализация матрицы масштабирования.
  void init_scale_matrix(float kx, float ky, float kz);

  /// \brief Применение матрицы к массиву вершин (QVector3D).
  void apply_matrix_to_vertices(QVector<QVector3D>& vertices,
                                float matrix[4][4]);

 public:
  /// \brief Применить преобразование к модели по действию пользователя.
  void applyTransformation(UserAction_t action, ObjModel& model,
                           float value = 0.0f);

  /// \name Операции над произвольным массивом точек (CPU-пайплайн команд)
  ///@{
  void Move(std::vector<Point3D>& points, float dx, float dy, float dz);
  void RotateX(std::vector<Point3D>& points, float angle_deg);
  void RotateY(std::vector<Point3D>& points, float angle_deg);
  void RotateZ(std::vector<Point3D>& points, float angle_deg);
  void Scale(std::vector<Point3D>& points, float kx, float ky, float kz);
  ///@}
};

}  // namespace s21

#endif  // CPP4_3DVIEWER_V2_0_AFFINE_TRANSFORMER_AFFINE_TRANSFORMER_H_
