#include <gtest/gtest.h>
#include <sys/stat.h>

#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include <QTextStream>
#include <QtCore/QString>

#include "../objloader/objloader.h"
#include "../parser/parser.h"

using namespace s21;
using namespace std;

static float f3(const QVector3D& v) { return v.length(); }

static void minmax(const QVector<QVector3D>& v, QVector3D& mn, QVector3D& mx) {
  mn = v[0];
  mx = v[0];
  for (const auto& p : v) {
    mn.setX(std::min(mn.x(), p.x()));
    mn.setY(std::min(mn.y(), p.y()));
    mn.setZ(std::min(mn.z(), p.z()));
    mx.setX(std::max(mx.x(), p.x()));
    mx.setY(std::max(mx.y(), p.y()));
    mx.setZ(std::max(mx.z(), p.z()));
  }
}

TEST(ObjLoader, ParseFaceVertexVariants) {
  ObjLoader l;
  {
    QVector<int> v = l.parseFaceVertex("4/3/2");
    ASSERT_EQ(v.size(), 3);
    EXPECT_EQ(v[0], 4);
    EXPECT_EQ(v[1], 3);
    EXPECT_EQ(v[2], 2);
  }
  {
    QVector<int> v = l.parseFaceVertex("4//2");
    ASSERT_EQ(v.size(), 3);
    EXPECT_EQ(v[0], 4);
    EXPECT_EQ(v[1], -1);
    EXPECT_EQ(v[2], 2);
  }
  {
    QVector<int> v = l.parseFaceVertex("4/3");
    ASSERT_EQ(v.size(), 2);
    EXPECT_EQ(v[0], 4);
    EXPECT_EQ(v[1], 3);
  }
  {
    QVector<int> v = l.parseFaceVertex("x/y/z");
    ASSERT_EQ(v.size(), 3);
    EXPECT_EQ(v[0], -1);
    EXPECT_EQ(v[1], -1);
    EXPECT_EQ(v[2], -1);
  }
}

TEST(ObjModel, PrepareRenderData_NoNormals_DefaultColor) {
  ObjModel m;
  m.vertices = {QVector3D(0, 0, 0), QVector3D(1, 0, 0), QVector3D(0, 1, 0)};
  Face f{};
  f.vertexIndex[0] = 0;
  f.vertexIndex[1] = 1;
  f.vertexIndex[2] = 2;
  f.texCoordIndex[0] = f.texCoordIndex[1] = f.texCoordIndex[2] = -1;
  f.normalIndex[0] = f.normalIndex[1] = f.normalIndex[2] = -1;
  f.material = "";
  m.faces.append(f);

  m.prepareRenderData();

  ASSERT_EQ(m.renderIndices.size(), 3u);
  EXPECT_EQ(m.renderIndices[0], 0u);
  EXPECT_EQ(m.renderIndices[1], 1u);
  EXPECT_EQ(m.renderIndices[2], 2u);

  ASSERT_EQ(m.renderEdges.size(), 6u);
  EXPECT_EQ(m.renderEdges[0], 0u);
  EXPECT_EQ(m.renderEdges[1], 1u);
  EXPECT_EQ(m.renderEdges[2], 1u);
  EXPECT_EQ(m.renderEdges[3], 2u);
  EXPECT_EQ(m.renderEdges[4], 2u);
  EXPECT_EQ(m.renderEdges[5], 0u);

  ASSERT_EQ(m.renderVertices.size(), 3 * 14u);

  const float* v0 = m.renderVertices.data() + 0 * 14;
  const float* v1 = m.renderVertices.data() + 1 * 14;
  const float* v2 = m.renderVertices.data() + 2 * 14;

  EXPECT_FLOAT_EQ(v0[0], 0);
  EXPECT_FLOAT_EQ(v0[1], 0);
  EXPECT_FLOAT_EQ(v0[2], 0);
  EXPECT_FLOAT_EQ(v1[0], 1);
  EXPECT_FLOAT_EQ(v1[1], 0);
  EXPECT_FLOAT_EQ(v1[2], 0);
  EXPECT_FLOAT_EQ(v2[0], 0);
  EXPECT_FLOAT_EQ(v2[1], 1);
  EXPECT_FLOAT_EQ(v2[2], 0);

  EXPECT_FLOAT_EQ(v0[5], 0);
  EXPECT_FLOAT_EQ(v0[6], 0);
  EXPECT_FLOAT_EQ(v0[7], 1);
  EXPECT_FLOAT_EQ(v1[5], 0);
  EXPECT_FLOAT_EQ(v1[6], 0);
  EXPECT_FLOAT_EQ(v1[7], 1);
  EXPECT_FLOAT_EQ(v2[5], 0);
  EXPECT_FLOAT_EQ(v2[6], 0);
  EXPECT_FLOAT_EQ(v2[7], 1);

  EXPECT_FLOAT_EQ(v0[8], 0.8f);
  EXPECT_FLOAT_EQ(v0[9], 0.8f);
  EXPECT_FLOAT_EQ(v0[10], 0.8f);

  EXPECT_FLOAT_EQ(v0[11], 1);
  EXPECT_FLOAT_EQ(v0[12], 0);
  EXPECT_FLOAT_EQ(v0[13], 0);
  EXPECT_FLOAT_EQ(v1[11], 0);
  EXPECT_FLOAT_EQ(v1[12], 1);
  EXPECT_FLOAT_EQ(v1[13], 0);
  EXPECT_FLOAT_EQ(v2[11], 0);
  EXPECT_FLOAT_EQ(v2[12], 0);
  EXPECT_FLOAT_EQ(v2[13], 1);
}

