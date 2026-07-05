#include "glwidget.h"

#include <QDateTime>
#include <QFont>
#include <QPair>
#include <QVector2D>
#include <QtGlobal>

#include "../settings_store/settings_store.h"
#include "../view/view.h"

using namespace s21;

static inline QImage flipVertical(const QImage& img) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 9, 0)
  return img.flipped(Qt::Vertical);
#else
  return img.mirrored(false, /*vertical=*/true);
#endif
}

// qHash для использования QVector3D в QSet
uint qHash(const QVector3D& key, uint seed = 0) {
  return qHash(qHash(key.x()) ^ qHash(key.y()) ^ qHash(key.z()), seed);
}

void GLWidget::applySettings(const s21::UiSettings& u) {
  setBackgroundColor(u.background);
  setEdgeColor(u.edgeColor);
  setVertexColor(u.vertexColor);
  setEdgeThickness(u.edgeThickness);
  setVertexSize(u.vertexSize);
  setLineType(static_cast<LineType>(u.lineType));
  setProjectionType(static_cast<ProjectionType>(u.projectionType));
  setVertexDisplayType(static_cast<VertexDisplayType>(u.vertexDisplayType));
  setWireframeMode(u.wireframe);
  rotationSpeed_ = u.rotationSpeed;
}

GLWidget::GLWidget(QWidget* parent)
    : QOpenGLWidget(parent),
      facade_(nullptr),
      vbo(QOpenGLBuffer::VertexBuffer),
      ibo(QOpenGLBuffer::IndexBuffer),
      shaderProgram(nullptr),
      vao(),
      texture(nullptr),
      modelLoaded_(false),
      hasMaterials_(false),
      currentModelPath_(""),
      rotationAngle_(0.0f),
      rotationSpeed_(1.0f),
      cameraDistance_(5.0f),
      edgeThickness_(1.0f),
      vertexSize_(5.0f),
      cameraRotation_(0, 0, 0),
      mousePressed_(false),
      backgroundColor_(1.0f, 1.0f, 1.0f, 1.0f),
      edgeColor_(1.0f, 0.0f, 0.0f),
      vertexColor_(0.0f, 0.0f, 1.0f),
      lineType_(LineType::SOLID),
      wireframeMode_(false),
      projectionType_(ProjectionType::PERSPECTIVE),
      vertexDisplayType_(VertexDisplayType::NONE),
      gifSize_(640, 480) {
  animationTimer_ = new QTimer(this);
  connect(animationTimer_, &QTimer::timeout, this,
          QOverload<>::of(&GLWidget::update));
  animationTimer_->start(16);

  rotationTimer_ = new QTimer(this);
  connect(rotationTimer_, &QTimer::timeout, this, [this]() {
    rotationTime_ += 16;
    emit rotationTimeUpdated(rotationTime_ / 1000.0f);
  });
  rotationTimer_->start(16);

  qDebug() << "GLWidget constructor completed";
}

GLWidget::~GLWidget() {
  qDebug() << "GLWidget destructor called";
  makeCurrent();
  vao.destroy();
  vbo.destroy();
  ibo.destroy();

  // Очищаем буферы осей
  axesVAO.destroy();
  axesVBO.destroy();
  axesIBO.destroy();

  // Очищаем буферы начала координат
  originVAO.destroy();
  originVBO.destroy();
  originIBO.destroy();

  delete shaderProgram;
  delete axesShaderProgram;
  delete vertexShaderProgram;
  delete textShaderProgram;
  delete originShaderProgram;
  clearTexture();
  for (int i = 0; i < 3; ++i) {
    delete textTextures[i];
    textTextures[i] = nullptr;
  }

  doneCurrent();
  qDebug() << "GLWidget destructor completed";
}

void GLWidget::setFacade(const s21::Facade& facade) {
  facade_ = &facade;
  const ObjModel& model = facade_->getObjModel();
  if (!model.modelName.isEmpty()) {
  }
  updateModel();
}

void GLWidget::setProjectionType(ProjectionType type) {
  projectionType_ = type;
  update();
}

void GLWidget::setVertexDisplayType(VertexDisplayType type) {
  vertexDisplayType_ = type;
  update();
}

void GLWidget::setViewFront() {
  cameraRotation_ = QVector3D(0, 0, 0);
  update();
}
void GLWidget::setViewBack() {
  cameraRotation_ = QVector3D(0, 180, 0);
  update();
}
void GLWidget::setViewLeft() {
  cameraRotation_ = QVector3D(0, 90, 0);
  update();
}
void GLWidget::setViewRight() {
  cameraRotation_ = QVector3D(0, -90, 0);
  update();
}
void GLWidget::setViewTop() {
  cameraRotation_ = QVector3D(90, 0, 0);
  update();
}
void GLWidget::setViewBottom() {
  cameraRotation_ = QVector3D(-90, 0, 0);
  update();
}

void GLWidget::setLineType(LineType type) {
  lineType_ = type;
  update();
}

