#include <gtest/gtest.h>

#include <QtCore/QString>

#include "../affine_transformer/affine_transformer.h"
#include "../objloader/objloader.h"
#include "../user_action.h"

using namespace s21;
using namespace std;

static void expectVecNear(const QVector3D& a, const QVector3D& b,
                          float eps = 1e-5f) {
  EXPECT_NEAR(a.x(), b.x(), eps);
  EXPECT_NEAR(a.y(), b.y(), eps);
  EXPECT_NEAR(a.z(), b.z(), eps);
}

TEST(AffineTransformer_Points, Move) {
  AffineTransformer t;
  vector<Point3D> pts = {{1, 2, 3, 1}, {-1, 0, 5, 1}};
  t.Move(pts, 10, -2, 0.5f);
  EXPECT_NEAR(pts[0].x, 11.0f, 1e-6);
  EXPECT_NEAR(pts[0].y, 0.0f, 1e-6);
  EXPECT_NEAR(pts[0].z, 3.5f, 1e-6);
  EXPECT_FLOAT_EQ(pts[0].homo, 1.0f);
  EXPECT_NEAR(pts[1].x, 9.0f, 1e-6);
  EXPECT_NEAR(pts[1].y, -2.0f, 1e-6);
  EXPECT_NEAR(pts[1].z, 5.5f, 1e-6);
  EXPECT_FLOAT_EQ(pts[1].homo, 1.0f);
}

TEST(AffineTransformer_Points, RotateX_90) {
  AffineTransformer t;
  vector<Point3D> pts = {{0, 1, 0, 1}};
  t.RotateX(pts, 90.0f);
  EXPECT_NEAR(pts[0].x, 0.0f, 1e-5);
  EXPECT_NEAR(pts[0].y, 0.0f, 1e-5);
  EXPECT_NEAR(pts[0].z, 1.0f, 1e-5);
}

TEST(AffineTransformer_Points, RotateY_90) {
  AffineTransformer t;
  vector<Point3D> pts = {{1, 0, 0, 1}};
  t.RotateY(pts, 90.0f);
  EXPECT_NEAR(pts[0].x, 0.0f, 1e-5);
  EXPECT_NEAR(pts[0].y, 0.0f, 1e-5);
  EXPECT_NEAR(pts[0].z, -1.0f, 1e-5);
}

TEST(AffineTransformer_Points, RotateZ_90) {
  AffineTransformer t;
  vector<Point3D> pts = {{1, 0, 0, 1}};
  t.RotateZ(pts, 90.0f);
  EXPECT_NEAR(pts[0].x, 0.0f, 1e-5);
  EXPECT_NEAR(pts[0].y, 1.0f, 1e-5);
  EXPECT_NEAR(pts[0].z, 0.0f, 1e-5);
}

TEST(AffineTransformer_Points, Scale_NonUniform) {
  AffineTransformer t;
  vector<Point3D> pts = {{1, -2, 3, 1}};
  t.Scale(pts, 2.0f, 0.5f, -1.0f);
  EXPECT_NEAR(pts[0].x, 2.0f, 1e-6);
  EXPECT_NEAR(pts[0].y, -1.0f, 1e-6);
  EXPECT_NEAR(pts[0].z, -3.0f, 1e-6);
}

TEST(AffineTransformer_Points, Scale_Zero_And_Negative) {
  AffineTransformer t;
  vector<Point3D> pts = {{1, 1, 1, 1}};
  t.Scale(pts, 0.0f, 2.0f, -3.0f);
  EXPECT_NEAR(pts[0].x, 0.0f, 1e-6);
  EXPECT_NEAR(pts[0].y, 2.0f, 1e-6);
  EXPECT_NEAR(pts[0].z, -3.0f, 1e-6);
}

static ObjModel makeTriangleModel() {
  ObjModel m;
  m.vertices = {QVector3D(1, 0, 0), QVector3D(0, 1, 0), QVector3D(0, 0, 1)};
  Face f{};
  f.vertexIndex[0] = 0;
  f.vertexIndex[1] = 1;
  f.vertexIndex[2] = 2;
  f.texCoordIndex[0] = f.texCoordIndex[1] = f.texCoordIndex[2] = -1;
  f.normalIndex[0] = f.normalIndex[1] = f.normalIndex[2] = -1;
  f.material = "";
  m.faces.append(f);
  return m;
}

TEST(AffineTransformer_Model, Apply_Move_X_Y_Z) {
  AffineTransformer t;
  auto m = makeTriangleModel();
  t.applyTransformation(UserAction_t::MoveModel_X, m, 2.0f);
  expectVecNear(m.vertices[0], QVector3D(3, 0, 0));
  auto m2 = makeTriangleModel();
  t.applyTransformation(UserAction_t::MoveModel_Y, m2, -1.0f);
  expectVecNear(m2.vertices[1], QVector3D(0, 0, 0));
  auto m3 = makeTriangleModel();
  t.applyTransformation(UserAction_t::MoveModel_Z, m3, 0.5f);
  expectVecNear(m3.vertices[2], QVector3D(0, 0, 1.5f));
}

TEST(AffineTransformer_Model, Apply_Rotate_XYZ_90) {
  AffineTransformer t;
  {
    auto m = makeTriangleModel();
    t.applyTransformation(UserAction_t::RotateModel_X, m, 90.0f);
    expectVecNear(m.vertices[1], QVector3D(0, 0, 1));
  }
  {
    auto m = makeTriangleModel();
    t.applyTransformation(UserAction_t::RotateModel_Y, m, 90.0f);
    expectVecNear(m.vertices[0], QVector3D(0, 0, -1));
  }
  {
    auto m = makeTriangleModel();
    t.applyTransformation(UserAction_t::RotateModel_Z, m, 90.0f);
    expectVecNear(m.vertices[0], QVector3D(0, 1, 0));
  }
}

TEST(AffineTransformer_Model, Apply_ScaleModel) {
  AffineTransformer t;
  auto m = makeTriangleModel();
  t.applyTransformation(UserAction_t::ScaleModel, m, 2.0f);
  expectVecNear(m.vertices[0], QVector3D(2, 0, 0));
  expectVecNear(m.vertices[1], QVector3D(0, 2, 0));
  expectVecNear(m.vertices[2], QVector3D(0, 0, 2));
  ASSERT_EQ(m.renderVertices.size(), 42u);
  ASSERT_EQ(m.renderIndices.size(), 3u);
  ASSERT_EQ(m.renderEdges.size(), 6u);
}

TEST(AffineTransformer_Model, Apply_NoOp_LoadObj_DoesNotChangeVertices) {
  AffineTransformer t;
  auto m = makeTriangleModel();
  QVector<QVector3D> before = m.vertices;
  t.applyTransformation(UserAction_t::LoadObj, m, 0.0f);
  ASSERT_EQ(m.vertices.size(), before.size());
  expectVecNear(m.vertices[0], before[0]);
  expectVecNear(m.vertices[1], before[1]);
  expectVecNear(m.vertices[2], before[2]);
}
