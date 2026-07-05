#ifndef CPP4_3DVIEWER_V2_0_OBJLOADER_OBJLOADER_H_
#define CPP4_3DVIEWER_V2_0_OBJLOADER_OBJLOADER_H_

/// \file
/// \brief Модель данных OBJ/MTL, загрузчик и утилиты геометрического анализа.
/// \details
///  - ObjModel хранит сырые данные (вершины, нормали, UV, грани, материалы) и
///  производные буферы для рендера.
///  - ObjLoader загружает OBJ/MTL из файла или строки, логирует MTL-парсинг.
///  - GeometryAnalyzer выполняет простые операции анализа (копланарность,
///  объединение по материалам, подсчёт рёбер).

#include <QColor>
#include <QMap>
#include <QPair>
#include <QSet>
#include <QString>
#include <QVector2D>
#include <QVector3D>
#include <QVector>
#include <string>

#include "../scope_timer/scope_timer.h"

namespace s21 {

/// \brief Вершина с позициями/UV/нормалью. Значения по умолчанию: tex=(0,0),
/// n=(0,0,1).
struct Vertex {
  QVector3D position;
  QVector2D texCoord;
  QVector3D normal;

  Vertex() : position(0, 0, 0), texCoord(0, 0), normal(0, 0, 1) {}
  explicit Vertex(const QVector3D& pos)
      : position(pos), texCoord(0, 0), normal(0, 0, 1) {}

  const QVector3D& getVertexPosition() const { return position; }
  const QVector2D& getVertexTexture() const { return texCoord; }
  const QVector3D& getVertexNormal() const { return normal; }
};

/// \brief Треугольная грань с индексами V/VT/VN и именем материала.
struct Face {
  int vertexIndex[3];
  int texCoordIndex[3];
  int normalIndex[3];
  QString material;

  Face() {
    for (int i = 0; i < 3; ++i) {
      vertexIndex[i] = -1;
      texCoordIndex[i] = -1;
      normalIndex[i] = -1;
    }
    material = "";
  }

  const QString& getFaceMaterial() const { return material; }
};

/// \brief Ненаправленное ребро (индексы вершин).
struct Edge {
  size_t p1_index;
  size_t p2_index;

  Edge()
      : p1_index(static_cast<size_t>(-1)), p2_index(static_cast<size_t>(-1)) {}
  Edge(size_t p1, size_t p2) : p1_index(p1), p2_index(p2) {}
};

/// \brief Материал из MTL (диффузный/окружающий/зеркальный цвет, карта и пр.).
struct Material {
  QColor diffuse;
  QColor ambient;
  QColor specular;
  float specularExponent;
  float dissolve;
  int illuminationModel;
  QString textureMap;

  Material()
      : diffuse(Qt::white),
        ambient(Qt::black),
        specular(Qt::white),
        specularExponent(0.0f),
        dissolve(1.0f),
        illuminationModel(2),
        textureMap("") {}

  const QColor& getDiffuse() const { return diffuse; }
  const QColor& getAmbient() const { return ambient; }
  const QColor& getSpecular() const { return specular; }
  const float& getSpecularExponent() const { return specularExponent; }
  const float& getDissolve() const { return dissolve; }
  const int& getIlluminationModel() const { return illuminationModel; }
  const QString& getTextureMap() const { return textureMap; }
};

/// \brief Модель OBJ: исходные данные и подготовленные буферы для рендера.
/// \details
///  - \c renderVertices содержит пачки по 14 float на каждую вершину
///  треугольника:
///    \c [x,y,z, u,v, nx,ny,nz, r,g,b, b0,b1,b2] (barycentric).
///  - \c renderIndices — индексы, согласованные с \c renderVertices (три на
///  грань).
///  - \c renderEdges — пары индексов для линий каркаса (по 6 на грань).
class ObjModel {
 public:
  static constexpr size_t max_obj_vertices_ = 1000000;

  // исходные данные
  QVector<QVector3D> vertices;
  QVector<QVector2D> texCoords;
  QVector<QVector3D> normals;
  QVector<Face> faces;
  QVector<Edge> edges;
  QString filePath;

  // данные для рендера
  QVector<float> renderVertices;
  QVector<unsigned int> renderIndices;
  QVector<unsigned int> renderEdges;

  // служебные поля
  QSet<QPair<size_t, size_t>> globalUniqueEdges;
  int originalFaceCount = 0;

  // метаданные/материалы
  QString modelName;
  QString smoothing;
  float smoothingValue{};
  QString group;
  QString mtdlib_file;
  QMap<QString, Material> materials;
  QString currentMaterial;

