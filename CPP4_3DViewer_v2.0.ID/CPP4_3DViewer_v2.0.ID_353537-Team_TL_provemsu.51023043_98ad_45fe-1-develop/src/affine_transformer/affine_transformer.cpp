#include "affine_transformer.h"

#include <cmath>

#include "../objloader/objloader.h"  // реализация ObjModel

namespace s21 {

namespace {
constexpr float kPi = 3.14159265358979323846f;
constexpr float kDegToRad = kPi / 180.0f;

void apply_matrix(Point3D& point, float matrix[4][4]) {
  float res[4] = {0.f, 0.f, 0.f, 0.f};
  const float vec[4] = {point.x, point.y, point.z, point.homo};
  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 4; ++j) res[i] += matrix[i][j] * vec[j];

  point.x = res[0];
  point.y = res[1];
  point.z = res[2];
  point.homo = res[3];
}

void reset_matrix(float matrix[4][4]) {
  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 4; ++j) matrix[i][j] = (i == j) ? 1.0f : 0.0f;
}

void convert_to_point3d(const QVector3D& v, Point3D& p) {
  p.x = v.x();
  p.y = v.y();
  p.z = v.z();
  p.homo = 1.0f;
}

QVector3D convert_to_qvector3d(const Point3D& p) {
  return QVector3D(p.x, p.y, p.z);
}
}  // namespace

void AffineTransformer::init_move_matrix(float dx, float dy, float dz) {
  reset_matrix(move_matrix_);
  move_matrix_[0][3] = dx;
  move_matrix_[1][3] = dy;
  move_matrix_[2][3] = dz;
}

void AffineTransformer::init_rotate_matrix(float angle_deg, char axis) {
  reset_matrix(rotate_matrix_);
  const float rad = angle_deg * kDegToRad;
  const float sinA = std::sin(rad);
  const float cosA = std::cos(rad);

  switch (axis) {
    case 'x':
      rotate_matrix_[1][1] = cosA;
      rotate_matrix_[1][2] = -sinA;
      rotate_matrix_[2][1] = sinA;
      rotate_matrix_[2][2] = cosA;
      break;
    case 'y':
      rotate_matrix_[0][0] = cosA;
      rotate_matrix_[0][2] = sinA;
      rotate_matrix_[2][0] = -sinA;
      rotate_matrix_[2][2] = cosA;
      break;
    case 'z':
      rotate_matrix_[0][0] = cosA;
      rotate_matrix_[0][1] = -sinA;
      rotate_matrix_[1][0] = sinA;
      rotate_matrix_[1][1] = cosA;
      break;
    default:
      break;
  }
}

void AffineTransformer::init_scale_matrix(float kx, float ky, float kz) {
  reset_matrix(scale_matrix_);
  scale_matrix_[0][0] = kx;
  scale_matrix_[1][1] = ky;
  scale_matrix_[2][2] = kz;
}

void AffineTransformer::apply_matrix_to_vertices(QVector<QVector3D>& vertices,
                                                 float matrix[4][4]) {
  for (QVector3D& v : vertices) {
    Point3D p{};
    convert_to_point3d(v, p);
    apply_matrix(p, matrix);
    v = convert_to_qvector3d(p);
  }
}

void AffineTransformer::applyTransformation(UserAction_t action,
                                            ObjModel& model, float value) {
  switch (action) {
    case UserAction_t::MoveModel_X:
      init_move_matrix(value, 0.0f, 0.0f);
      apply_matrix_to_vertices(model.vertices, move_matrix_);
      break;
    case UserAction_t::MoveModel_Y:
      init_move_matrix(0.0f, value, 0.0f);
      apply_matrix_to_vertices(model.vertices, move_matrix_);
      break;
    case UserAction_t::MoveModel_Z:
      init_move_matrix(0.0f, 0.0f, value);
      apply_matrix_to_vertices(model.vertices, move_matrix_);
      break;
    case UserAction_t::RotateModel_X:
      init_rotate_matrix(value, 'x');
      apply_matrix_to_vertices(model.vertices, rotate_matrix_);
      break;
    case UserAction_t::RotateModel_Y:
      init_rotate_matrix(value, 'y');
      apply_matrix_to_vertices(model.vertices, rotate_matrix_);
      break;
    case UserAction_t::RotateModel_Z:
      init_rotate_matrix(value, 'z');
      apply_matrix_to_vertices(model.vertices, rotate_matrix_);
      break;
    case UserAction_t::ScaleModel:
      init_scale_matrix(value, value, value);
      apply_matrix_to_vertices(model.vertices, scale_matrix_);
      break;
    default:
      break;
  }
  model.prepareRenderData();
}

void AffineTransformer::Move(std::vector<Point3D>& points, float dx, float dy,
                             float dz) {
  init_move_matrix(dx, dy, dz);
  for (auto& p : points) apply_matrix(p, move_matrix_);
}

void AffineTransformer::RotateX(std::vector<Point3D>& points, float angle_deg) {
  init_rotate_matrix(angle_deg, 'x');
  for (auto& p : points) apply_matrix(p, rotate_matrix_);
}

void AffineTransformer::RotateY(std::vector<Point3D>& points, float angle_deg) {
  init_rotate_matrix(angle_deg, 'y');
  for (auto& p : points) apply_matrix(p, rotate_matrix_);
}

void AffineTransformer::RotateZ(std::vector<Point3D>& points, float angle_deg) {
  init_rotate_matrix(angle_deg, 'z');
  for (auto& p : points) apply_matrix(p, rotate_matrix_);
}

void AffineTransformer::Scale(std::vector<Point3D>& points, float kx, float ky,
                              float kz) {
  init_scale_matrix(kx, ky, kz);
  for (auto& p : points) apply_matrix(p, scale_matrix_);
}

}  // namespace s21