void GLWidget::updateModel() {
  if (!facade_) {
    qDebug() << "Facade not set!";
    return;
  }
  rotationTime_ = 0;

  const ObjModel& newModel = facade_->getObjModel();
  if (!newModel.renderVertices.isEmpty() && !newModel.renderIndices.isEmpty()) {
    model_ = newModel;
    modelLoaded_ = true;
    hasMaterials_ = !model_.materials.isEmpty();
    if (!model_.filePath.isEmpty()) {
      currentModelPath_ = model_.filePath;
    }

    qDebug() << "Model updated successfully:";
    qDebug() << "  Vertices:" << model_.vertices.size();
    qDebug() << "  Faces:" << model_.faces.size();
    qDebug() << "  Render vertices:" << model_.renderVertices.size();
    qDebug() << "  Render indices:" << model_.renderIndices.size();
    qDebug() << "  Has materials:" << hasMaterials_;

    QString texturePath = "";
    for (const auto& mat : model_.materials) {
      if (!mat.textureMap.isEmpty()) {
        texturePath = mat.textureMap;
        if (!QFileInfo(texturePath).isAbsolute() &&
            !currentModelPath_.isEmpty()) {
          QFileInfo objFileInfo(currentModelPath_);
          QString baseDir = objFileInfo.absolutePath();
          texturePath = baseDir + "/" + texturePath;
        }
        qDebug() << "Texture path found:" << texturePath;
        break;
      }
    }
    if (!texturePath.isEmpty() && texturePath_ != texturePath) {
      clearTexture();
      loadTexture(texturePath);
      texturePath_ = texturePath;
    } else if (texturePath.isEmpty() && texture) {
      clearTexture();
    }

    if (context()) {
      makeCurrent();
      updateBuffers();
      doneCurrent();
    }
    recalcAllModelMetrics();
    update();
  } else {
    qDebug() << "Model has no render data";
    modelLoaded_ = false;
    hasMaterials_ = false;
    if (texture) clearTexture();
  }
}

void GLWidget::clearTexture() {
  if (texture) {
    delete texture;
    texture = nullptr;
    texturePath_.clear();
    qDebug() << "Texture cleared";
  }
}

void GLWidget::loadTexture(const QString& texturePath) {
  clearTexture();
  qDebug() << "Loading texture from:" << texturePath;
  QFile file(texturePath);
  if (file.exists()) {
    if (!texture) {
      texture = new QOpenGLTexture(QImage());
    }
    QImage img(texturePath);
    if (!img.isNull()) {
      texture->destroy();
      texture->setData(flipVertical(img));
      texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
      texture->setMagnificationFilter(QOpenGLTexture::Linear);
      texture->setWrapMode(QOpenGLTexture::Repeat);
      texturePath_ = texturePath;
      qDebug() << "Texture loaded successfully, size:" << img.size();
    } else {
      qDebug() << "Failed to load image from" << texturePath;
      clearTexture();
    }
  } else {
    qDebug() << "Texture file not found:" << texturePath;
    clearTexture();
  }
}

void GLWidget::setCurrentPath(const QString& filename) {
  currentModelPath_ = filename;
}

void GLWidget::resetCamera() {
  cameraDistance_ = 5.0f;
  cameraRotation_ = QVector3D(0, 0, 0);
  rotationAngle_ = 0.0f;
  rotationTime_ = 0;
  qDebug() << "Camera reset";
}

void GLWidget::setBackgroundColor(const QColor& color) {
  backgroundColor_.setRedF(color.redF());
  backgroundColor_.setGreenF(color.greenF());
  backgroundColor_.setBlueF(color.blueF());
  backgroundColor_.setAlphaF(color.alphaF());
  if (!isBackgroundCustomized_ &&
      (color.redF() != 1.0f || color.greenF() != 1.0f ||
       color.blueF() != 1.0f)) {
    isBackgroundCustomized_ = true;
  }
  update();
}

QMatrix4x4 GLWidget::getProjectionMatrix() const {
  QMatrix4x4 projection;
  float aspectRatio = (float)width() / height();
  if (projectionType_ == ProjectionType::PERSPECTIVE) {
    projection.perspective(45.0f, aspectRatio, 0.1f, 100.0f);
  } else {
    float size = cameraDistance_ * 0.5f;
    projection.ortho(-size * aspectRatio, size * aspectRatio, -size, size, 0.1f,
                     100.0f);
  }
  return projection;
}

void GLWidget::initializeGL() {
  qDebug() << "=== initializeGL() started ===";

  initializeOpenGLFunctions();

  qDebug() << "OpenGL Version:" << (const char*)glGetString(GL_VERSION);
  qDebug() << "OpenGL Vendor:" << (const char*)glGetString(GL_VENDOR);
  qDebug() << "OpenGL Renderer:" << (const char*)glGetString(GL_RENDERER);
  qDebug() << "GLSL Version:"
           << (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);

  // Базовые настройки OpenGL
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Очистка буфера
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Настройка шейдеров, буферов и вспомогательных объектов
  setupShaders();
  setupBuffers();
  setupAxes();
  setupVertexShader();
  setupVertexPointShader();
  setupOrigin();

  // Загрузка сохранённых настроек
  const UiSettings u = s21::SettingsStore::load();
  applySettings(u);

  update();
  qDebug() << "=== initializeGL() completed ===";
}

