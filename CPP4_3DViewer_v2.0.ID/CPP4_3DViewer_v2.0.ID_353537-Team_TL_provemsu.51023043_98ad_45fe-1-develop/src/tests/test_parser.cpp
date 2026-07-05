#include <gtest/gtest.h>
#include <sys/stat.h>

#include <QDir>
#include <QTemporaryDir>

#include "../objloader/objloader.h"
#include "../parser/parser.h"

using s21::ObjModel;
using s21::Parser;
using namespace s21;
using namespace std;

TEST(Parser_Log, Ctor_OpenFail_DoesNotCrash) {
  QTemporaryDir tmp;
  ASSERT_TRUE(tmp.isValid());
  QDir d(tmp.path());
  ASSERT_TRUE(d.mkpath("ro"));
  QString ro = tmp.path() + "/ro";
  QByteArray p = ro.toUtf8();
  ASSERT_EQ(::chmod(p.constData(), 0555), 0);
  QString old = QDir::currentPath();
  ASSERT_TRUE(QDir::setCurrent(ro));
  {
    Parser parser;
    ObjModel m;
    bool ok = Parser::parseObjString("", m, true);
    EXPECT_FALSE(ok);
  }
  QDir::setCurrent(old);
  ::chmod(p.constData(), 0755);
}

TEST(Parser_Basics, EmptyInput_ReturnsFalse) {
  ObjModel m;
  EXPECT_FALSE(Parser::parseObjString("", m, false));
  EXPECT_TRUE(m.vertices.isEmpty());
  EXPECT_TRUE(m.faces.isEmpty());
}

TEST(Parser_Comments, TrailingSharpAreRemoved) {
  ObjModel m;
  std::string obj =
      "v 0 0 0   # a\n"
      "v 1 0 0   # b\n"
      "v 0 1 0   # c\n"
      "f 1 2 3   # t\n";
  bool ok = Parser::parseObjString(obj, m, true);
  EXPECT_TRUE(ok);
  ASSERT_EQ(m.vertices.size(), 3);
  ASSERT_EQ(m.faces.size(), 1);
}

TEST(Parser_Unknown, UnknownLinesAreIgnored) {
  ObjModel m;
  std::string obj =
      "foo bar baz\n"
      "v 0 0 0\nv 1 0 0\nv 0 1 0\n"
      "f 1 2 3\n";
  EXPECT_TRUE(Parser::parseObjString(obj, m, true));
  EXPECT_EQ(m.faces.size(), 1);
}

TEST(Parser_V, GoodBadVertices) {
  ObjModel m1;
  std::string bad =
      "v 1 2\n"
      "v a b c\n"
      "f 1 2 3\n";
  EXPECT_FALSE(Parser::parseObjString(bad, m1, true));
  EXPECT_TRUE(m1.vertices.isEmpty());

  ObjModel m2;
  std::string good =
      "v 0 0 0\nv 1 0 0\nv 0 1 0\n"
      "f 1 2 3\n";
  EXPECT_TRUE(Parser::parseObjString(good, m2, false));
  EXPECT_EQ(m2.vertices.size(), 3);
  EXPECT_EQ(m2.faces.size(), 1);
}

TEST(Parser_VT_VN, FallbacksOnInvalidTokens) {
  ObjModel m;
  std::string obj =
      "v 0 0 0\nv 1 0 0\nv 0 1 0\n"
      "vt x y\n"
      "vn x y z\n"
      "f 1 2 3\n";
  EXPECT_TRUE(Parser::parseObjString(obj, m, true));
  ASSERT_EQ(m.texCoords.size(), 1);
  EXPECT_FLOAT_EQ(m.texCoords[0].x(), 0.0f);
  EXPECT_FLOAT_EQ(m.texCoords[0].y(), 0.0f);
  ASSERT_EQ(m.normals.size(), 1);
  EXPECT_FLOAT_EQ(m.normals[0].x(), 0.0f);
  EXPECT_FLOAT_EQ(m.normals[0].y(), 0.0f);
  EXPECT_FLOAT_EQ(m.normals[0].z(), 1.0f);
}

