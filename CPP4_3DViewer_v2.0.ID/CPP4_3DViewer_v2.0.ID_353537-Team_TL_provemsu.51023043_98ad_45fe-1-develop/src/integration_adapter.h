#ifndef INTEGRATION_ADAPTER_H
#define INTEGRATION_ADAPTER_H

/// \file
/// \brief Адаптер типов между Qt и внутренними структурами.

#include <QVector3D>
#include <QVector>
#include <algorithm>
#include <vector>

#include "geometry_types.h"  // Point3D

namespace s21 {

/// \brief Утилиты конвертации между QVector3D и Point3D,
///        а также между контейнерами Qt и std.
class TypeAdapter {
 public:
  TypeAdapter() = delete;

  /// \brief Конвертировать один QVector3D в Point3D (homo = 1).
  static Point3D qvectorToPoint3D(const QVector3D& v) {
    return {v.x(), v.y(), v.z(), 1.0f};
  }

  /// \brief Конвертировать один Point3D в QVector3D (игнорирует homo).
  static QVector3D point3DToQVector(const Point3D& p) {
    return QVector3D(p.x, p.y, p.z);
  }

  /// \brief Конвертировать массив Qt-вершин в std::vector<Point3D>.
  static std::vector<Point3D> convertVertices(const QVector<QVector3D>& src) {
    std::vector<Point3D> out;
    out.reserve(static_cast<size_t>(src.size()));
    // for (const auto& v : src) out.push_back(qvectorToPoint3D(v));
    std::transform(src.begin(), src.end(), std::back_inserter(out),
                   [](const QVector3D& v) { return qvectorToPoint3D(v); });
    return out;
  }

  /// \brief Конвертировать массив Point3D обратно в QVector<QVector3D>.
  static QVector<QVector3D> convertVerticesBack(
      const std::vector<Point3D>& src) {
    QVector<QVector3D> out;
    out.reserve(static_cast<int>(src.size()));
    for (const auto& p : src) out.append(point3DToQVector(p));
    return out;
  }
};

}  // namespace s21

#endif  // INTEGRATION_ADAPTER_H