void GLWidget::setupShaders() {
  qDebug() << "Setting up shaders...";

  shaderProgram = new QOpenGLShaderProgram(this);

  constexpr const char* vertexShaderSource =
      "#version 330 core\n"
      "layout(location = 0) in vec3 position;\n"
      "layout(location = 1) in vec2 texCoord;\n"
      "layout(location = 2) in vec3 normal;\n"
      "layout(location = 3) in vec3 materialColor;\n"
      "layout(location = 4) in vec3 bary;\n"
      "uniform mat4 modelViewProjection;\n"
      "uniform mat4 modelMatrix;\n"
      "uniform mat3 normalMatrix;\n"
      "out vec3 fragNormal;\n"
      "out vec3 fragPosition;\n"
      "out vec2 fragTexCoord;\n"
      "out vec3 fragMaterialColor;\n"
      "out vec3 vBary;\n"
      "void main() {\n"
      "    gl_Position = modelViewProjection * vec4(position, 1.0);\n"
      "    fragPosition = vec3(modelMatrix * vec4(position, 1.0));\n"
      "    fragNormal = normalize(normalMatrix * normal);\n"
      "    fragTexCoord = texCoord;\n"
      "    fragMaterialColor = materialColor;\n"
      "    vBary = bary;\n"
      "}\n";

  constexpr const char* fragmentShaderSource =
      "#version 330 core\n"
      "in vec3 fragNormal;\n"
      "in vec3 fragPosition;\n"
      "in vec2 fragTexCoord;\n"
      "in vec3 fragMaterialColor;\n"
      "in vec3 vBary;\n"
      "out vec4 fragColor;\n"
      "uniform vec3 lightDirection;\n"
      "uniform vec3 lightColor;\n"
      "uniform sampler2D textureSampler;\n"
      "uniform bool useTexture;\n"
      "uniform bool wireframeMode;\n"
      "uniform vec3 edgeColor;\n"
      "uniform vec3 vertexColor;\n"
      "uniform int lineType;\n"
      "uniform bool isHiddenLine;\n"
      "uniform vec2 viewportSize;\n"
      "uniform float edgeThickness;\n"
      "float edgeMask() { float e = min(min(vBary.x, vBary.y), vBary.z); float "
      "d = fwidth(e); float w = max(1e-6, edgeThickness) * d; return 1.0 - "
      "smoothstep(0.0, w, e); }\n"
      "int edgeId(vec3 b) { return (b.x < b.y && b.x < b.z) ? 0 : ((b.y < b.z) "
      "? 1 : 2); }\n"
      "float edgeParam01(vec3 b, int id) { if (id == 0) return b.y / max(1e-6, "
      "b.y + b.z); if (id == 1) return b.x / max(1e-6, b.x + b.z); return b.x "
      "/ max(1e-6, b.x + b.y); }\n"
      "float dashMask(float t, float repeats, float duty) { float x = fract(t "
      "* repeats); return step(x, duty); }\n"
      "void main() {\n"
      "    vec3 N = normalize(fragNormal);\n"
      "    vec3 L = normalize(-lightDirection);\n"
      "    float diff = max(dot(N, L), 0.0);\n"
      "    vec3 diffuse = diff * lightColor;\n"
      "    vec3 ambient = 0.3 * lightColor;\n"
      "    vec3 color = fragMaterialColor;\n"
      "    if (useTexture && !wireframeMode) {\n"
      "        vec4 texColor = texture(textureSampler, fragTexCoord);\n"
      "        if (texColor.a < 0.1) discard; color *= texColor.rgb;\n"
      "    }\n"
      "    if (wireframeMode) {\n"
      "        float e = edgeMask(); if (e < 0.5) discard;\n"
      "        int eid = edgeId(vBary); float t = edgeParam01(vBary, eid);\n"
      "        if (lineType == 1 || (lineType == 0 && isHiddenLine)) { if "
      "(dashMask(t, 20.0, 0.5) < 0.5) discard; }\n"
      "        else if (lineType == 2) { float x = fract(t * 20.0); if (!((x < "
      "0.15) || (x > 0.35 && x < 0.7))) discard; }\n"
      "        vec3 ec = edgeColor; if (isHiddenLine) ec *= 0.5; vec3 result = "
      "(0.8 * ambient + 0.2 * diffuse) * ec; fragColor = vec4(result, 1.0);\n"
      "    } else { vec3 result = (ambient + diffuse) * color; fragColor = "
      "vec4(result, 1.0); }\n"
      "}\n";

  if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                              vertexShaderSource)) {
    qDebug() << "Vertex shader compilation FAILED:" << shaderProgram->log();
    return;
  }
  if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                              fragmentShaderSource)) {
    qDebug() << "Fragment shader compilation FAILED:" << shaderProgram->log();
    return;
  }
  if (!shaderProgram->link()) {
    qDebug() << "Shader program linking FAILED:" << shaderProgram->log();
    return;
  }

  qDebug() << "Shaders compiled and linked successfully";
}

void GLWidget::setupBuffers() {
  qDebug() << "Setting up buffers...";

  if (vao.create()) {
    vao.bind();
    qDebug() << "VAO created successfully";
  }

  vbo = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
  if (vbo.create()) {
    qDebug() << "VBO created successfully";
  }

  ibo = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
  if (ibo.create()) {
    qDebug() << "IBO created successfully";
  }

  if (modelLoaded_) {
    updateBuffers();
  }

  vao.release();
}

void GLWidget::updateBuffers() {
  SCOPE_TIMER_WARN("GLWidget::updateBuffers", 100.0);
  if (!modelLoaded_ || model_.renderVertices.isEmpty() ||
      model_.renderIndices.isEmpty()) {
    qDebug() << "Cannot update buffers: model not loaded or empty";
    return;
  }

  qDebug() << "Updating buffers with model data...";
  qDebug() << "Vertices size:" << model_.renderVertices.size()
           << "Indices size:" << model_.renderIndices.size();
  qDebug() << "Has materials:" << hasMaterials_;

  vao.bind();

  vbo.bind();
  vbo.allocate(model_.renderVertices.data(),
               model_.renderVertices.size() * sizeof(float));

  ibo.bind();
  ibo.allocate(model_.renderIndices.data(),
               model_.renderIndices.size() * sizeof(unsigned int));

  int stride = 14 * sizeof(float);
  shaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, stride);
  shaderProgram->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(float), 2, stride);
  shaderProgram->setAttributeBuffer(2, GL_FLOAT, 5 * sizeof(float), 3, stride);
  shaderProgram->setAttributeBuffer(3, GL_FLOAT, 8 * sizeof(float), 3, stride);
  shaderProgram->setAttributeBuffer(4, GL_FLOAT, 11 * sizeof(float), 3, stride);
  shaderProgram->enableAttributeArray(4);

  shaderProgram->bind();
  shaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, stride);
  shaderProgram->enableAttributeArray(0);
  shaderProgram->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(float), 2, stride);
  shaderProgram->enableAttributeArray(1);
  shaderProgram->setAttributeBuffer(2, GL_FLOAT, 5 * sizeof(float), 3, stride);
  shaderProgram->enableAttributeArray(2);
  shaderProgram->setAttributeBuffer(3, GL_FLOAT, 8 * sizeof(float), 3, stride);
  shaderProgram->enableAttributeArray(3);
  shaderProgram->release();
  vao.release();

  GLint location;
  location = shaderProgram->attributeLocation("position");
  if (location == -1) qDebug() << "Attribute 'position' not found";
  location = shaderProgram->attributeLocation("texCoord");
  if (location == -1) qDebug() << "Attribute 'texCoord' not found";
  location = shaderProgram->attributeLocation("normal");
  if (location == -1) qDebug() << "Attribute 'normal' not found";
  location = shaderProgram->attributeLocation("materialColor");
  if (location == -1) qDebug() << "Attribute 'materialColor' not found";

  qDebug() << "Buffers updated successfully with material colors";
}

