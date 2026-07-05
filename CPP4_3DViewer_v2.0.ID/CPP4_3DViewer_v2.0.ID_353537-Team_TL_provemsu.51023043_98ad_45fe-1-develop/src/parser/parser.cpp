#include "parser.h"

#include <cctype>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "../scope_timer/scope_timer.h"

using namespace s21;

namespace {

/// \brief Парсит до трёх float из C-строки формата "x y z".
/// \return Количество успешно разобранных чисел (0..3).
static int parse_floats3(const char** s, float& x, float& y, float& z) {
  int count = 0;
  char* e = nullptr;
  while (**s && std::isspace(static_cast<unsigned char>(**s))) ++(*s);

  x = std::strtof(*s, &e);
  if (e != *s) {
    ++count;
    *s = e;
  } else {
    return 0;
  }

  while (**s && std::isspace(static_cast<unsigned char>(**s))) ++(*s);
  y = std::strtof(*s, &e);
  if (e != *s) {
    ++count;
    *s = e;
  } else {
    return count;
  }

  while (**s && std::isspace(static_cast<unsigned char>(**s))) ++(*s);
  z = std::strtof(*s, &e);
  if (e != *s) {
    ++count;
    *s = e;
  }
  return count;
}

/// \brief Разбирает токен вершины грани: "v", "v/vt", "v//vn", "v/vt/vn".
/// \return true, если число индекса вершины присутствует; иначе false.
static bool parse_face_triplet(const std::string& token, int& v, int& vt,
                               int& vn) {
  v = vt = vn = -1;
  const char* s = token.c_str();
  char* e = nullptr;

  long t = std::strtol(s, &e, 10);
  if (e == s) return false;
  v = static_cast<int>(t);

  if (*e == '/') {
    const char* p = e + 1;
    if (*p != '/') {  // a/b[/c]
      long t2 = std::strtol(p, &e, 10);
      if (e != p) vt = static_cast<int>(t2);
      if (*e == '/') {  // a/b/c
        p = e + 1;
        if (*p) {
          long t3 = std::strtol(p, &e, 10);
          if (e != p) vn = static_cast<int>(t3);
        }
      }
    } else {  // a//c
      p = e + 1;
      if (*p == '/') ++p;
      if (*p) {
        long t3 = std::strtol(p, &e, 10);
        if (e != p) vn = static_cast<int>(t3);
      }
    }
  }
  return true;
}

}  // namespace

std::ofstream Parser::logFile_;

Parser::Parser() {
  openLogFile();
  logger("Parser initialised", true);
}

Parser::~Parser() {
  logger("Parser class was deleted!", true);
  closeLogFile();
}

bool Parser::parseObjString(const std::string& data, ObjModel& model,
                            bool log) {
  SCOPE_TIMER("Parser::parseObjString");
  static bool isInitialized = false;
  if (!isInitialized) {
    openLogFile();
    isInitialized = true;
  }

  model.clear(!model.materials.isEmpty());
  model.vertices.reserve(1000000);
  model.texCoords.reserve(1000000);
  model.normals.reserve(1000000);
  model.faces.reserve(500000);
  model.edges.reserve(500000);
  model.globalUniqueEdges.reserve(500000);

  std::istringstream iss(data);
  std::string line;
  size_t lineNumber = 0;

  while (std::getline(iss, line)) {
    ++lineNumber;
    if (line.empty() || line.rfind("#", 0) == 0) continue;
    removedSharpComments(line, log);

    if (line.rfind("v ", 0) == 0)
      parseVLine(line, model, log);
    else if (line.rfind("vt", 0) == 0)
      parseVTLine(line, model, log);
    else if (line.rfind("vn", 0) == 0)
      parseVNLine(line, model, log);
    else if (line.rfind("f ", 0) == 0)
      parseFLine(line, model, log);
    else if (line.rfind("o ", 0) == 0)
      parseOLine(line, model, log);
    else if (line.rfind("mt", 0) == 0)
      parseMtllibLine(line, model, log);
    else if (line.rfind("g ", 0) == 0)
      parseGLine(line, model, log);
    else if (line.rfind("us", 0) == 0)
      parseUsemtlLine(line, model, log);
    else if (line.rfind("s ", 0) == 0)
      parseSLine(line, model, log);
    else if (log)
      logger(std::string("Unknown/ignored line: ") + line, log);
  }

  if (model.getTotalVertices() > model.getMaxVertices()) {
    if (log)
      logger("ERROR! You tried to upload file bigger than 10^6 vertices!!!",
             log);
    return false;
  }

  if (log) {
    logger("Parsing completed. Lines: " + std::to_string(lineNumber) +
               ", Faces: " + std::to_string(model.faces.size()),
           log);
  }
  return !model.faces.isEmpty();
}