TEST(ObjModel, PrepareRenderData_WithNormals_AndMaterialColor) {
  ObjModel m;
  m.vertices = {QVector3D(0, 0, 0), QVector3D(1, 0, 0), QVector3D(0, 1, 0)};
  m.normals = {QVector3D(0, 0, -1)};
  Material mat;
  mat.diffuse = QColor::fromRgbF(0.2f, 0.4f, 0.6f);
  m.materials["mat1"] = mat;

  Face f{};
  f.vertexIndex[0] = 0;
  f.vertexIndex[1] = 1;
  f.vertexIndex[2] = 2;
  f.texCoordIndex[0] = f.texCoordIndex[1] = f.texCoordIndex[2] = -1;
  f.normalIndex[0] = 0;
  f.normalIndex[1] = 0;
  f.normalIndex[2] = 0;
  f.material = "mat1";
  m.faces.append(f);

  m.prepareRenderData();

  ASSERT_EQ(m.renderVertices.size(), 3 * 14u);
  const float* v0 = m.renderVertices.data() + 0 * 14;
  EXPECT_FLOAT_EQ(v0[5], 0);
  EXPECT_FLOAT_EQ(v0[6], 0);
  EXPECT_FLOAT_EQ(v0[7], -1);
  EXPECT_NEAR(v0[8], 0.2f, 1e-5);
  EXPECT_NEAR(v0[9], 0.4f, 1e-5);
  EXPECT_NEAR(v0[10], 0.6f, 1e-5);
}

TEST(ObjModel, BoundingBox_Center_Scale) {
  ObjModel m;
  m.vertices = {QVector3D(2, 2, 2), QVector3D(6, 6, 6), QVector3D(2, 6, 4)};
  QVector3D size = m.calculateBoundingBox();
  EXPECT_FLOAT_EQ(size.x(), 4);
  EXPECT_FLOAT_EQ(size.y(), 4);
  EXPECT_FLOAT_EQ(size.z(), 4);

  m.centerModel();
  QVector3D mn, mx;
  minmax(m.vertices, mn, mx);
  EXPECT_NEAR(mx.x(), -mn.x(), 1e-6);
  EXPECT_NEAR(mx.y(), -mn.y(), 1e-6);
  EXPECT_NEAR(mx.z(), -mn.z(), 1e-6);

  auto before = m.vertices;
  m.scaleModel(2.0f);
  EXPECT_NEAR(m.vertices[0].x(), before[0].x() * 2.0f, 1e-6);
  EXPECT_NEAR(m.vertices[1].y(), before[1].y() * 2.0f, 1e-6);
}