void GLWidget::setupOrigin() {
  qDebug() << "Setting up coordinate origin...";

  originShaderProgram = new QOpenGLShaderProgram(this);

  constexpr const char* originVertexShader =
      "#version 330 core\n"
      "layout(location = 0) in vec3 position;\n"
      "uniform mat4 modelViewProjection;\n"
      "uniform vec3 originColor;\n"
      "out vec3 vertexColor;\n"
      "void main() {\n"
      "    gl_Position = modelViewProjection * vec4(position, 1.0);\n"
      "    vertexColor = originColor;\n"
      "}\n";

  constexpr const char* originFragmentShader =
      "#version 330 core\n"
      "in vec3 vertexColor;\n"
      "out vec4 fragColor;\n"
      "void main() {\n"
      "    fragColor = vec4(vertexColor, 1.0);\n"
      "}\n";

  originShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                               originVertexShader);
  originShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                               originFragmentShader);
  originShaderProgram->link();

  QVector<float> originVertices;
  QVector<unsigned int> originIndices;

  // Простая сфера из треугольников (икосаэдр)
  constexpr const float radius = 0.03f;
  constexpr const float phi = 1.6180339887f;  // Золотое сечение

  // 12 вершин икосаэдра
  float vertices[12][3] = {{-1, phi, 0},  {1, phi, 0},   {-1, -phi, 0},
                           {1, -phi, 0},  {0, -1, phi},  {0, 1, phi},
                           {0, -1, -phi}, {0, 1, -phi},  {phi, 0, -1},
                           {phi, 0, 1},   {-phi, 0, -1}, {-phi, 0, 1}};
  // Нормализуем и масштабируем вершины
  for (int i = 0; i < 12; ++i) {
    float len = std::sqrt(vertices[i][0] * vertices[i][0] +
                          vertices[i][1] * vertices[i][1] +
                          vertices[i][2] * vertices[i][2]);
    originVertices.append(vertices[i][0] / len * radius);
    originVertices.append(vertices[i][1] / len * radius);
    originVertices.append(vertices[i][2] / len * radius);
  }

  // 20 треугольных граней икосаэдра
  unsigned int faces[20][3] = {{0, 11, 5},  {0, 5, 1},  {0, 1, 7},  {0, 7, 10},
                               {0, 10, 11}, {1, 5, 9},  {5, 11, 4}, {11, 10, 2},
                               {10, 7, 6},  {7, 1, 8},  {3, 9, 4},  {3, 4, 2},
                               {3, 2, 6},   {3, 6, 8},  {3, 8, 9},  {4, 9, 5},
                               {2, 4, 11},  {6, 2, 10}, {8, 6, 7},  {9, 8, 1}};
  for (int i = 0; i < 20; ++i) {
    originIndices.append(faces[i][0]);
    originIndices.append(faces[i][1]);
    originIndices.append(faces[i][2]);
  }

  originVAO.create();
  originVAO.bind();

  originVBO = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
  originVBO.create();
  originVBO.bind();
  originVBO.allocate(originVertices.data(),
                     originVertices.size() * sizeof(float));

  originIBO = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
  originIBO.create();
  originIBO.bind();
  originIBO.allocate(originIndices.data(),
                     originIndices.size() * sizeof(unsigned int));

  originShaderProgram->bind();
  originShaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3,
                                          3 * sizeof(float));  // position
  originShaderProgram->enableAttributeArray(0);
  originShaderProgram->release();

  originVAO.release();

  qDebug() << "Coordinate origin setup completed";
}

void GLWidget::setupAxes() {
  qDebug() << "Setting up coordinate axes...";

  axesShaderProgram = new QOpenGLShaderProgram(this);

  constexpr const char* axesVertexShader =
      "#version 330 core\n"
      "layout(location = 0) in vec3 position;\n"
      "layout(location = 1) in vec3 color;\n"
      "uniform mat4 modelViewProjection;\n"
      "out vec3 vertexColor;\n"
      "void main() {\n"
      "    gl_Position = modelViewProjection * vec4(position, 1.0);\n"
      "    vertexColor = color;\n"
      "}\n";

  constexpr const char* axesFragmentShader =
      "#version 330 core\n"
      "in vec3 vertexColor;\n"
      "out vec4 fragColor;\n"
      "void main() {\n"
      "    fragColor = vec4(vertexColor, 1.0);\n"
      "}\n";

  axesShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                             axesVertexShader);
  axesShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                             axesFragmentShader);
  axesShaderProgram->link();

  float axesData[] = {
      // Позиция       // Цвет
      // Ось X (красная)
      0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
      // Ось Y (зелёная)
      0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
      // Ось Z (синяя)
      0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f};

  unsigned int axesIndices[] = {0, 1, 2, 3, 4, 5};

  axesVAO.create();
  axesVAO.bind();

  axesVBO = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
  axesVBO.create();
  axesVBO.bind();
  axesVBO.allocate(axesData, sizeof(axesData));

  axesIBO = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
  axesIBO.create();
  axesIBO.bind();
  axesIBO.allocate(axesIndices, sizeof(axesIndices));

  axesShaderProgram->bind();
  axesShaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3,
                                        6 * sizeof(float));  // position
  axesShaderProgram->enableAttributeArray(0);
  axesShaderProgram->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(float), 3,
                                        6 * sizeof(float));  // color
  axesShaderProgram->enableAttributeArray(1);
  axesShaderProgram->release();

  axesVAO.release();

  setupTextLabels();

  qDebug() << "Coordinate axes setup completed";
}

