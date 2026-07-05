#include "objloader.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QStringList>
#include <QTextStream>
#include <QtConcurrent/QtConcurrent>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "../parser/parser.h"

using namespace s21;

std::ofstream ObjLoader::mtl_logFile_;

// ---------------- ObjModel ----------------

void ObjModel::clear(bool preserveMaterials) {
  vertices.clear();
  texCoords.clear();
  normals.clear();
  faces.clear();
  globalUniqueEdges.clear();
  renderVertices.clear();
  renderIndices.clear();
  renderEdges.clear();
  modelName.clear();
  originalFaceCount = 0;
  if (!preserveMaterials) {
    materials.clear();
    filePath.clear();
  }
  currentMaterial = "";
}

void ObjModel::prepareRenderData() {
  SCOPE_TIMER("ObjModel::prepareRenderData");
  renderVertices.clear();
  renderIndices.clear();
  renderEdges.clear();

  const int faceCount = faces.size();
  if (faceCount <= 0) return;

  // 1) кэш цветов граней
  QVector<QVector3D> faceColors;
  faceColors.reserve(faceCount);
  for (int fi = 0; fi < faceCount; ++fi) {
    const QString& name = faces[fi].material;
    QVector3D col(0.8f, 0.8f, 0.8f);
    if (!name.isEmpty()) {
      auto it = materials.constFind(name);
      if (it != materials.cend()) {
        const Material& m = it.value();
        col =
            QVector3D(m.diffuse.redF(), m.diffuse.greenF(), m.diffuse.blueF());
      }
    }
    faceColors.push_back(col);
  }

  // 2) «снимки» исходных массивов
  const Face* facesPtr = faces.constData();
  const QVector3D* vertsPtr = vertices.constData();
  const QVector2D* uvsPtr = texCoords.constData();
  const QVector3D* normsPtr = normals.constData();
  const QVector3D* fcolPtr = faceColors.constData();

  const int vertsCount = vertices.size();
  const int uvsCount = texCoords.size();
  const int normsCount = normals.size();

  // 3) выходные буферы
  static constexpr int floatsPerVertex = 14;
  static constexpr int vertsPerFace = 3;

  renderVertices.resize(faceCount * vertsPerFace * floatsPerVertex);
  renderIndices.resize(faceCount * vertsPerFace);
  renderEdges.resize(faceCount * 6);

  float* vData = renderVertices.data();
  unsigned int* iData = renderIndices.data();
  unsigned int* eData = renderEdges.data();

  static const float kBary[3][3] = {
      {1.f, 0.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 0.f, 1.f}};

  // 4) индексируем фейсы
  QVector<int> idx(faceCount);
  for (int i = 0; i < faceCount; ++i) idx[i] = i;

  QtConcurrent::blockingMap(idx, [=](int fi) {
    const Face& f = facesPtr[fi];

    QVector3D nrm(0, 0, 1);
    const int vi0 = f.vertexIndex[0], vi1 = f.vertexIndex[1],
              vi2 = f.vertexIndex[2];

    const bool vertsOk = (unsigned)vi0 < (unsigned)vertsCount &&
                         (unsigned)vi1 < (unsigned)vertsCount &&
                         (unsigned)vi2 < (unsigned)vertsCount;

    const bool haveAllNormals =
        (unsigned)f.normalIndex[0] < (unsigned)normsCount &&
        (unsigned)f.normalIndex[1] < (unsigned)normsCount &&
        (unsigned)f.normalIndex[2] < (unsigned)normsCount;

    if (!haveAllNormals && vertsOk) {
      const QVector3D& v0 = vertsPtr[vi0];
      const QVector3D& v1 = vertsPtr[vi1];
      const QVector3D& v2 = vertsPtr[vi2];
      QVector3D nn = QVector3D::crossProduct(v1 - v0, v2 - v0);
      if (nn.lengthSquared() > 0.0f) nrm = nn.normalized();
    }

    const QVector3D mcol = fcolPtr[fi];

    const int baseF = fi * 3 * floatsPerVertex;
    const int baseI = fi * 3;
    const int baseE = fi * 6;

    for (int j = 0; j < 3; ++j) {
      const int vi = f.vertexIndex[j];
      const int ti = f.texCoordIndex[j];
      const int ni = f.normalIndex[j];

      const QVector3D pos = ((unsigned)vi < (unsigned)vertsCount)
                                ? vertsPtr[vi]
                                : QVector3D(0, 0, 0);
      const QVector2D tex =
          ((unsigned)ti < (unsigned)uvsCount) ? uvsPtr[ti] : QVector2D(0, 0);
      const QVector3D nor =
          ((unsigned)ni < (unsigned)normsCount) ? normsPtr[ni] : nrm;

      float* dst = vData + baseF + j * floatsPerVertex;
      dst[0] = pos.x();
      dst[1] = pos.y();
      dst[2] = pos.z();
      dst[3] = tex.x();
      dst[4] = tex.y();
      dst[5] = nor.x();
      dst[6] = nor.y();
      dst[7] = nor.z();
      dst[8] = mcol.x();
      dst[9] = mcol.y();
      dst[10] = mcol.z();
      dst[11] = kBary[j][0];
      dst[12] = kBary[j][1];
      dst[13] = kBary[j][2];

      iData[baseI + j] = baseI + j;
    }

    eData[baseE + 0] = baseI + 0;
    eData[baseE + 1] = baseI + 1;
    eData[baseE + 2] = baseI + 1;
    eData[baseE + 3] = baseI + 2;
    eData[baseE + 4] = baseI + 2;
    eData[baseE + 5] = baseI + 0;
  });
}