  // геттеры (публичный контракт)
  const QString& getModelName() const { return modelName; }
  const QString& getSmoothing() const { return smoothing; }
  const float& getSmoothingValue() const { return smoothingValue; }
  const QString& getGroup() const { return group; }
  const QString& getMtdlibFile() const { return mtdlib_file; }
  const QMap<QString, Material>& getMaterials() const { return materials; }
  const QString& getCurrentMaterial() const { return currentMaterial; }
  const QVector<QVector3D>& getObjVertices() const { return vertices; }
  const QVector<QVector2D>& getObjTextures() const { return texCoords; }
  const QVector<QVector3D>& getObjNormals() const { return normals; }
  const QVector<Face>& getObjFaces() const { return faces; }
  const QVector<Edge>& getObjEdges() const { return edges; }
  const QVector<float>& getRenderVertices() const { return renderVertices; }
  const QVector<unsigned int>& getRenderIndices() const {
    return renderIndices;
  }

  // агрегаты
  size_t getTotalVertices() const { return vertices.size(); }
  size_t getTotalTextures() const { return texCoords.size(); }
  size_t getTotalNormals() const { return normals.size(); }
  size_t getTotalFaces() const { return faces.size(); }
  size_t getTotalEdges() const { return edges.size(); }
  size_t getTotalRenderVertices() const { return renderVertices.size(); }
  size_t getTotalRenderIndices() const { return renderIndices.size(); }
  size_t getMaxVertices() const { return max_obj_vertices_; }

  /// \brief Полная очистка модели.
  /// \param preserveMaterials Сохранить ли таблицу материалов/путь к файлу.
  void clear(bool preserveMaterials = false);

  /// \brief Формирует массивы \c renderVertices/\c renderIndices/\c
  /// renderEdges.
  void prepareRenderData();

  /// \brief Размеры AABB (max-min).
  QVector3D calculateBoundingBox() const;

  /// \brief Центрирует модель в окне (сдвиг в минус центр AABB).
  void centerModel();

  /// \brief Масштабирует все вершины.
  void scaleModel(float scale);
};

/// \brief Загрузчик OBJ/MTL в ObjModel.
class ObjLoader {
 public:
  ObjLoader();
  ~ObjLoader();

  /// \brief Загружает из файла .obj (и, при наличии, .mtl).
  static bool loadFromFile(const QString& filename, ObjModel& model);

  /// \brief Загружает из строки с содержимым .obj.
  static bool loadFromString(const QString& objData, ObjModel& model);

  /// \brief Парсит вариант записи вершины грани: "v", "v/vt", "v//vn",
  /// "v/vt/vn". \return Массив индексов (от -1 при отсутствии/ошибке).
  static QVector<int> parseFaceVertex(const QString& vertexStr);

 private:
  // логирование MTL
  static std::ofstream mtl_logFile_;
  static constexpr const char* mtl_logFileName_ = "mtl_parser_logs.txt";

  static bool parseObjData(const QString& data, ObjModel& model,
                           bool log = true);
  static void loadMtlFile(const QString& mtlPath, ObjModel& model);
  static void parseMtlFile(const QString& mtlData, ObjModel& model,
                           bool log = true);
  static void openMtlLogFile();
  static void closeMtlLogFile();
  static void mtlLogger(const std::string& message, bool logEnabled);
};

/// \brief Простейший анализатор геометрии (объединение копланарных граней и
/// т.п.).
class GeometryAnalyzer {
 public:
  struct TrueFace {
    QVector<size_t> vertices;
    QString material;
    QVector3D normal;
  };

  /// \brief Возвращает укрупнённые (возможные quad) «истинные» грани, объединяя
  /// копланарные по материалу.
  static QVector<TrueFace> analyzeFaces(const ObjModel& model);

  /// \brief Считает число уникальных рёбер среди «истинных» граней.
  static int getTrueEdgeCount(const QVector<TrueFace>& trueFaces);

  /// \brief Проверяет, лежат ли точки в одной плоскости в пределах допуска.
  static bool areCoplanar(const QVector<QVector3D>& vertices,
                          float tolerance = 0.001f);

  /// \brief Делят ли две треугольные грани общее ребро.
  static bool shareEdge(const Face& face1, const Face& face2);

  /// \brief Нормаль треугольника по трём точкам (или (0,0,1), если точек < 3).
  static QVector3D calculateFaceNormal(const QVector<QVector3D>& vertices);
};

}  // namespace s21

#endif  // CPP4_3DVIEWER_V2_0_OBJLOADER_OBJLOADER_H_