TEST(ObjLoader, LoadFromFile_LoadsMtl_AllFields) {
  QTemporaryDir tmp;
  ASSERT_TRUE(tmp.isValid());
  QString mtlPath = tmp.path() + "/test.mtl";
  QString objPath = tmp.path() + "/mesh.obj";

  QString mtl =
      "newmtl m1\n"
      "Ka 0.1 0.2 0.3\n"
      "Kd 0.4 0.5 0.6\n"
      "Ks 0.7 0.8 0.9\n"
      "Ns 32\n"
      "d 0.75\n"
      "illum 2\n"
      "map_Kd tex.png\n";
  {
    QFile f(mtlPath);
    ASSERT_TRUE(f.open(QIODevice::WriteOnly | QIODevice::Text));
    QTextStream ts(&f);
    ts << mtl;
    f.close();
  }

  QString obj =
      "mtllib test.mtl\n"
      "v 0 0 0\n"
      "v 1 0 0\n"
      "v 0 1 0\n"
      "usemtl m1\n"
      "f 1 2 3\n";
  {
    QFile f(objPath);
    ASSERT_TRUE(f.open(QIODevice::WriteOnly | QIODevice::Text));
    QTextStream ts(&f);
    ts << obj;
    f.close();
  }

  ObjLoader l;
  ObjModel m;
  ASSERT_TRUE(l.loadFromFile(objPath, m));

  ASSERT_TRUE(m.materials.contains("m1"));
  const Material& mm = m.materials["m1"];
  EXPECT_NEAR(mm.ambient.redF(), 0.1f, 1e-5);
  EXPECT_NEAR(mm.ambient.greenF(), 0.2f, 1e-5);
  EXPECT_NEAR(mm.ambient.blueF(), 0.3f, 1e-5);
  EXPECT_NEAR(mm.diffuse.redF(), 0.4f, 1e-5);
  EXPECT_NEAR(mm.diffuse.greenF(), 0.5f, 1e-5);
  EXPECT_NEAR(mm.diffuse.blueF(), 0.6f, 1e-5);
  EXPECT_NEAR(mm.specular.redF(), 0.7f, 1e-5);
  EXPECT_NEAR(mm.specular.greenF(), 0.8f, 1e-5);
  EXPECT_NEAR(mm.specular.blueF(), 0.9f, 1e-5);
  EXPECT_NEAR(mm.specularExponent, 32.0f, 1e-6);
  EXPECT_NEAR(mm.dissolve, 0.75f, 1e-6);
  EXPECT_EQ(mm.illuminationModel, 2);
  EXPECT_EQ(mm.textureMap, QString("tex.png"));
}

TEST(ObjLoader, LoadFromFile_ApplyDiffuseColor_ToRenderData) {
  QTemporaryDir tmp;
  ASSERT_TRUE(tmp.isValid());
  QString mtlPath = tmp.path() + "/test.mtl";
  QString objPath = tmp.path() + "/mesh.obj";

  QString mtl =
      "newmtl m1\n"
      "Kd 0.4 0.5 0.6\n";
  {
    QFile f(mtlPath);
    ASSERT_TRUE(f.open(QIODevice::WriteOnly | QIODevice::Text));
    QTextStream ts(&f);
    ts << mtl;
    f.close();
  }

  QString obj =
      "mtllib test.mtl\n"
      "v 0 0 0\n"
      "v 1 0 0\n"
      "v 0 1 0\n"
      "usemtl m1\n"
      "f 1 2 3\n";
  {
    QFile f(objPath);
    ASSERT_TRUE(f.open(QIODevice::WriteOnly | QIODevice::Text));
    QTextStream ts(&f);
    ts << obj;
    f.close();
  }

  ObjLoader l;
  ObjModel m;
  ASSERT_TRUE(l.loadFromFile(objPath, m));
  m.prepareRenderData();
  ASSERT_EQ(m.renderVertices.size(), 42u);
  const float* v0 = m.renderVertices.data() + 0 * 14;
  EXPECT_NEAR(v0[8], 0.4f, 1e-5);
  EXPECT_NEAR(v0[9], 0.5f, 1e-5);
  EXPECT_NEAR(v0[10], 0.6f, 1e-5);
}