QVector3D ObjModel::calculateBoundingBox() const {
  SCOPE_TIMER("ObjModel::calculateBoundingBox");
  if (vertices.isEmpty()) return QVector3D(1, 1, 1);

  QVector3D min = vertices[0];
  QVector3D max = vertices[0];

  for (const QVector3D& vertex : vertices) {
    min.setX(qMin(min.x(), vertex.x()));
    min.setY(qMin(min.y(), vertex.y()));
    min.setZ(qMin(min.z(), vertex.z()));

    max.setX(qMax(max.x(), vertex.x()));
    max.setY(qMax(max.y(), vertex.y()));
    max.setZ(qMax(max.z(), vertex.z()));
  }

  return max - min;
}

void ObjModel::centerModel() {
  SCOPE_TIMER("ObjModel::centerModel");
  if (vertices.isEmpty()) return;

  QVector3D min = vertices[0];
  QVector3D max = vertices[0];

  for (const QVector3D& vertex : vertices) {
    min.setX(qMin(min.x(), vertex.x()));
    min.setY(qMin(min.y(), vertex.y()));
    min.setZ(qMin(min.z(), vertex.z()));

    max.setX(qMax(max.x(), vertex.x()));
    max.setY(qMax(max.y(), vertex.y()));
    max.setZ(qMax(max.z(), vertex.z()));
  }

  QVector3D center = (min + max) / 2.0f;

  std::transform(vertices.begin(), vertices.end(), vertices.begin(),
                 [&center](QVector3D vertex) { return vertex - center; });
}

void ObjModel::scaleModel(float scale) {
  SCOPE_TIMER("ObjModel::scaleModel");
  std::transform(vertices.begin(), vertices.end(), vertices.begin(),
                 [scale](QVector3D vertex) { return vertex * scale; });
}

// ---------------- ObjLoader ----------------

ObjLoader::ObjLoader() {
  openMtlLogFile();
  mtlLogger("ObjLoader initialised", true);
}

ObjLoader::~ObjLoader() {
  mtlLogger("ObjLoader class was deleted!", true);
  closeMtlLogFile();
}

bool ObjLoader::loadFromFile(const QString& filename, ObjModel& model) {
  SCOPE_TIMER("ObjLoader::loadFromFile");
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return false;
  }

  QTextStream in(&file);
  QString content = in.readAll();
  file.close();

  model.clear();

  QFileInfo objFileInfo(filename);
  QString objDir = objFileInfo.absolutePath();
  QString mtlFilename = "";
  QStringList lines = content.split('\n');
  auto it = std::find_if(lines.begin(), lines.end(), [](const QString& line) {
    return line.trimmed().startsWith("mtllib ");
  });
  if (it != lines.end()) {
    mtlFilename = it->split(QRegularExpression("\\s+"), Qt::SkipEmptyParts)[1];
  }
  if (!mtlFilename.isEmpty()) {
    QString mtlPath = objDir + "/" + mtlFilename;
    loadMtlFile(mtlPath, model);
  }

  return parseObjData(content, model);
}

bool ObjLoader::loadFromString(const QString& objData, ObjModel& model) {
  model.clear();
  return parseObjData(objData, model);
}