void GLWidget::setupTextLabels() {
  qDebug() << "Setting up text labels for axes...";

  textShaderProgram = new QOpenGLShaderProgram(this);

  constexpr const char* textVertexShader =
      "#version 330 core\n"
      "layout(location = 0) in vec3 position;\n"
      "layout(location = 1) in vec2 texCoord;\n"
      "uniform mat4 modelViewProjection;\n"
      "out vec2 TexCoord;\n"
      "void main() {\n"
      "    gl_Position = modelViewProjection * vec4(position, 1.0);\n"
      "    TexCoord = texCoord;\n"
      "}\n";

  constexpr const char* textFragmentShader =
      "#version 330 core\n"
      "in vec2 TexCoord;\n"
      "out vec4 fragColor;\n"
      "uniform sampler2D textTexture;\n"
      "void main() {\n"
      "    vec4 sampled = texture(textTexture, TexCoord);\n"
      "    fragColor = vec4(sampled.rgb, sampled.a);\n"
      "    if (fragColor.a < 0.1) discard;\n"
      "}\n";

  textShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                             textVertexShader);
  textShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                             textFragmentShader);
  textShaderProgram->link();

  QFont font;
  font.setPointSize(16);
  font.setBold(true);

  QImage image(64, 64, QImage::Format_RGBA8888);

  QPainter painter(&image);
  painter.setFont(font);
  painter.setRenderHint(QPainter::Antialiasing);

  QString labels[] = {"X", "Y", "Z"};

  for (int i = 0; i < 3; ++i) {
    image.fill(QColor(0, 0, 0, 200));
    painter.setPen(Qt::white);
    painter.drawText(image.rect(), Qt::AlignCenter, labels[i]);
    textTextures[i] = new QOpenGLTexture(flipVertical(image));
    textTextures[i]->setMinificationFilter(QOpenGLTexture::Linear);
    textTextures[i]->setMagnificationFilter(QOpenGLTexture::Linear);
  }

  painter.end();

  float textData[] = {
      // X
      1.1f, -0.05f, -0.05f, 0.0f, 0.0f, 1.2f, -0.05f, -0.05f, 1.0f, 0.0f, 1.2f,
      0.05f, -0.05f, 1.0f, 1.0f, 1.1f, 0.05f, -0.05f, 0.0f, 1.0f,
      // Y
      -0.05f, 1.1f, -0.05f, 0.0f, 0.0f, 0.05f, 1.1f, -0.05f, 1.0f, 0.0f, 0.05f,
      1.2f, -0.05f, 1.0f, 1.0f, -0.05f, 1.2f, -0.05f, 0.0f, 1.0f,
      // Z
      -0.05f, -0.05f, 1.1f, 0.0f, 0.0f, 0.05f, -0.05f, 1.1f, 1.0f, 0.0f, 0.05f,
      0.05f, 1.2f, 1.0f, 1.0f, -0.05f, 0.05f, 1.2f, 0.0f, 1.0f};

  unsigned int textIndices[] = {0, 1, 2, 2, 3, 0,  4,  5,  6,
                                6, 7, 4, 8, 9, 10, 10, 11, 8};

  textVAO.create();
  textVAO.bind();

  textVBO = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
  textVBO.create();
  textVBO.bind();
  textVBO.allocate(textData, sizeof(textData));

  textIBO = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
  textIBO.create();
  textIBO.bind();
  textIBO.allocate(textIndices, sizeof(textIndices));

  textShaderProgram->bind();
  textShaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, 5 * sizeof(float));
  textShaderProgram->enableAttributeArray(0);
  textShaderProgram->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(float), 2,
                                        5 * sizeof(float));
  textShaderProgram->enableAttributeArray(1);
  textShaderProgram->release();

  textVAO.release();

  qDebug() << "Text labels setup completed";
}

void GLWidget::setupVertexShader() {
  qDebug() << "Setting up vertex point shader...";

  vertexShaderProgram = new QOpenGLShaderProgram(this);

  constexpr const char* vertexPointVertexShader =
      "#version 330 core\n"
      "layout(location = 0) in vec3 position;\n"
      "uniform mat4 modelViewProjection;\n"
      "uniform vec3 vertexColor;\n"
      "out vec3 pointColor;\n"
      "void main() {\n"
      "    gl_Position = modelViewProjection * vec4(position, 1.0);\n"
      "    pointColor = vertexColor;\n"
      "}\n";

  constexpr const char* vertexPointFragmentShader =
      "#version 330 core\n"
      "in vec3 pointColor;\n"
      "out vec4 fragColor;\n"
      "void main() {\n"
      "    fragColor = vec4(pointColor, 1.0);\n"
      "}\n";

  vertexShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                               vertexPointVertexShader);
  vertexShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                               vertexPointFragmentShader);

  if (!vertexShaderProgram->link()) {
    qDebug() << "Vertex point shader linking FAILED:"
             << vertexShaderProgram->log();
    return;
  }

  qDebug() << "Vertex point shader compiled successfully";
}