TEST(GeometryAnalyzer, CoplanarMergeAndEdgeCount) {
  ObjModel m;
  m.vertices = {QVector3D(0, 0, 0), QVector3D(1, 0, 0), QVector3D(1, 1, 0),
                QVector3D(0, 1, 0)};
  Face f1{};
  f1.vertexIndex[0] = 0;
  f1.vertexIndex[1] = 1;
  f1.vertexIndex[2] = 2;
  f1.normalIndex[0] = f1.normalIndex[1] = f1.normalIndex[2] = -1;
  f1.texCoordIndex[0] = f1.texCoordIndex[1] = f1.texCoordIndex[2] = -1;
  f1.material = "m";
  Face f2{};
  f2.vertexIndex[0] = 0;
  f2.vertexIndex[1] = 2;
  f2.vertexIndex[2] = 3;
  f2.normalIndex[0] = f2.normalIndex[1] = f2.normalIndex[2] = -1;
  f2.texCoordIndex[0] = f2.texCoordIndex[1] = f2.texCoordIndex[2] = -1;
  f2.material = "m";
  m.faces = {f1, f2};

  auto tf = GeometryAnalyzer::analyzeFaces(m);
  ASSERT_EQ(tf.size(), 1);
  EXPECT_EQ(tf[0].vertices.size(), 4u);
  int edges = GeometryAnalyzer::getTrueEdgeCount(tf);
  EXPECT_EQ(edges, 4);
}

TEST(GeometryAnalyzer, NoMergeDifferentMaterial) {
  ObjModel m;
  m.vertices = {QVector3D(0, 0, 0), QVector3D(1, 0, 0), QVector3D(1, 1, 0),
                QVector3D(0, 1, 0)};
  Face f1{};
  f1.vertexIndex[0] = 0;
  f1.vertexIndex[1] = 1;
  f1.vertexIndex[2] = 2;
  f1.material = "a";
  Face f2{};
  f2.vertexIndex[0] = 0;
  f2.vertexIndex[1] = 2;
  f2.vertexIndex[2] = 3;
  f2.material = "b";
  m.faces = {f1, f2};

  auto tf = GeometryAnalyzer::analyzeFaces(m);
  ASSERT_EQ(tf.size(), 2);
  int edges = GeometryAnalyzer::getTrueEdgeCount(tf);
  EXPECT_EQ(edges, 5);
}

TEST(GeometryAnalyzer, ShareEdgeAndFaceNormal) {
  Face a{}, b{};
  a.vertexIndex[0] = 0;
  a.vertexIndex[1] = 1;
  a.vertexIndex[2] = 2;
  b.vertexIndex[0] = 2;
  b.vertexIndex[1] = 1;
  b.vertexIndex[2] = 3;
  EXPECT_TRUE(GeometryAnalyzer::shareEdge(a, b));

  QVector<QVector3D> tri = {QVector3D(0, 0, 0), QVector3D(1, 0, 0),
                            QVector3D(0, 1, 0)};
  QVector3D n = GeometryAnalyzer::calculateFaceNormal(tri);
  EXPECT_NEAR(n.x(), 0, 1e-6);
  EXPECT_NEAR(n.y(), 0, 1e-6);
  EXPECT_NEAR(n.z(), 1, 1e-6);

  QVector<QVector3D> noncop = {QVector3D(0, 0, 0), QVector3D(1, 0, 0),
                               QVector3D(0, 1, 0), QVector3D(0, 0, 0.1f)};
  EXPECT_FALSE(GeometryAnalyzer::areCoplanar(noncop, 1e-3f));
  EXPECT_TRUE(GeometryAnalyzer::areCoplanar(noncop, 0.2f));
}