QVector<int> ObjLoader::parseFaceVertex(const QString& vertexStr) {
  QVector<int> result;
  QStringList parts = vertexStr.split('/');

  for (const QString& part : parts) {
    if (part.isEmpty()) {
      result.append(-1);
    } else {
      bool ok;
      int index = part.toInt(&ok);
      result.append(ok ? index : -1);
    }
  }

  return result;
}

bool ObjLoader::parseObjData(const QString& data, ObjModel& model, bool log) {
  Parser parser;
  return parser.parseObjString(data.toStdString(), model, log);
}

void ObjLoader::loadMtlFile(const QString& mtlPath, ObjModel& model) {
  QFile mtlFile(mtlPath);
  if (mtlFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream mtlIn(&mtlFile);
    QString mtlContent = mtlIn.readAll();
    mtlFile.close();
    parseMtlFile(mtlContent, model);
  }
}

void ObjLoader::parseMtlFile(const QString& mtlData, ObjModel& model,
                             bool log) {
  SCOPE_TIMER("ObjLoader::parseMtlFile");
  static bool isInitialized = false;
  if (!isInitialized) {
    openMtlLogFile();
    isInitialized = true;
  }
  auto start = std::chrono::high_resolution_clock::now();
  QString currentMat = "";
  QStringList lines = mtlData.split('\n');
  mtlLogger("Parsing MTL file with " + std::to_string(lines.size()) + " lines",
            log);
  for (const QString& line : lines) {
    QString trimmedLine = line.trimmed();
    if (trimmedLine.isEmpty() || trimmedLine.startsWith('#')) continue;
    QStringList tokens =
        trimmedLine.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    if (tokens.isEmpty()) continue;
    if (tokens[0] == "newmtl") {
      if (tokens.size() > 1) {
        currentMat = tokens[1];
        model.materials[currentMat] = Material();
        mtlLogger("New material (newmtl) defined: " + currentMat.toStdString(),
                  log);
      }
    } else if (tokens[0] == "Ka" && !currentMat.isEmpty()) {
      if (tokens.size() >= 4) {
        bool ok1, ok2, ok3;
        float r = tokens[1].toFloat(&ok1);
        float g = tokens[2].toFloat(&ok2);
        float b = tokens[3].toFloat(&ok3);
        if (ok1 && ok2 && ok3) {
          model.materials[currentMat].ambient = QColor::fromRgbF(r, g, b);
          mtlLogger("Set ambient color (Ka) for " + currentMat.toStdString() +
                        " to: " + std::to_string(r) + " " + std::to_string(g) +
                        " " + std::to_string(b),
                    log);
        }
      }
    } else if (tokens[0] == "Kd" && !currentMat.isEmpty()) {
      if (tokens.size() >= 4) {
        bool ok1, ok2, ok3;
        float r = tokens[1].toFloat(&ok1);
        float g = tokens[2].toFloat(&ok2);
        float b = tokens[3].toFloat(&ok3);
        if (ok1 && ok2 && ok3) {
          model.materials[currentMat].diffuse = QColor::fromRgbF(r, g, b);
          mtlLogger("Set diffuse color (Kd) for " + currentMat.toStdString() +
                        " to: " + std::to_string(r) + " " + std::to_string(g) +
                        " " + std::to_string(b),
                    log);
        }
      }
    } else if (tokens[0] == "Ks" && !currentMat.isEmpty()) {
      if (tokens.size() >= 4) {
        bool ok1, ok2, ok3;
        float r = tokens[1].toFloat(&ok1);
        float g = tokens[2].toFloat(&ok2);
        float b = tokens[3].toFloat(&ok3);
        if (ok1 && ok2 && ok3) {
          model.materials[currentMat].specular = QColor::fromRgbF(r, g, b);
          mtlLogger("Set specular color (Ks) for " + currentMat.toStdString() +
                        " to: " + std::to_string(r) + " " + std::to_string(g) +
                        " " + std::to_string(b),
                    log);
        }
      }
    } else if (tokens[0] == "Ns" && !currentMat.isEmpty()) {
      if (tokens.size() >= 2) {
        bool ok;
        float ns = tokens[1].toFloat(&ok);
        if (ok) {
          model.materials[currentMat].specularExponent = ns;
          mtlLogger("Set specularExponent (Ns) for: " +
                        currentMat.toStdString() + " to: " + std::to_string(ns),
                    log);
        }
      }
    } else if (tokens[0] == "d" && !currentMat.isEmpty()) {
      if (tokens.size() >= 2) {
        bool ok;
        float d = tokens[1].toFloat(&ok);
        if (ok && d >= 0.0f && d <= 1.0f) {
          model.materials[currentMat].dissolve = d;
          mtlLogger("Set dissolve (d) for: " + currentMat.toStdString() +
                        " to: " + std::to_string(d),
                    log);
        }
      }
    } else if (tokens[0] == "illum" && !currentMat.isEmpty()) {
      if (tokens.size() >= 2) {
        bool ok;
        int illum = tokens[1].toInt(&ok);
        if (ok && illum >= 0 && illum <= 10) {
          model.materials[currentMat].illuminationModel = illum;
          mtlLogger("Set illumination model (illum) for: " +
                        currentMat.toStdString() +
                        " to: " + std::to_string(illum),
                    log);
        }
      }
    } else if (tokens[0] == "map_Kd" && !currentMat.isEmpty()) {
      if (tokens.size() > 1) {
        model.materials[currentMat].textureMap = tokens[1];
        mtlLogger("Set texture map (map_Kd) for: " + currentMat.toStdString() +
                      " to: " + tokens[1].toStdString(),
                  log);
      }
    }
  }
  QStringList materialKeys = model.materials.keys();
  QString materialsList = materialKeys.join(", ");
  mtlLogger(
      "MTL parsing completed. Materials loaded: " + materialsList.toStdString(),
      log);
  auto end = std::chrono::high_resolution_clock::now();
  double elapsed_time = std::chrono::duration<double>(end - start).count();
  mtlLogger("Parsing mtl completed. Time: " + std::to_string(elapsed_time) +
                " seconds.",
            log);
}

