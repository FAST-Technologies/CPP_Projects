#ifndef CPP4_3DVIEWER_V2_0_GLWIDGET_GLWIDGET_H_
#define CPP4_3DVIEWER_V2_0_GLWIDGET_GLWIDGET_H_

/// \file
/// \brief OpenGL-виджет визуализации 3D-модели, осей и вспомогательных
///        элементов интерфейса (метки осей, точечные вершины, gif-запись).

#include <QColor>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QImage>
#include <QKeyEvent>
#include <QLabel>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QPainter>
#include <QPoint>
#include <QProgressBar>
#include <QSet>
#include <QThreadPool>
#include <QTimer>
#include <QVector3D>
#include <QVector>
#include <QWheelEvent>
#include <QtGlobal>
#include <QtOpenGLWidgets/QOpenGLWidget>
#include <cmath>

#include "../3rdparty/qgifimage/qgifimage.h"
#include "../facade.h"
#include "../objloader/objloader.h"
#include "../scope_timer/scope_timer.h"
#include "../settings_store/settings_store.h"

#ifndef slots
#define slots Q_SLOTS
#endif

namespace s21 {

class View;

/// \brief Тип линии рёбер.
enum class LineType {
  SOLID = 0,   ///< Сплошные; скрытые — пунктир.
  DASHED = 1,  ///< Пунктир (все рёбра).
  DOTDASH = 2
};  ///< Штрихпунктир (все рёбра).

/// \brief Тип проекции.
enum class ProjectionType {
  PERSPECTIVE = 0,  ///< Центральная проекция.
  ORTHOGONAL = 1
};  ///< Параллельная проекция.

/// \brief Отображение вершин.
enum class VertexDisplayType {
  NONE = 0,    ///< Не отображать.
  CIRCLE = 1,  ///< Круглые точки.
  SQUARE = 2
};  ///< Квадратные точки.

/// \brief OpenGL-виджет: отвечает за буферы, шейдеры и прорисовку модели.
/// \sa Facade, View
class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
  Q_OBJECT

 public:
  /// \brief Создать GL-виджет.
  explicit GLWidget(QWidget *parent = nullptr);
  /// \brief Виртуальный деструктор: корректно освобождает GL-ресурсы.
  ~GLWidget() override;

  /// \brief Привязать фасад (источник данных модели).
  void setFacade(const s21::Facade &facade);

  /// \brief Обновить вершинные/индексные буферы из текущей модели.
  void updateBuffers();
  /// \brief Считать модель из фасада и применить (пересчитать
  /// буферы/состояние).
  void updateModel();

  /// \name Параметры камеры/сцены
  ///@{
  void setRotationSpeed(float speed) { rotationSpeed_ = speed; }
  void setCameraDistance(float distance) { cameraDistance_ = distance; }
  void setCurrentPath(const QString &filename);
  void setBackgroundColor(const QColor &color);

  QColor getBackgroundColor() const { return backgroundColor_; }
  QVector3D getEdgeColor() const { return edgeColor_; }
  float getEdgeThickness() const { return edgeThickness_; }
  QVector3D getVertexColor() const { return vertexColor_; }
  float getVertexSize() const { return vertexSize_; }
  float getRotationTime() const { return rotationTime_ / 1000.0f; }
  float getRotationSpeed() const { return rotationSpeed_; }
  ///@}

  /// \brief Сбросить камеру (дистанция/углы/таймер).
  void resetCamera();

  /// \name Состояния модели/режимов
  ///@{
  bool hasModel() const { return modelLoaded_; }
  bool hasMaterials() const { return hasMaterials_; }
  bool wireframeChosen() const { return wireframeMode_; }
  ///@}

  /// \name Подсчёт метрик модели
  ///@{
  void recalcVertexCountClassic();
  void recalcVertexCountTriangulate() {
    vertices_count_triangulate_ = model_.renderVertices.size() / 14;
  }
  void recalcFaceCount();
  void recalcFaceCountClassic();
  void recalcEdgeCount();
  void recalcTrueEdgeCount();
  void recalcAllModelMetrics();
  void invalidateModelMetrics();

  int getVerticesCountClassic() const { return vertices_count_classic_; }
  int getVerticesCountTriangulate() const {
    return vertices_count_triangulate_;
  }
  int getFaceCount() const { return face_count_; }
  int getFaceCountClassic() const { return face_count_classic_; }
  int getEdgeCount() const { return edge_count_; }
  int getTrueEdgeCount() const { return edge_count_; }
  ///@}

  /// \name Внешний вид
  ///@{
  void setEdgeColor(const QVector3D &color);
  void setVertexColor(const QVector3D &color);
  void setEdgeThickness(float thickness);
  void setVertexSize(float size);
  void setWireframeMode(bool enabled);
  void setLineType(LineType type);
  LineType getLineType() const { return lineType_; }

  void setProjectionType(ProjectionType type);
  ProjectionType getProjectionType() const { return projectionType_; }

