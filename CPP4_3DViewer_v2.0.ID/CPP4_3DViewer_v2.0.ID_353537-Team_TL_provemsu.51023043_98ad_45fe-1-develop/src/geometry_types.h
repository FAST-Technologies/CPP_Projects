#ifndef CPP4_3DVIEWER_V2_0_GEOMETRY_TYPES_H_
#define CPP4_3DVIEWER_V2_0_GEOMETRY_TYPES_H_

/// \file
/// \brief Базовые геометрические типы (Point3D/Point2D) без зависимостей от Qt.

struct Point3D {
  float x{0.0f};
  float y{0.0f};
  float z{0.0f};
  float homo{1.0f};

  Point3D operator-(const Point3D& rhs) const {
    return {x - rhs.x, y - rhs.y, z - rhs.z};
  }
  Point3D operator+(const Point3D& rhs) const {
    return {x + rhs.x, y + rhs.y, z + rhs.z};
  }
  Point3D operator*(const Point3D& rhs) const {
    return {x * rhs.x, y * rhs.y, z * rhs.z};
  }
  Point3D operator*(float s) const { return {x * s, y * s, z * s}; }
  Point3D operator/(const Point3D& rhs) const {
    return {x / rhs.x, y / rhs.y, z / rhs.z};
  }
  Point3D operator/(float s) const { return {x / s, y / s, z / s}; }
};

struct Point2D {
  float u{0.0f};
  float v{0.0f};
};

#endif  // CPP4_3DVIEWER_V2_0_GEOMETRY_TYPES_H_