TEST(ObjLoader, LoadFromString_Integration) {
  ObjLoader l;
  ObjModel m;
  QString obj =
      "o M\n"
      "v 0 0 0\n"
      "v 1 0 0\n"
      "v 0 1 0\n"
      "vt 0 0\n"
      "vt 1 0\n"
      "vt 0 1\n"
      "vn 0 0 1\n"
      "usemtl matX\n"
      "f 1/1/1 2/2/1 3/3/1\n";
  bool ok = l.loadFromString(obj, m);
  ASSERT_TRUE(ok);
  ASSERT_EQ(m.vertices.size(), 3);
  ASSERT_EQ(m.texCoords.size(), 3);
  ASSERT_EQ(m.normals.size(), 1);
  ASSERT_EQ(m.faces.size(), 1);
  EXPECT_EQ(m.modelName, QString("M"));
  m.prepareRenderData();
  ASSERT_EQ(m.renderVertices.size(), 42u);
}

TEST(ObjLoader, LoadFromFile_ReturnsFalse_WhenFileMissing) {
  ObjLoader l;
  ObjModel m;
  QString bad = "/definitely/not/exist/mesh.obj";
  EXPECT_FALSE(l.loadFromFile(bad, m));
  EXPECT_TRUE(m.vertices.isEmpty());
  EXPECT_TRUE(m.faces.isEmpty());
  EXPECT_TRUE(m.edges.isEmpty());
}

TEST(ObjLoader, LogFile_OpenFail_OnReadOnlyCWD) {
  QTemporaryDir tmp;
  ASSERT_TRUE(tmp.isValid());
  QString ro = tmp.path() + "/ro";
  QDir d(tmp.path());
  ASSERT_TRUE(d.mkpath("ro"));
  QByteArray b = ro.toUtf8();
  ASSERT_EQ(::chmod(b.constData(), 0555), 0);
  QString old = QDir::currentPath();
  ASSERT_TRUE(QDir::setCurrent(ro));
  {
    ObjLoader l;
    ObjModel m;
  }
  QDir::setCurrent(old);
  ::chmod(b.constData(), 0755);
}

TEST(ObjModel, PrepareRenderData_InvalidVertexIndex_FallbackZeroPos) {
  ObjModel m;
  m.vertices = {QVector3D(0, 0, 0), QVector3D(1, 0, 0)};

  Face f{};
  f.vertexIndex[0] = 0;
  f.vertexIndex[1] = 1;
  f.vertexIndex[2] = 2;
  f.texCoordIndex[0] = f.texCoordIndex[1] = f.texCoordIndex[2] = -1;
  f.normalIndex[0] = f.normalIndex[1] = f.normalIndex[2] = -1;
  f.material = "";
  m.faces.append(f);

  m.prepareRenderData();

  ASSERT_EQ(m.renderVertices.size(), 3 * 14u);
  const float* v2 = m.renderVertices.data() + 2 * 14;
  EXPECT_FLOAT_EQ(v2[0], 0.0f);
  EXPECT_FLOAT_EQ(v2[1], 0.0f);
  EXPECT_FLOAT_EQ(v2[2], 0.0f);

  EXPECT_FLOAT_EQ(v2[5], 0.0f);
  EXPECT_FLOAT_EQ(v2[6], 0.0f);
  EXPECT_FLOAT_EQ(v2[7], 1.0f);

  ASSERT_EQ(m.renderIndices.size(), 3u);
  EXPECT_EQ(m.renderIndices[0], 0u);
  EXPECT_EQ(m.renderIndices[1], 1u);
  EXPECT_EQ(m.renderIndices[2], 2u);
}
