#ifndef CPP4_3DVIEWER_V2_0_SETTINGS_STORE_SETTINGS_STORE_H_
#define CPP4_3DVIEWER_V2_0_SETTINGS_STORE_SETTINGS_STORE_H_

/// \file
/// \brief Хранение/восстановление пользовательских настроек UI через QSettings.

#include <QColor>
#include <QSettings>
#include <QVariant>
#include <QVector3D>

namespace s21 {

struct UiSettings {
  QColor background = QColor(255, 255, 255, 255);
  QVector3D edgeColor = {1.0f, 0.0f, 0.0f};
  QVector3D vertexColor = {0.0f, 0.0f, 1.0f};
  float edgeThickness = 1.0f;
  float vertexSize = 5.0f;
  float rotationSpeed = 1.0f;
  int lineType = 0;
  int projectionType = 0;
  int vertexDisplayType = 0;
  bool wireframe = false;
};

class SettingsStore {
 public:
  static UiSettings load() {
    QSettings s;
    UiSettings u;
    s.beginGroup("render");
    u.background = s.value("background", u.background).value<QColor>();
    u.edgeColor = s.value("edgeColor", QVariant::fromValue(u.edgeColor))
                      .value<QVector3D>();
    u.vertexColor = s.value("vertexColor", QVariant::fromValue(u.vertexColor))
                        .value<QVector3D>();
    u.edgeThickness = s.value("edgeThickness", u.edgeThickness).toFloat();
    u.vertexSize = s.value("vertexSize", u.vertexSize).toFloat();
    u.rotationSpeed = s.value("rotationSpeed", u.rotationSpeed).toFloat();
    u.lineType = s.value("lineType", u.lineType).toInt();
    u.projectionType = s.value("projectionType", u.projectionType).toInt();
    u.vertexDisplayType =
        s.value("vertexDisplayType", u.vertexDisplayType).toInt();
    u.wireframe = s.value("wireframe", u.wireframe).toBool();
    s.endGroup();
    return u;
  }

  static void save(const UiSettings& u) {
    QSettings s;
    s.beginGroup("render");
    s.setValue("background", u.background);
    s.setValue("edgeColor", QVariant::fromValue(u.edgeColor));
    s.setValue("vertexColor", QVariant::fromValue(u.vertexColor));
    s.setValue("edgeThickness", u.edgeThickness);
    s.setValue("vertexSize", u.vertexSize);
    s.setValue("rotationSpeed", u.rotationSpeed);
    s.setValue("lineType", u.lineType);
    s.setValue("projectionType", u.projectionType);
    s.setValue("vertexDisplayType", u.vertexDisplayType);
    s.setValue("wireframe", u.wireframe);
    s.endGroup();
  }
};

}  // namespace s21

#endif  // CPP4_3DVIEWER_V2_0_SETTINGS_STORE_SETTINGS_STORE_H_