TEST(Parser_Faces, BareABC_Triangle) {
  ObjModel m;
  std::string obj =
      "v 0 0 0\nv 1 0 0\nv 0 1 0\n"
      "f 1 2 3\n";
  EXPECT_TRUE(Parser::parseObjString(obj, m, false));
  ASSERT_EQ(m.faces.size(), 1);
  const Face& f = m.faces[0];
  EXPECT_EQ(f.vertexIndex[0], 0);
  EXPECT_EQ(f.vertexIndex[1], 1);
  EXPECT_EQ(f.vertexIndex[2], 2);
  EXPECT_EQ(f.texCoordIndex[0], -1);
  EXPECT_EQ(f.normalIndex[0], -1);
}

TEST(Parser_Faces, ASlashB_VTOnly) {
  ObjModel m;
  std::string obj =
      "v 0 0 0\nv 1 0 0\nv 1 1 0\n"
      "vt 0 0\nvt 1 0\nvt 1 1\n"
      "f 1/1 2/2 3/3\n";
  EXPECT_TRUE(Parser::parseObjString(obj, m, false));
  ASSERT_EQ(m.faces.size(), 1);
  const Face& f = m.faces[0];
  EXPECT_EQ(f.texCoordIndex[0], 0);
  EXPECT_EQ(f.texCoordIndex[1], 1);
  EXPECT_EQ(f.texCoordIndex[2], 2);
  EXPECT_EQ(f.normalIndex[0], -1);
  EXPECT_EQ(f.normalIndex[1], -1);
  EXPECT_EQ(f.normalIndex[2], -1);
}

TEST(Parser_Faces, ASlashSlashC_VNOnly) {
  ObjModel m;
  std::string obj =
      "v 0 0 0\nv 1 0 0\nv 0 1 0\n"
      "vn 0 0 1\n"
      "f 1//1 2//1 3//1\n";
  EXPECT_TRUE(Parser::parseObjString(obj, m, false));
  ASSERT_EQ(m.faces.size(), 1);
  const Face& f = m.faces[0];
  EXPECT_EQ(f.normalIndex[0], 0);
  EXPECT_EQ(f.normalIndex[1], 0);
  EXPECT_EQ(f.normalIndex[2], 0);
  EXPECT_EQ(f.texCoordIndex[0], -1);
}

TEST(Parser_Faces, ASlashBSlashC_AllTriplets) {
  ObjModel m;
  std::string obj =
      "v 0 0 0\nv 1 0 0\nv 1 1 0\nv 0 1 0\n"
      "vt 0 0\nvt 1 0\nvt 1 1\nvt 0 1\n"
      "vn 0 0 1\n"
      "usemtl matX\n"
      "f 1/1/1 2/2/1 3/3/1 4/4/1\n";
  bool ok = Parser::parseObjString(obj, m, false);
  ASSERT_TRUE(ok);
  ASSERT_EQ(m.faces.size(), 2);
  EXPECT_EQ(m.originalFaceCount, 1);
  const Face& f0 = m.faces[0];
  const Face& f1 = m.faces[1];
  EXPECT_EQ(f0.vertexIndex[0], 0);
  EXPECT_EQ(f0.vertexIndex[1], 1);
  EXPECT_EQ(f0.vertexIndex[2], 2);
  EXPECT_EQ(f0.texCoordIndex[0], 0);
  EXPECT_EQ(f0.texCoordIndex[1], 1);
  EXPECT_EQ(f0.texCoordIndex[2], 2);
  EXPECT_EQ(f0.normalIndex[0], 0);
  EXPECT_EQ(f0.normalIndex[1], 0);
  EXPECT_EQ(f0.normalIndex[2], 0);
  EXPECT_EQ(f0.material, QString("matX"));
  EXPECT_EQ(f1.vertexIndex[0], 0);
  EXPECT_EQ(f1.vertexIndex[1], 2);
  EXPECT_EQ(f1.vertexIndex[2], 3);
  EXPECT_EQ(f1.texCoordIndex[0], 0);
  EXPECT_EQ(f1.texCoordIndex[1], 2);
  EXPECT_EQ(f1.texCoordIndex[2], 3);
  EXPECT_EQ(f1.normalIndex[0], 0);
  EXPECT_EQ(f1.normalIndex[1], 0);
  EXPECT_EQ(f1.normalIndex[2], 0);
  EXPECT_EQ(f1.material, QString("matX"));
}