void Parser::removedSharpComments(std::string& line, bool log) {
  size_t sharp_pos = line.find('#');
  if (sharp_pos != std::string::npos) {
    line.resize(sharp_pos);
    logger("Removed sharp comment at position " + std::to_string(sharp_pos) +
               " from line: " + line.substr(0, sharp_pos),
           log);
  }
}

void Parser::parseVLine(const std::string& line, ObjModel& model, bool log) {
  const char* s = line.c_str() + 2;
  float x = 0.0F, y = 0.0F, z = 0.0F;
  if (parse_floats3(&s, x, y, z) == 3) {
    model.vertices.append(QVector3D(x, y, z));
    if (log) logger("Vertex added: " + line, log);
  } else {
    if (log) logger("Failed to parse vertex: " + line, log);
  }
}

void Parser::parseVTLine(const std::string& line, ObjModel& model, bool log) {
  const char* s = line.c_str() + 2;
  float u = 0.0F, v = 0.0F, dummy = 0.0F;
  int n = parse_floats3(&s, u, v, dummy);
  if (n >= 2) {
    model.texCoords.append(QVector2D(u, v));
    if (log) logger("TexCoord added: " + line, log);
  } else {
    model.texCoords.append(QVector2D(0.0F, 0.0F));
    if (log) logger("Failed to parse vt; inserted (0,0): " + line, log);
  }
}

void Parser::parseVNLine(const std::string& line, ObjModel& model, bool log) {
  const char* s = line.c_str() + 2;
  float x = 0.0F, y = 0.0F, z = 0.0F;
  if (parse_floats3(&s, x, y, z) >= 3) {
    model.normals.append(QVector3D(x, y, z));
    if (log) logger("Normal added: " + line, log);
  } else {
    model.normals.append(QVector3D(0.0F, 0.0F, 1.0F));
    if (log) logger("Failed to parse normal; inserted (0,0,1): " + line, log);
  }
}