void ObjLoader::openMtlLogFile() {
  if (!mtl_logFile_.is_open()) {
    mtl_logFile_.open(mtl_logFileName_, std::ios::app);
    if (!mtl_logFile_.is_open()) {
      std::cerr << "Failed to open log file: " << mtl_logFileName_ << std::endl;
    } else {
      mtlLogger("Log file opened successfully", true);
    }
  }
}

void ObjLoader::closeMtlLogFile() {
  if (mtl_logFile_.is_open()) {
    mtlLogger("Log file closed", true);
    mtl_logFile_.close();
  }
}

void ObjLoader::mtlLogger(const std::string& message, bool logEnabled) {
  if (logEnabled && mtl_logFile_.is_open()) {
    auto now = std::chrono::system_clock::now();
    auto nowTime = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&nowTime), "%Y-%m-%d %H:%M:%S");
    mtl_logFile_ << "[" << ss.str() << "] " << message << std::endl;
    mtl_logFile_.flush();
  }
}

// ---------------- GeometryAnalyzer ----------------

QVector<GeometryAnalyzer::TrueFace> GeometryAnalyzer::analyzeFaces(
    const ObjModel& model) {
  SCOPE_TIMER("GeometryAnalyzer::analyzeFaces");

  const int F = model.faces.size();
  QVector<TrueFace> trueFaces;
  trueFaces.reserve(F);
  if (F == 0 || model.vertices.isEmpty()) return trueFaces;

  auto getV = [&](int idx) -> const QVector3D& { return model.vertices[idx]; };

  auto edgeKey = [](int a, int b) -> quint64 {
    if (a > b) std::swap(a, b);
    return (quint64(quint32(a)) << 32) | quint64(quint32(b));
  };

  QVector<QVector3D> faceNormals;
  faceNormals.reserve(F);
  faceNormals.resize(F);
  for (int i = 0; i < F; ++i) {
    const Face& f = model.faces[i];
    QVector<QVector3D> tri;
    tri.reserve(3);
    tri.append(getV(f.vertexIndex[0]));
    tri.append(getV(f.vertexIndex[1]));
    tri.append(getV(f.vertexIndex[2]));
    faceNormals[i] = calculateFaceNormal(tri);
  }

  QHash<quint64, QVector<int>> edge2faces;
  edge2faces.reserve(F * 3);
  for (int i = 0; i < F; ++i) {
    const Face& f = model.faces[i];
    const int a = f.vertexIndex[0], b = f.vertexIndex[1], c = f.vertexIndex[2];
    edge2faces[edgeKey(a, b)].append(i);
    edge2faces[edgeKey(b, c)].append(i);
    edge2faces[edgeKey(c, a)].append(i);
  }

  QVector<uchar> processed(F, 0);

  constexpr float cosTol = 0.9998477f;  // cos(1°)

  for (int i = 0; i < F; ++i) {
    if (processed[i]) continue;

    const Face& f = model.faces[i];
    const int a = f.vertexIndex[0], b = f.vertexIndex[1], c = f.vertexIndex[2];

    TrueFace tf;
    tf.material = f.material;
    tf.vertices = {static_cast<size_t>(a), static_cast<size_t>(b),
                   static_cast<size_t>(c)};
    tf.normal = faceNormals[i];

    processed[i] = 1;

    auto tryMergeViaEdge = [&](int v0, int v1) -> int {
      const auto it = edge2faces.constFind(edgeKey(v0, v1));
      if (it == edge2faces.cend()) return -1;

      const QVector<int>& neigh = it.value();
      for (int j : neigh) {
        if (j == i || processed[j]) continue;

        const Face& g = model.faces[j];
        if (g.material != f.material) continue;

        const float d = QVector3D::dotProduct(faceNormals[i], faceNormals[j]);
        if (!std::isfinite(d) || d < cosTol) continue;

        const int six[6] = {
            a, b, c, g.vertexIndex[0], g.vertexIndex[1], g.vertexIndex[2]};
        int uniq[4];
        int count = 0;
        for (int t = 0; t < 6; ++t) {
          int v = six[t];
          bool seen = false;
          for (int u = 0; u < count; ++u) {
            if (uniq[u] == v) {
              seen = true;
              break;
            }
          }
          if (!seen) {
            if (count < 4)
              uniq[count++] = v;
            else {
              count = 5;  // LCOV_EXCL_LINE
              break;      // LCOV_EXCL_LINE
            }
          }
        }
        if (count != 4) continue;

        tf.vertices.clear();
        tf.vertices.reserve(4);
        tf.vertices.append(static_cast<size_t>(uniq[0]));
        tf.vertices.append(static_cast<size_t>(uniq[1]));
        tf.vertices.append(static_cast<size_t>(uniq[2]));
        tf.vertices.append(static_cast<size_t>(uniq[3]));
        processed[j] = 1;
        return j;
      }
      return -1;
    };

    int merged = -1;
    if ((merged = tryMergeViaEdge(a, b)) < 0 &&
        (merged = tryMergeViaEdge(b, c)) < 0 &&
        (merged = tryMergeViaEdge(c, a)) < 0) {
      // нет пары — остаётся треугольник
    } else {
      // объединён в quad
    }

    trueFaces.append(std::move(tf));
  }

  return trueFaces;
}