void GLWidget::setupVertexPointShader() {
  vertexPointShaderProgram = new QOpenGLShaderProgram(this);

  constexpr const char* vertexShaderSource =
      "#version 330 core\n"
      "layout(location = 0) in vec3 position;\n"
      "uniform mat4 modelViewProjection;\n"
      "uniform float pointSize;\n"
      "void main() {\n"
      "    gl_Position = modelViewProjection * vec4(position, 1.0);\n"
      "    gl_PointSize = pointSize;\n"
      "}\n";

  constexpr const char* fragmentShaderSource =
      "#version 330 core\n"
      "uniform vec3 vertexColor;\n"
      "uniform int displayType;\n"
      "out vec4 fragColor;\n"
      "void main() {\n"
      "    if (displayType == 0) discard;\n"
      "    vec2 coord = gl_PointCoord - vec2(0.5, 0.5);\n"
      "    if (displayType == 1) {\n"
      "        float dist = length(coord);\n"
      "        if (dist > 0.5) discard;\n"
      "        float alpha = 1.0 - smoothstep(0.4, 0.5, dist);\n"
      "        fragColor = vec4(vertexColor, alpha);\n"
      "    } else if (displayType == 2) {\n"
      "        fragColor = vec4(vertexColor, 1.0);\n"
      "    } else {\n"
      "        discard;\n"
      "    }\n"
      "}\n";

  vertexPointShaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                                    vertexShaderSource);
  vertexPointShaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                                    fragmentShaderSource);

  if (!vertexPointShaderProgram->link()) {
    qDebug() << "Vertex point shader linking failed:"
             << vertexPointShaderProgram->log();
  }
}

void GLWidget::renderOrigin(const QMatrix4x4& mvp) {
  if (!originShaderProgram) return;
  glDisable(GL_DEPTH_TEST);

  originShaderProgram->bind();
  originShaderProgram->setUniformValue("modelViewProjection", mvp);
  originShaderProgram->setUniformValue("originColor",
                                       QVector3D(0.0f, 0.0f, 0.0f));

  originVAO.bind();
  originIBO.bind();
  glDrawElements(GL_TRIANGLES, 60, GL_UNSIGNED_INT, nullptr);
  originIBO.release();
  originVAO.release();

  originShaderProgram->release();
  glEnable(GL_DEPTH_TEST);
}

void GLWidget::renderAxes(const QMatrix4x4& mvp) {
  if (!axesShaderProgram) return;

  glDisable(GL_DEPTH_TEST);
  glLineWidth(3.0f);

  axesShaderProgram->bind();
  axesShaderProgram->setUniformValue("modelViewProjection", mvp);

  axesVAO.bind();
  axesIBO.bind();
  glDrawElements(GL_LINES, 6, GL_UNSIGNED_INT, nullptr);
  axesIBO.release();
  axesVAO.release();

  axesShaderProgram->release();

  if (textShaderProgram) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    textShaderProgram->bind();
    textShaderProgram->setUniformValue("modelViewProjection", mvp);

    textVAO.bind();
    textIBO.bind();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    for (int i = 0; i < 3; ++i) {
      textTextures[i]->bind(0);
      textShaderProgram->setUniformValue("textTexture", 0);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT,
                     (void*)(i * 6 * sizeof(unsigned int)));
      textTextures[i]->release();
    }

    textIBO.release();
    textVAO.release();

    textShaderProgram->release();
    glDisable(GL_BLEND);
  }
  glEnable(GL_DEPTH_TEST);
}