  void setVertexDisplayType(VertexDisplayType type);
  VertexDisplayType getVertexDisplayType() const { return vertexDisplayType_; }
  ///@}

  /// \name Предустановленные виды камеры
  ///@{
  void setViewFront();
  void setViewBack();
  void setViewLeft();
  void setViewRight();
  void setViewTop();
  void setViewBottom();
  ///@}

  /// \brief Кол-во кадров в записываемом GIF.
  int getGifTotalFrames() const { return gifTotalFrames_; }

  /// \brief Применить сохранённые настройки UI/рендеринга.
  void applySettings(const s21::UiSettings &u);

  /// \brief Количество уникальных вершин оригинальной модели.
  size_t getTotalUniqueVertices() const { return vertices_count_classic_; }

  /// \name Отображение осей
  ///@{
  bool isAxesVisible() const { return axesVisible_; }
  void setAxesVisible(bool v);
  void toggleAxes();
  ///@}

 protected:
  /// \name Жизненный цикл OpenGL
  ///@{
  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;
  ///@}

  /// \name Обработчики ввода
  ///@{
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;
  ///@}

 private:
  const s21::Facade *facade_ = nullptr;  ///< Источник модели (фасад).

  /// \name Вспомогательная инициализация/отрисовка
  ///@{
  void setupShaders();
  void setupBuffers();
  void setupAxes();
  void renderAxes(const QMatrix4x4 &mvp);
  void setupVertexShader();
  void setupTextLabels();
  void loadTexture(const QString &texturePath);
  void clearTexture();
  void renderOrigin(const QMatrix4x4 &mvp);
  void setupOrigin();
  QMatrix4x4 getProjectionMatrix() const;
  void renderVertexPoints(const QMatrix4x4 &mvp);
  void setupVertexPointShader();
  ///@}

  QOpenGLBuffer vbo;
  QOpenGLBuffer ibo;
  QOpenGLShaderProgram *shaderProgram;
  QOpenGLVertexArrayObject vao;
  QOpenGLTexture *texture = nullptr;
  QOpenGLTexture *textTextures[3] = {nullptr, nullptr, nullptr};

  QOpenGLShaderProgram *textShaderProgram;
  QOpenGLVertexArrayObject textVAO;
  QOpenGLBuffer textVBO;
  QOpenGLBuffer textIBO;

  QOpenGLShaderProgram *originShaderProgram;
  QOpenGLVertexArrayObject originVAO;
  QOpenGLBuffer originVBO;
  QOpenGLBuffer originIBO;

  ObjModel model_;
  bool modelLoaded_;
  bool hasMaterials_;
  bool isBackgroundCustomized_ = false;
  QString currentModelPath_;

  float rotationAngle_;
  float rotationSpeed_;
  float cameraDistance_;
  float edgeThickness_;
  float vertexSize_;
  QVector3D cameraRotation_;

  bool mousePressed_;
  QPoint lastMousePos_;

  QTimer *animationTimer_;
  QTimer *rotationTimer_;
  int rotationTime_ = 0;
  QColor backgroundColor_;
  QVector3D edgeColor_;
  QVector3D vertexColor_;
  QString texturePath_;
  LineType lineType_;
  bool wireframeMode_;

  QOpenGLBuffer axesVBO;
  QOpenGLBuffer axesIBO;
  QOpenGLVertexArrayObject axesVAO;
  QOpenGLShaderProgram *axesShaderProgram;
  QOpenGLShaderProgram *vertexShaderProgram;

  ProjectionType projectionType_;
  VertexDisplayType vertexDisplayType_;

  QOpenGLShaderProgram *vertexPointShaderProgram;
  QOpenGLVertexArrayObject vertexPointVAO;
  QOpenGLBuffer vertexPointVBO;

  QVector<QImage> gifFrames_;
  const QSize gifSize_;
  int gifCurrentFrame_ = 0;
  int gifTotalFrames_ = 50;

  int vertices_count_classic_ = 0;
  int vertices_count_triangulate_ = 0;
  int face_count_ = 0;
  int face_count_classic_ = 0;
  int edge_count_ = 0;
  int true_edge_count_ = 0;

  bool axesVisible_ = false;

 public slots:
  /// \brief Сохранить скриншот кадра в формате \p format (JPEG/PNG/BMP/JPG).
  void takeJpeg(const QString &format);
  /// \brief Записать короткий GIF-ролик текущего рендера.
  void recordGif();

 signals:
  /// \brief Обновление времени вращения (для UI).
  void rotationTimeUpdated(float timeSeconds);
  /// \brief Запрос трансформации модели (например, по хоткеям).
  void transformRequested(UserAction_t action, float value = 0.0f);
  /// \brief Сообщение о результате сохранения (путь/ошибка).
  void saveStatus(const QString &message);
};

}  // namespace s21

#endif  // CPP4_3DVIEWER_V2_0_GLWIDGET_GLWIDGET_H_