TEST(Parser_Faces, Quad_WithMismatchedVT_NotAssigned) {
  ObjModel m;
  std::string obj =
      "v 0 0 0\nv 1 0 0\nv 1 1 0\nv 0 1 0\n"
      "vt 0 0\nvt 1 0\nvt 1 1\n"
      "f 1/1 2/2 3/3 4/9\n";
  bool ok = Parser::parseObjString(obj, m, false);
  ASSERT_TRUE(ok);
  ASSERT_EQ(m.faces.size(), 2);
  const Face& a = m.faces[0];
  const Face& b = m.faces[1];
  EXPECT_EQ(a.texCoordIndex[0], -1);
  EXPECT_EQ(a.texCoordIndex[1], -1);
  EXPECT_EQ(a.texCoordIndex[2], -1);
  EXPECT_EQ(b.texCoordIndex[0], -1);
  EXPECT_EQ(b.texCoordIndex[1], -1);
  EXPECT_EQ(b.texCoordIndex[2], -1);
}

TEST(Parser_Faces, VertexIndexOutOfRange_SkipsFace) {
  ObjModel m;
  std::string obj =
      "v 0 0 0\nv 1 0 0\n"
      "f 1 2 3\n";
  EXPECT_FALSE(Parser::parseObjString(obj, m, false));
  EXPECT_TRUE(m.faces.isEmpty());
}

TEST(Parser_Faces, NegativeIndicesIgnored) {
  ObjModel m;
  std::string obj =
      "v 0 0 0\nv 1 0 0\nv 0 1 0\n"
      "f -1 -2 -3\n";
  EXPECT_FALSE(Parser::parseObjString(obj, m, false));
  EXPECT_TRUE(m.faces.isEmpty());
}

TEST(Parser_Edges, UniqueAcrossFaces) {
  ObjModel m;
  std::string obj =
      "v 0 0 0\nv 1 0 0\nv 0 1 0\nv 1 1 0\n"
      "f 1 2 3\n"
      "f 3 2 4\n";
  EXPECT_TRUE(Parser::parseObjString(obj, m, false));
  ASSERT_EQ(m.faces.size(), 2);
  EXPECT_EQ(m.edges.size(), 5u);
  EXPECT_EQ(m.globalUniqueEdges.size(), 5);
}

TEST(Parser_Meta, O_G_Mtllib_Usemtl_S_Parsed) {
  ObjModel m;
  std::string obj =
      "o MyObj\n"
      "g MyGroup\n"
      "mtllib my.mtl\n"
      "s 1.5\n"
      "v 0 0 0\nv 1 0 0\nv 0 1 0\n"
      "usemtl matA\n"
      "f 1 2 3\n"
      "s off\n";
  EXPECT_TRUE(Parser::parseObjString(obj, m, true));
  EXPECT_EQ(m.modelName, QString("MyObj"));
  EXPECT_EQ(m.group, QString("MyGroup"));
  EXPECT_EQ(m.mtdlib_file, QString("my.mtl"));
  ASSERT_EQ(m.faces.size(), 1);
  EXPECT_EQ(m.faces[0].material, QString("matA"));
  EXPECT_FLOAT_EQ(m.smoothingValue, 0.0f);
  EXPECT_EQ(m.smoothing, QString("off"));
}

TEST(Parser_Log, ParsingCompleted_Message_Path) {
  ObjModel m;
  std::string obj =
      "v 0 0 0\n"
      "v 1 0 0\n"
      "v 0 1 0\n"
      "f 1 2 3\n";
  bool ok = Parser::parseObjString(obj, m, true);
  EXPECT_TRUE(ok);
  EXPECT_FALSE(m.faces.isEmpty());
}