bool GeometryAnalyzer::areCoplanar(const QVector<QVector3D>& vertices,
                                   float tolerance) {
  if (vertices.size() < 4) return true;

  QVector3D v1 = vertices[1] - vertices[0];
  QVector3D v2 = vertices[2] - vertices[0];
  QVector3D normal = QVector3D::crossProduct(v1, v2).normalized();

  for (int i = 3; i < vertices.size(); ++i) {
    QVector3D v = vertices[i] - vertices[0];
    float distance = qAbs(QVector3D::dotProduct(v, normal));
    if (distance > tolerance) return false;
  }

  return true;
}

bool GeometryAnalyzer::shareEdge(const Face& face1, const Face& face2) {
  QSet<int> vertices1, vertices2;
  for (int i = 0; i < 3; ++i) {
    vertices1.insert(face1.vertexIndex[i]);
    vertices2.insert(face2.vertexIndex[i]);
  }
  QSet<int> intersection = vertices1.intersect(vertices2);
  return intersection.size() == 2;
}

QVector3D GeometryAnalyzer::calculateFaceNormal(
    const QVector<QVector3D>& vertices) {
  if (vertices.size() < 3) return QVector3D(0, 0, 1);
  QVector3D v1 = vertices[1] - vertices[0];
  QVector3D v2 = vertices[2] - vertices[0];
  return QVector3D::crossProduct(v1, v2).normalized();
}

int GeometryAnalyzer::getTrueEdgeCount(const QVector<TrueFace>& trueFaces) {
  SCOPE_TIMER("GeometryAnalyzer::getTrueEdgeCount");
  QSet<QPair<size_t, size_t>> uniqueEdges;

  for (const TrueFace& face : trueFaces) {
    for (int i = 0; i < face.vertices.size(); ++i) {
      size_t v1 = face.vertices[i];
      size_t v2 = face.vertices[(i + 1) % face.vertices.size()];
      QPair<size_t, size_t> edge = qMakePair(qMin(v1, v2), qMax(v1, v2));
      uniqueEdges.insert(edge);
    }
  }

  return uniqueEdges.size();
}