void GLWidget::renderVertexPoints(const QMatrix4x4& mvp) {
  if (vertexDisplayType_ == VertexDisplayType::NONE ||
      !vertexPointShaderProgram) {
    return;
  }
  glEnable(GL_PROGRAM_POINT_SIZE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  vertexPointShaderProgram->bind();
  vertexPointShaderProgram->setUniformValue("modelViewProjection", mvp);
  vertexPointShaderProgram->setUniformValue("vertexColor", vertexColor_);
  vertexPointShaderProgram->setUniformValue("pointSize", vertexSize_);
  vertexPointShaderProgram->setUniformValue(
      "displayType", static_cast<int>(vertexDisplayType_));

  vao.bind();
  glDrawArrays(GL_POINTS, 0, model_.renderVertices.size() / 14);
  vao.release();

  vertexPointShaderProgram->release();
  glDisable(GL_BLEND);
  glDisable(GL_PROGRAM_POINT_SIZE);
}

void GLWidget::resizeGL(int w, int h) {
  qDebug() << "resizeGL called with dimensions:" << w << "x" << h;
  glViewport(0, 0, w, h);
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  qDebug() << "Viewport set to:" << viewport[0] << viewport[1] << viewport[2]
           << viewport[3];
}

void GLWidget::paintGL() {
  static int frameCount = 0;
  frameCount++;

  glClearColor(backgroundColor_.redF(), backgroundColor_.greenF(),
               backgroundColor_.blueF(), backgroundColor_.alphaF());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (!shaderProgram) return;

  rotationAngle_ += rotationSpeed_;
  if (rotationAngle_ > 360.0f) rotationAngle_ -= 360.0f;

  QMatrix4x4 modelMatrix;
  modelMatrix.rotate(rotationAngle_, 0.0f, 1.0f, 0.0f);
  modelMatrix.rotate(cameraRotation_.x(), 1.0f, 0.0f, 0.0f);
  modelMatrix.rotate(cameraRotation_.y(), 0.0f, 1.0f, 0.0f);

  QMatrix4x4 view;
  view.translate(0.0f, 0.0f, -cameraDistance_);
  QMatrix4x4 projection = getProjectionMatrix();
  QMatrix4x4 mvp = projection * view * modelMatrix;

  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  QVector2D viewportSize(viewport[2], viewport[3]);

  if (axesVisible_) {
    renderAxes(mvp);
    renderOrigin(mvp);
  }

  if (!modelLoaded_ || model_.renderIndices.isEmpty()) {
    if (frameCount <= 3) {
      qDebug() << "Frame" << frameCount << "- no model to render";
    }
    return;
  }

  QMatrix3x3 normalMatrix = modelMatrix.normalMatrix();
  vao.bind();
  shaderProgram->bind();

  // Общие uniform
  shaderProgram->setUniformValue("modelViewProjection", mvp);
  shaderProgram->setUniformValue("modelMatrix", modelMatrix);
  shaderProgram->setUniformValue("normalMatrix", normalMatrix);
  shaderProgram->setUniformValue("lightDirection",
                                 QVector3D(0.2f, -1.0f, -0.3f));
  shaderProgram->setUniformValue("lightColor", QVector3D(1.0f, 1.0f, 1.0f));
  shaderProgram->setUniformValue("viewportSize", viewportSize);

  if (wireframeMode_) {
    shaderProgram->setUniformValue("wireframeMode", true);
    shaderProgram->setUniformValue("edgeColor", edgeColor_);
    shaderProgram->setUniformValue("vertexColor", vertexColor_);
    shaderProgram->setUniformValue("useTexture", false);
    shaderProgram->setUniformValue("lineType", static_cast<int>(lineType_));
    shaderProgram->setUniformValue("edgeThickness", edgeThickness_);

    glEnable(GL_DEPTH_TEST);

    if (lineType_ == LineType::SOLID) {
      // Проход 1: видимые рёбра
      shaderProgram->setUniformValue("isHiddenLine", false);
      glDepthFunc(GL_LEQUAL);
      glDrawElements(GL_TRIANGLES, model_.renderIndices.size(), GL_UNSIGNED_INT,
                     nullptr);

      // Проход 2: скрытые рёбра
      shaderProgram->setUniformValue("isHiddenLine", true);
      glDepthFunc(GL_GREATER);
      glDrawElements(GL_TRIANGLES, model_.renderIndices.size(), GL_UNSIGNED_INT,
                     nullptr);

      glDepthFunc(GL_LESS);
    } else {
      shaderProgram->setUniformValue("isHiddenLine", false);
      glDepthFunc(GL_LESS);
      glDrawElements(GL_TRIANGLES, model_.renderIndices.size(), GL_UNSIGNED_INT,
                     nullptr);
    }

  } else {
    shaderProgram->setUniformValue("wireframeMode", false);
    shaderProgram->setUniformValue("useTexture", texture != nullptr);
    shaderProgram->setUniformValue("isHiddenLine", false);

    if (texture) {
      texture->bind(0);
      shaderProgram->setUniformValue("textureSampler", 0);
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawElements(GL_TRIANGLES, model_.renderIndices.size(), GL_UNSIGNED_INT,
                   nullptr);

    if (texture) texture->release();
  }

  shaderProgram->release();
  vao.release();

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  renderVertexPoints(mvp);

  if (frameCount <= 3) {
    qDebug() << "Frame" << frameCount << "rendered successfully";
    qDebug() << "Wireframe mode:" << wireframeMode_
             << "Line type:" << static_cast<int>(lineType_);
  }
}

void GLWidget::mousePressEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    mousePressed_ = true;
    lastMousePos_ = event->pos();
  }
}

void GLWidget::mouseMoveEvent(QMouseEvent* event) {
  if (mousePressed_) {
    QPoint delta = event->pos() - lastMousePos_;
    cameraRotation_.setX(cameraRotation_.x() + delta.y() * 0.5f);
    cameraRotation_.setY(cameraRotation_.y() + delta.x() * 0.5f);
    if (cameraRotation_.x() > 89.0f) cameraRotation_.setX(89.0f);
    if (cameraRotation_.x() < -89.0f) cameraRotation_.setX(-89.0f);
    lastMousePos_ = event->pos();
    update();
  }
}

void GLWidget::mouseReleaseEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    mousePressed_ = false;
  }
}

void GLWidget::wheelEvent(QWheelEvent* event) {
  float delta = event->angleDelta().y() / 120.0f;
  cameraDistance_ -= delta * 0.5f;
  update();
}

void GLWidget::recalcVertexCountClassic() {
  if (!modelLoaded_ || model_.vertices.isEmpty()) {
    vertices_count_classic_ = 0;
    return;
  }
  const qsizetype n = model_.vertices.size();
  QSet<QVector3D> uniqueVertices;
  uniqueVertices.reserve(static_cast<int>(n));
  const QVector<QVector3D>& verts = model_.vertices;
  for (int i = 0; i < n; ++i) uniqueVertices.insert(verts[i]);
  vertices_count_classic_ = uniqueVertices.size();
}

void GLWidget::recalcFaceCountClassic() {
  if (!modelLoaded_) {
    face_count_classic_ = 0;
    return;
  }
  QVector<GeometryAnalyzer::TrueFace> trueFaces =
      GeometryAnalyzer::analyzeFaces(model_);
  face_count_classic_ = trueFaces.size();
}

void GLWidget::recalcTrueEdgeCount() {
  if (!modelLoaded_ || model_.faces.isEmpty()) {
    true_edge_count_ = 0;
    return;
  }
  QSet<QPair<size_t, size_t>> uniqueOriginalEdges;
  uniqueOriginalEdges.reserve(model_.edges.size());
  for (const Edge& edge : model_.edges) {
    QPair<size_t, size_t> edgePair = qMakePair(
        qMin(edge.p1_index, edge.p2_index), qMax(edge.p1_index, edge.p2_index));
    uniqueOriginalEdges.insert(edgePair);
  }
  true_edge_count_ = uniqueOriginalEdges.size();
}

void GLWidget::recalcFaceCount() {
  if (!modelLoaded_) {
    face_count_ = 0;
    return;
  }
  face_count_ = static_cast<int>(model_.faces.size());
}