void Parser::parseFLine(const std::string& line, ObjModel& model, bool log) {
  std::string data = line.substr(2);
  std::istringstream iss(data);
  std::string token;

  std::vector<size_t> vIdx, vtIdx, vnIdx;
  vIdx.reserve(4);
  vtIdx.reserve(4);
  vnIdx.reserve(4);

  Face face;
  face.material = model.currentMaterial;

  size_t facePoints = 0;
  while (iss >> token) {
    int v = -1, vt = -1, vn = -1;
    if (!parse_face_triplet(token, v, vt, vn)) continue;
    if (v > 0) {
      size_t v0 = static_cast<size_t>(v - 1);
      if (v0 < static_cast<size_t>(model.vertices.size())) {
        vIdx.push_back(v0);
        if (vt > 0 && static_cast<size_t>(vt - 1) <
                          static_cast<size_t>(model.texCoords.size()))
          vtIdx.push_back(static_cast<size_t>(vt - 1));
        if (vn > 0 && static_cast<size_t>(vn - 1) <
                          static_cast<size_t>(model.normals.size()))
          vnIdx.push_back(static_cast<size_t>(vn - 1));
        ++facePoints;
      }
    }
  }

  if (facePoints >= 3) {
    model.originalFaceCount++;

    for (size_t i = 0; i < vIdx.size(); ++i) {
      size_t a = vIdx[i];
      size_t b = vIdx[(i + 1) % vIdx.size()];
      QPair<size_t, size_t> edge = qMakePair(qMin(a, b), qMax(a, b));
      if (!model.globalUniqueEdges.contains(edge)) {
        model.edges.append(Edge(a, b));
        model.globalUniqueEdges.insert(edge);
      }
    }

    if (facePoints == 3) {
      face.vertexIndex[0] = vIdx[0];
      face.vertexIndex[1] = vIdx[1];
      face.vertexIndex[2] = vIdx[2];
      if (!vtIdx.empty() && vtIdx.size() >= 3) {
        face.texCoordIndex[0] = vtIdx[0];
        face.texCoordIndex[1] = vtIdx[1];
        face.texCoordIndex[2] = vtIdx[2];
      }
      if (!vnIdx.empty() && vnIdx.size() >= 3) {
        face.normalIndex[0] = vnIdx[0];
        face.normalIndex[1] = vnIdx[1];
        face.normalIndex[2] = vnIdx[2];
      }
      model.faces.append(face);
      if (log) logger("Face added (triangle): " + line, log);
    } else {
      for (size_t i = 1; i + 1 < vIdx.size(); ++i) {
        Face tri;
        tri.material = model.currentMaterial;
        tri.vertexIndex[0] = vIdx[0];
        tri.vertexIndex[1] = vIdx[i];
        tri.vertexIndex[2] = vIdx[i + 1];
        if (!vtIdx.empty() && vtIdx.size() == vIdx.size()) {
          tri.texCoordIndex[0] = vtIdx[0];
          tri.texCoordIndex[1] = vtIdx[i];
          tri.texCoordIndex[2] = vtIdx[i + 1];
        }
        if (!vnIdx.empty() && vnIdx.size() == vIdx.size()) {
          tri.normalIndex[0] = vnIdx[0];
          tri.normalIndex[1] = vnIdx[i];
          tri.normalIndex[2] = vnIdx[i + 1];
        }
        model.faces.append(tri);
      }
      if (log) logger("Face added (triangulated): " + line, log);
    }
  } else {
    if (log) logger("Failed to parse face: " + line, log);
  }
}

void Parser::parseOLine(const std::string& line, ObjModel& model, bool log) {
  std::string data = line.substr(2);
  model.modelName = QString::fromStdString(data).trimmed();
  if (log) logger("Object name set: " + data, log);
}

void Parser::parseMtllibLine(const std::string& line, ObjModel& model,
                             bool log) {
  std::string data = line.substr(7);
  model.mtdlib_file = QString::fromStdString(data).trimmed();
  if (log) logger("MTL library set: " + data, log);
}

void Parser::parseGLine(const std::string& line, ObjModel& model, bool log) {
  std::string data = line.substr(2);
  model.group = QString::fromStdString(data).trimmed();
  if (log) logger("Group set: " + data, log);
}

void Parser::parseUsemtlLine(const std::string& line, ObjModel& model,
                             bool log) {
  std::string data = line.substr(7);
  model.currentMaterial = QString::fromStdString(data).trimmed();
  if (log) logger("Material set: " + data, log);
}

void Parser::parseSLine(const std::string& line, ObjModel& model, bool log) {
  std::string data = line.substr(2);
  std::istringstream iss(data);
  float value;
  if (iss >> value) {
    model.smoothingValue = value;
  } else {
    model.smoothingValue = 0.0f;
  }
  model.smoothing = QString::fromStdString(data).trimmed();
  if (log) logger("Smoothing set: " + data, log);
}

void Parser::openLogFile() {
  if (!logFile_.is_open()) {
    logFile_.open(logFileName_, std::ios::app);
    if (logFile_.is_open()) {
      logger("Log file opened successfully", true);
    } else {
      std::cerr << "Failed to open log file: " << logFileName_ << std::endl;
    }
  }
}

void Parser::closeLogFile() {
  if (logFile_.is_open()) {
    logger("Log file closed", true);
    logFile_.close();
  }
}

void Parser::logger(const std::string& message, bool logEnabled) {
  if (logEnabled && logFile_.is_open()) {
    auto now = std::chrono::system_clock::now();
    auto nowTime = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&nowTime), "%Y-%m-%d %H:%M:%S");
    logFile_ << "[" << ss.str() << "] " << message << std::endl;
    logFile_.flush();
  }
}