TEST(Parser_Log, Invalid_V_VT_VN_WithLog) {
  ObjModel m1;
  std::string bad_v = "v a b c\nf 1 2 3\n";
  EXPECT_FALSE(Parser::parseObjString(bad_v, m1, true));
  EXPECT_TRUE(m1.vertices.isEmpty());

  ObjModel m2;
  std::string bad_vt = "v 0 0 0\nv 1 0 0\nv 0 1 0\nvt x y\nf 1 2 3\n";
  EXPECT_TRUE(Parser::parseObjString(bad_vt, m2, true));
  ASSERT_EQ(m2.texCoords.size(), 1);
  EXPECT_FLOAT_EQ(m2.texCoords[0].x(), 0.0f);
  EXPECT_FLOAT_EQ(m2.texCoords[0].y(), 0.0f);

  ObjModel m3;
  std::string bad_vn = "v 0 0 0\nv 1 0 0\nv 0 1 0\nvn x y z\nf 1 2 3\n";
  EXPECT_TRUE(Parser::parseObjString(bad_vn, m3, true));
  ASSERT_EQ(m3.normals.size(), 1);
  EXPECT_FLOAT_EQ(m3.normals[0].x(), 0.0f);
  EXPECT_FLOAT_EQ(m3.normals[0].y(), 0.0f);
  EXPECT_FLOAT_EQ(m3.normals[0].z(), 1.0f);
}

TEST(Parser, FaceTriangulation_Assigns_VT_and_VN) {
  ObjModel m;
  std::string obj =
      "v 0 0 0\n"
      "v 1 0 0\n"
      "v 1 1 0\n"
      "v 0 1 0\n"
      "vt 0 0\n"
      "vt 1 0\n"
      "vt 1 1\n"
      "vt 0 1\n"
      "vn 0 0 1\n"
      "usemtl m\n"
      "f 1/1/1 2/2/1 3/3/1 4/4/1\n";
  bool ok = Parser::parseObjString(obj, m, false);
  ASSERT_TRUE(ok);
  ASSERT_EQ(m.faces.size(), 2);

  const Face& f0 = m.faces[0];
  const Face& f1 = m.faces[1];

  EXPECT_EQ(f0.vertexIndex[0], 0);
  EXPECT_EQ(f0.vertexIndex[1], 1);
  EXPECT_EQ(f0.vertexIndex[2], 2);
  EXPECT_EQ(f0.texCoordIndex[0], 0);
  EXPECT_EQ(f0.texCoordIndex[1], 1);
  EXPECT_EQ(f0.texCoordIndex[2], 2);
  EXPECT_EQ(f0.normalIndex[0], 0);
  EXPECT_EQ(f0.normalIndex[1], 0);
  EXPECT_EQ(f0.normalIndex[2], 0);

  EXPECT_EQ(f1.vertexIndex[0], 0);
  EXPECT_EQ(f1.vertexIndex[1], 2);
  EXPECT_EQ(f1.vertexIndex[2], 3);
  EXPECT_EQ(f1.texCoordIndex[0], 0);
  EXPECT_EQ(f1.texCoordIndex[1], 2);
  EXPECT_EQ(f1.texCoordIndex[2], 3);
  EXPECT_EQ(f1.normalIndex[0], 0);
  EXPECT_EQ(f1.normalIndex[1], 0);
  EXPECT_EQ(f1.normalIndex[2], 0);
}

TEST(Parser_Limit, TooManyVertices_ExceedsMax_ReturnsFalse) {
  const size_t N = s21::ObjModel::max_obj_vertices_ + 1;
  std::string obj;
  obj.reserve(N * 8 + 16);
  for (size_t i = 0; i < N; ++i) obj += "v 0 0 0\n";
  obj += "f 1 2 3\n";
  ObjModel m;
  bool ok = Parser::parseObjString(obj, m, true);
  EXPECT_FALSE(ok);
  EXPECT_FALSE(m.faces.isEmpty());
}

TEST(Parser_Floats3, Vertex_SecondFloatFails_AfterFirstOk) {
  ObjModel m;
  std::string obj =
      "v 0 0 0\n"
      "v 1 0 0\n"
      "v 0 a 0\n"
      "v 0 1 0\n"
      "f 1 2 4\n";
  bool ok = Parser::parseObjString(obj, m, true);
  EXPECT_FALSE(ok);
  EXPECT_EQ(m.vertices.size(), 3);
  EXPECT_TRUE(m.faces.isEmpty());
}