void GLWidget::recalcEdgeCount() {
  if (!modelLoaded_ || model_.faces.isEmpty()) {
    edge_count_ = 0;
    return;
  }
  QSet<QPair<size_t, size_t>> uniqueTriangulatedEdges;
  uniqueTriangulatedEdges.reserve(model_.faces.size() * 3);
  for (const Face& face : model_.faces) {
    for (int j = 0; j < 3; ++j) {
      size_t v1 = face.vertexIndex[j];
      size_t v2 = face.vertexIndex[(j + 1) % 3];
      if (v1 != static_cast<size_t>(-1) && v2 != static_cast<size_t>(-1)) {
        QPair<size_t, size_t> e = qMakePair(qMin(v1, v2), qMax(v1, v2));
        uniqueTriangulatedEdges.insert(e);
      }
    }
  }
  edge_count_ = uniqueTriangulatedEdges.size();
}

void GLWidget::recalcAllModelMetrics() {
  {
    SCOPE_TIMER("GLWidget::recalcVertexCountClassic");
    recalcVertexCountClassic();
  }
  {
    SCOPE_TIMER("GLWidget::recalcVertexCountTriangulate");
    recalcVertexCountTriangulate();
  }
  {
    SCOPE_TIMER("GLWidget::recalcFaceCount");
    recalcFaceCount();
  }
  {
    SCOPE_TIMER("GLWidget::recalcFaceCountClassic");
    recalcFaceCountClassic();
  }
  {
    SCOPE_TIMER("GLWidget::recalcEdgeCount");
    recalcEdgeCount();
  }
  {
    SCOPE_TIMER("GLWidget::recalcTrueEdgeCount");
    recalcTrueEdgeCount();
  }
}

void GLWidget::invalidateModelMetrics() {
  vertices_count_classic_ = 0;
  face_count_ = 0;
  face_count_classic_ = 0;
  edge_count_ = 0;
  true_edge_count_ = 0;
}

void GLWidget::setEdgeColor(const QVector3D& color) {
  qDebug() << "edgeColor set to" << color;
  edgeColor_ = color;
  update();
}
void GLWidget::setVertexColor(const QVector3D& color) {
  vertexColor_ = color;
  update();
}
void GLWidget::setEdgeThickness(float thickness) {
  edgeThickness_ = thickness;
  update();
}
void GLWidget::setVertexSize(float size) {
  vertexSize_ = size;
  update();
}
void GLWidget::setWireframeMode(bool enabled) {
  wireframeMode_ = enabled;
  update();
}

void GLWidget::keyPressEvent(QKeyEvent* event) {
  switch (event->key()) {
    case Qt::Key_W:
      emit transformRequested(UserAction_t::MoveModel_Y, 1.0f);
      break;
    case Qt::Key_S:
      emit transformRequested(UserAction_t::MoveModel_Y, -1.0f);
      break;
  }
  update();
}

void GLWidget::takeJpeg(const QString& format) {
  QString relative_path = "./records/" + format.toLower();
  QString absolute_path = QDir(relative_path).absolutePath();

  QDir dir;
  if (!dir.exists(absolute_path)) dir.mkpath(absolute_path);

  QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
  QString filePath =
      absolute_path + "/snapshot_" + timestamp + "." + format.toLower();

  QImage screenshot = this->grabFramebuffer();
  QThreadPool::globalInstance()->start([screenshot, filePath,
                                        fmt = format.toUpper()]() {
    const bool success = screenshot.save(filePath, fmt.toUtf8().constData());
    if (!success) {
      qDebug() << "Error save" << fmt << "in:" << filePath;
    } else {
      qDebug() << fmt << "saved in:" << filePath;
    }
  });
}

void GLWidget::recordGif() {
  qDebug() << "recordGif called.";
  auto view = qobject_cast<View*>(this->window());
  if (!view) return;
  qDebug() << "recordGif work.";

  QString relative_path = "./records/gif";
  QString absolute_path = QDir(relative_path).absolutePath();
  QDir dir;
  if (!dir.exists(absolute_path)) dir.mkpath(absolute_path);

  QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
  QString filePath = absolute_path + "/animation_" + timestamp + ".gif";

  gifFrames_.clear();
  gifCurrentFrame_ = 0;

  view->showGifProgress(gifTotalFrames_);

  QTimer* timer = new QTimer(this);

  connect(timer, &QTimer::timeout, this, [this, filePath, timer, view]() {
    QImage frame = this->grabFramebuffer();

    frame = frame.scaled(gifSize_, Qt::KeepAspectRatioByExpanding,
                         Qt::SmoothTransformation);
    int x = (frame.width() - gifSize_.width()) / 2;
    int y = (frame.height() - gifSize_.height()) / 2;
    frame = frame.copy(x, y, gifSize_.width(), gifSize_.height());
    frame = frame.convertToFormat(QImage::Format_RGB888);

    gifFrames_.push_back(frame);
    view->updateGifProgress(gifCurrentFrame_ + 1);

    if (++gifCurrentFrame_ >= gifTotalFrames_) {
      timer->stop();

      QVector<QImage> framesCopy = gifFrames_;
      QSize gifSizeCopy = gifSize_;
      View* viewPtr = view;

      QThreadPool::globalInstance()->start(
          [this, framesCopy, gifSizeCopy, filePath, viewPtr]() {
            QGifImage gif(gifSizeCopy);
            gif.setDefaultDelay(100);
            for (const QImage& f : framesCopy) gif.addFrame(f);

            const bool success = gif.save(filePath);
            const QString message =
                success ? QString("GIF сохранён в: %1").arg(filePath)
                        : QString("Ошибка сохранения GIF в: %1").arg(filePath);

            emit saveStatus(message);  // останется, как у тебя
            QMetaObject::invokeMethod(
                viewPtr, [viewPtr]() { viewPtr->finishGifProgress(); },
                Qt::QueuedConnection);
            qDebug() << "GIF saved in:" << filePath;
          });

      timer->deleteLater();
    }
  });
  timer->start(100);
}

void GLWidget::setAxesVisible(bool v) {
  axesVisible_ = v;
  update();
}
void GLWidget::toggleAxes() { setAxesVisible(!axesVisible_); }
