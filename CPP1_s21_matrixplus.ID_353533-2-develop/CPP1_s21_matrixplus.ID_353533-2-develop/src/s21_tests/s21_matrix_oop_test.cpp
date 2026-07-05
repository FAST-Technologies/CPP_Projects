#include "./../s21_include/s21_matrix_oop.h"

#include <gtest/gtest.h>

#include <random>

int GetRandInt(int min, int max) {
  std::random_device rd;
  std::mt19937 generator(rd());
  std::uniform_int_distribution<> distribution(min, max);
  return distribution(generator);
}

double GetRandDouble(double min, double max) {
  std::random_device rd;
  std::mt19937 generator(rd());
  std::uniform_real_distribution<double> distribution(min, max);
  return distribution(generator);
}

void FillMatrix(S21Matrix& matrix, double value) {
  for (int i = 0; i < matrix.get_rows(); ++i) {
    for (int j = 0; j < matrix.get_cols(); ++j) {
      matrix(i, j) = value;
    }
  }
}

TEST(ConstructorTests, DefaultConstructor) {
  S21Matrix A;
  EXPECT_EQ(A.get_rows() + 2, S21Matrix::MATRIX_SIZE + 2);
  EXPECT_EQ(A.get_cols() + 2, S21Matrix::MATRIX_SIZE + 2);
}

TEST(ConstructorTests, RandomDefaultConstructor) {
  S21Matrix A;
  EXPECT_EQ(A.get_rows(), S21Matrix::MATRIX_SIZE);
  EXPECT_EQ(A.get_cols(), S21Matrix::MATRIX_SIZE);
  S21Matrix B(10, 10);
  EXPECT_EQ(B.get_rows(), 10);
  EXPECT_EQ(B.get_cols(), 10);
}

TEST(ConstructorTests, MoveConstructor) {
  S21Matrix A(3, 3);
  A(0, 0) = 1.0;
  S21Matrix B(std::move(A));
  EXPECT_EQ(B.get_rows(), 3);
  EXPECT_EQ(B.get_cols(), 3);
  EXPECT_EQ(B(0, 0), 1.0);
  EXPECT_EQ(A.get_rows(), 0);
  EXPECT_EQ(A.get_cols(), 0);
}

TEST(ConstructorTests, RandomMoveConstructor) {
  int rows = GetRandInt(1, 5), cols = GetRandInt(1, 5);
  S21Matrix A(rows, cols);
  FillMatrix(A, GetRandDouble(-10, 10));
  S21Matrix B(std::move(A));
  EXPECT_EQ(B.get_rows(), rows);
  EXPECT_EQ(B.get_cols(), cols);
  EXPECT_EQ(A.get_rows(), 0);
  EXPECT_EQ(A.get_cols(), 0);
}

TEST(ConstructorTests, CopyConstructorBasic) {
  S21Matrix a(2, 2);
  a(0, 0) = 4;
  a(0, 1) = 5;
  a(1, 0) = 1;
  a(1, 1) = 8;
  S21Matrix b(a);
  ASSERT_TRUE(b == a);
}

TEST(ConstructorTests, RandomCopyConstructorBasic) {
  int rows = GetRandInt(1, 5), cols = GetRandInt(1, 5);
  S21Matrix A(rows, cols);
  FillMatrix(A, GetRandDouble(-10, 10));
  S21Matrix b(A);
  ASSERT_TRUE(b == A);
}

TEST(ConstructorTests, CopyConstructorAfterMult) {
  S21Matrix a(2, 2);
  a(0, 0) = 4;
  a(0, 1) = 8;
  a(1, 0) = 16;
  a(1, 1) = -24;
  a *= a;
  S21Matrix b(a);
  ASSERT_TRUE(b == a);
}

TEST(ConstructorTests, RandomCopyConstructorAfterMult) {
  int rows = GetRandInt(1, 5), cols = GetRandInt(1, 5);
  S21Matrix a(rows, cols);
  FillMatrix(a, GetRandDouble(-10, 10));
  if (cols == rows) a *= a;
  S21Matrix b(a);
  ASSERT_TRUE(b == a);
}

TEST(ConstructorTests, CopyConstructorWithAssignment) {
  S21Matrix A(2, 2);
  A(0, 0) = 8;
  A(0, 1) = 3;
  A(1, 0) = 5;
  A(1, 1) = 7;
  A = A * A;
  S21Matrix b(A);
  ASSERT_TRUE(b == A);
}

TEST(ConstructorTests, RandomCopyConstructorWithAssignment) {
  int rows = GetRandInt(1, 5), cols = GetRandInt(1, 5);
  S21Matrix a(rows, cols);
  FillMatrix(a, GetRandDouble(-10, 10));
  if (cols == rows) a = a * a;
  S21Matrix b(a);
  ASSERT_TRUE(b == a);
}

TEST(ConstructorTests, CustomConstructorInvalidSize) {
  EXPECT_THROW(S21Matrix(-1, 2), std::invalid_argument);
  EXPECT_THROW(S21Matrix(2, -1), std::invalid_argument);
  EXPECT_THROW(S21Matrix(0, 0), std::invalid_argument);
}

TEST(ResizeTests, SetRowsUp) {
  S21Matrix A(9, 9);
  A.set_rows(6);
  EXPECT_EQ(A.get_rows(), 6);
  EXPECT_EQ(A.get_cols(), 9);
  EXPECT_EQ(A(4, 0), 0.0);
}

TEST(ResizeTests, RandomSetRowsUp) {
  int rows = GetRandInt(1, 5), cols = GetRandInt(1, 5);
  S21Matrix A(rows, cols);
  int newRows = rows + GetRandInt(1, 5);
  A.set_rows(newRows);
  EXPECT_EQ(A.get_rows(), newRows);
  EXPECT_EQ(A.get_cols(), cols);
}

TEST(ResizeTests, SetColsUp) {
  S21Matrix A(8, 8);
  A.set_cols(9);
  EXPECT_EQ(A.get_cols(), 9);
  EXPECT_EQ(A.get_rows(), 8);
  EXPECT_EQ(A(0, 3), 0.0);
}

TEST(ResizeTests, RandomSetColsUp) {
  int rows = GetRandInt(1, 5), cols = GetRandInt(1, 5);
  S21Matrix A(rows, cols);
  int newCols = cols + GetRandInt(1, 5);
  A.set_cols(newCols);
  EXPECT_EQ(A.get_cols(), newCols);
  EXPECT_EQ(A.get_rows(), rows);
}

TEST(ResizeTests, SetRowsDown) {
  S21Matrix A(4, 8);
  A.set_rows(2);
  EXPECT_EQ(A.get_rows(), 2);
  EXPECT_EQ(A.get_cols(), 8);
  EXPECT_FALSE(A.get_rows() == 4);
}

TEST(ResizeTests, SetColsDown) {
  S21Matrix A(9, 4);
  A.set_cols(2);
  EXPECT_EQ(A.get_cols(), 2);
  EXPECT_EQ(A.get_rows(), 9);
  EXPECT_FALSE(A.get_rows() == 4);
}

TEST(ResizeTests, SetRowsInvalid) {
  S21Matrix A(2, 2);
  EXPECT_THROW(A.set_rows(-1), std::invalid_argument);
}

TEST(ResizeTests, RandomSetRowsDown) {
  int rows = GetRandInt(3, 10), cols = GetRandInt(1, 5);
  S21Matrix A(rows, cols);
  FillMatrix(A, GetRandDouble(-10, 10));
  int newRows = GetRandInt(1, rows - 1);
  A.set_rows(newRows);
  EXPECT_EQ(A.get_rows(), newRows);
  EXPECT_EQ(A.get_cols(), cols);
  EXPECT_THROW(A(rows - 1, 0), std::out_of_range);
}

TEST(AccessorTests, GetRowsBasic) {
  S21Matrix A(122, 3);
  EXPECT_EQ(A.get_rows(), 122);
}

TEST(AccessorTests, RandomGetRowsBasic) {
  int rows = GetRandInt(1, 10), cols = GetRandInt(1, 10);
  S21Matrix A(rows, cols);
  EXPECT_EQ(A.get_rows(), rows);
}

TEST(ConstructorTests, RandomCustomConstructor) {
  int rows = GetRandInt(1, 10), cols = GetRandInt(1, 10);
  S21Matrix A(rows, cols);
  EXPECT_EQ(A.get_rows(), rows);
  EXPECT_EQ(A.get_cols(), cols);
}

TEST(ConstructorTests, CustomConstructor_3x5) {
  S21Matrix A(3, 5);
  EXPECT_EQ(A.get_rows(), 3);
  EXPECT_EQ(A.get_cols(), 5);
}

TEST(EqualityTests, RandomEqualEmptyMatrices) {
  int size = GetRandInt(1, 5);
  S21Matrix A(size, size);
  S21Matrix D(size, size);
  ASSERT_TRUE(A == D);
}

TEST(EqualityTests, EqualEmptyMatrices) {
  S21Matrix A(18, 18);
  S21Matrix B(18, 18);
  ASSERT_TRUE(A == B);
}

TEST(EqualityTests, UnequalDifferentSize) {
  S21Matrix A(3, 3);
  S21Matrix B(2, 2);
  ASSERT_FALSE(A == B);
}

TEST(EqualityTests, UnequalDifferentValues) {
  S21Matrix A(2, 2);
  S21Matrix F(2, 2);
  A(0, 0) = 1.0;
  F(0, 0) = 3.14356;
  ASSERT_FALSE(A == F);
}

TEST(EqualityTests, RandomUnequalDifferentSize) {
  int rows1 = GetRandInt(1, 5), cols1 = GetRandInt(1, 5);
  int rows2 = rows1 + GetRandInt(1, 5), cols2 = cols1 + GetRandInt(1, 5);
  S21Matrix A(rows1, cols1);
  S21Matrix B(rows2, cols2);
  ASSERT_FALSE(A == B);
}

TEST(EqualityTests, EqualWithEpsilon) {
  S21Matrix A(2, 2), B(2, 2);
  A(0, 0) = 1.0;
  A(0, 1) = 1e-10;
  B(0, 0) = 1.0;
  B(0, 1) = 0.0;
  ASSERT_TRUE(A == B);
}

TEST(EqualityTests, RandomEqualSameSizeDefault) {
  int size = GetRandInt(1, 5);
  S21Matrix a(size, size), b(size, size);
  ASSERT_TRUE(a == b);
}

TEST(EqualityTests, RandomNotEqualDifferentSizes) {
  int rows1 = GetRandInt(1, 5), cols1 = GetRandInt(1, 5);
  int rows2 = rows1 + GetRandInt(1, 5), cols2 = cols1 + GetRandInt(1, 5);
  S21Matrix a(rows1, cols1);
  S21Matrix b(rows2, cols2);
  EXPECT_FALSE(a == b);
}

TEST(OperatorTests, MultiplyByNegativeNumber) {
  S21Matrix D(2, 2), Y(2, 2);
  D(0, 0) = 1.34;
  D(0, 1) = 2.85;
  D(1, 0) = -2.14;
  D(1, 1) = 6.76;
  Y(0, 0) = -1.34;
  Y(0, 1) = -2.85;
  Y(1, 0) = 2.14;
  Y(1, 1) = -6.76;
  S21Matrix c = Y * -1;
  ASSERT_TRUE(c == D);
}

TEST(OperatorTests, RandomMultiplyByNegativeNumber) {
  int size = GetRandInt(1, 5);
  S21Matrix a(size, size), b(size, size);
  double s = GetRandDouble(-10, 10);
  FillMatrix(a, s);
  FillMatrix(b, s * (-1));
  S21Matrix c = b * (-1);
  ASSERT_TRUE(c == a);
}

TEST(OperatorTests, RandomAddMatrix) {
  int size = GetRandInt(1, 5);
  S21Matrix a(size, size), b(size, size), c(size, size);
  for (int i = 0; i < size; ++i)
    for (int j = 0; j < size; ++j) {
      a(i, j) = i + j;
      b(i, j) = (i + j) * GetRandDouble(1, 5);
      c(i, j) = a(i, j) + b(i, j);
    }
  ASSERT_TRUE(a + b == c);
}

TEST(OperatorTests, AddMatrix) {
  S21Matrix S(2, 2), T(2, 2), L(2, 2);
  for (int i = 0; i < 2; ++i)
    for (int j = 0; j < 2; ++j) {
      S(i, j) = (i + j) * (-3);
      T(i, j) = (i + j) * 6;
      L(i, j) = S(i, j) + T(i, j);
    }
  ASSERT_TRUE(S + T == L);
}

TEST(OperatorTests, AddMatrixInvalid) {
  S21Matrix a(2, 2), b(3, 3);
  EXPECT_THROW(a + b, std::invalid_argument);
}

TEST(AccessorTests, GetColsBasic) {
  S21Matrix a(3, 8);
  EXPECT_NE(a.get_cols(), 4);
}

TEST(AccessorTests, RandomGetColsBasic) {
  int rows = GetRandInt(1, 10), cols = GetRandInt(1, 10);
  S21Matrix a(rows, cols);
  EXPECT_EQ(a.get_cols(), cols);
}

TEST(OperatorTests, AssignmentOperator) {
  S21Matrix Q(2, 2), T(3, 3);
  Q = T;
  EXPECT_EQ(Q.get_rows(), 3);
  EXPECT_EQ(Q.get_cols(), 3);
  EXPECT_TRUE(Q == T);
}

TEST(OperatorTests, RandomAssignmentOperator) {
  int rows1 = GetRandInt(1, 5), cols1 = GetRandInt(1, 5);
  int rows2 = GetRandInt(rows1, 10), cols2 = GetRandInt(cols1, 10);
  S21Matrix a(rows1, cols1), b(rows2, cols2);
  FillMatrix(b, GetRandDouble(-10, 10));
  a = b;
  EXPECT_EQ(a.get_rows(), rows2);
  EXPECT_EQ(a.get_cols(), cols2);
  EXPECT_TRUE(a == b);
}

TEST(EqualityTests, EqualSameSizeDefault) {
  S21Matrix E(2, 2), Q(2, 2);
  ASSERT_TRUE(E == Q);
}

TEST(EqualityTests, NotEqualDifferentSizes) {
  S21Matrix R(3, 8), T(4, 9);
  EXPECT_FALSE(R == T);
}

TEST(OperatorTests, RandomMatrixSubtraction) {
  int size = GetRandInt(1, 5);
  S21Matrix a(size, size), b(size, size), c(size, size);
  FillMatrix(a, GetRandDouble(-10, 10));
  FillMatrix(b, GetRandDouble(-10, 10));
  for (int i = 0; i < size; ++i)
    for (int j = 0; j < size; ++j) c(i, j) = a(i, j) - b(i, j);
  ASSERT_TRUE(a - b == c);
}

TEST(OperatorTests, MatrixSubtraction) {
  S21Matrix U(2, 2), F(2, 2), O(2, 2);
  U(0, 0) = 12;
  U(0, 1) = 8;
  U(1, 0) = 5;
  U(1, 1) = 6;
  F(0, 0) = 2;
  F(0, 1) = 4;
  F(1, 0) = 5;
  F(1, 1) = 7;
  O(0, 0) = 10;
  O(0, 1) = 4;
  O(1, 0) = 0;
  O(1, 1) = -1;
  ASSERT_TRUE(U - F == O);
}

TEST(OperatorTests, RandomAddEqualOperator) {
  int size = GetRandInt(1, 5);
  S21Matrix a(size, size), b(size, size), c(size, size);
  FillMatrix(a, GetRandDouble(-10, 10));
  FillMatrix(b, GetRandDouble(-10, 10));
  for (int i = 0; i < size; ++i)
    for (int j = 0; j < size; ++j) c(i, j) = a(i, j) + b(i, j);
  a += b;
  ASSERT_TRUE(a == c);
}

TEST(OperatorTests, RandomSubEqualOperator) {
  int size = GetRandInt(1, 5);
  S21Matrix a(size, size), b(size, size), c(size, size);
  FillMatrix(a, GetRandDouble(-10, 10));
  FillMatrix(b, GetRandDouble(-10, 10));
  for (int i = 0; i < size; ++i)
    for (int j = 0; j < size; ++j) c(i, j) = a(i, j) - b(i, j);
  a -= b;
  ASSERT_TRUE(a == c);
}

TEST(OperatorTests, SubEqualOperator) {
  S21Matrix L(2, 2), O(2, 2), T(2, 2);
  L(0, 0) = 2.8;
  L(0, 1) = 7.3;
  L(1, 0) = 3.3;
  L(1, 1) = 8.1;
  O(0, 0) = 2.8;
  O(0, 1) = 7.3;
  O(1, 0) = 3.3;
  O(1, 1) = 8.1;
  T(0, 0) = 0.0;
  T(0, 1) = 0.0;
  T(1, 0) = 0.0;
  T(1, 1) = 0.0;
  L -= O;
  ASSERT_TRUE(L == T);
}

TEST(OperatorTests, MultiplyByNumber) {
  S21Matrix a(3, 3), b(3, 3);
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j) {
      a(i, j) = 1;
      b(i, j) = 1 * 2.5;
    }
  S21Matrix c = a * 2.5;
  ASSERT_TRUE(c == b);
}

TEST(OperatorTests, MultiplyByNumberEqual) {
  S21Matrix Y(3, 3), Z(3, 3);
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j) {
      Y(i, j) = 8;
      Z(i, j) = 8 * 2.2;
    }
  Y *= 2.2;
  ASSERT_TRUE(Y == Z);
}

TEST(OperatorTests, RandomMultiplyByNumber) {
  int size = GetRandInt(1, 5);
  S21Matrix a(size, size), b(size, size);
  FillMatrix(a, GetRandDouble(-10, 10));
  double factor = GetRandDouble(1, 5);
  for (int i = 0; i < size; ++i)
    for (int j = 0; j < size; ++j) b(i, j) = a(i, j) * factor;
  S21Matrix c = a * factor;
  ASSERT_TRUE(c == b);
}

TEST(OperatorTests, RandomMultiplyByNumberEqual) {
  int size = GetRandInt(1, 5);
  S21Matrix a(size, size), b(size, size);
  FillMatrix(a, GetRandDouble(-10, 10));
  double factor = GetRandDouble(1, 5);
  for (int i = 0; i < size; ++i)
    for (int j = 0; j < size; ++j) b(i, j) = a(i, j) * factor;
  a *= factor;
  ASSERT_TRUE(a == b);
}

TEST(OperatorTests, IndexAccess) {
  S21Matrix A(10, 10);
  A(5, 2) = 1498;
  ASSERT_EQ(A(5, 2), 1498);
}

TEST(OperatorTests, RandomIndexAccess) {
  int size = GetRandInt(5, 10);
  S21Matrix a(size, size);
  int i = GetRandInt(0, size - 1), j = GetRandInt(0, size - 1);
  double value = GetRandDouble(-10, 10);
  a(i, j) = value;
  ASSERT_EQ(a(i, j), value);
}

TEST(OperatorTests, IndexAccessInvalid) {
  S21Matrix a(2, 2);
  EXPECT_THROW(a(-1, 0), std::out_of_range);
  EXPECT_THROW(a(2, 0), std::out_of_range);
  EXPECT_THROW(a(0, -1), std::out_of_range);
  EXPECT_THROW(a(0, 2), std::out_of_range);
}

TEST(OperatorTests, ConstIndexAccess) {
  const S21Matrix a(2, 2);
  EXPECT_THROW(a(-1, 0), std::out_of_range);
  EXPECT_THROW(a(2, 0), std::out_of_range);
  EXPECT_THROW(a(0, -1), std::out_of_range);
  EXPECT_THROW(a(0, 2), std::out_of_range);
  S21Matrix b(2, 2);
  b(0, 0) = 1.0;
  const S21Matrix& cb = b;
  EXPECT_EQ(cb(0, 0), 1.0);
}

TEST(Operations, GetMatrixMinorInvalid) {
  S21Matrix a(2, 2);
  EXPECT_THROW(a.GetMatrixMinor(-1, 0), std::out_of_range);
  EXPECT_THROW(a.GetMatrixMinor(2, 0), std::out_of_range);
  EXPECT_THROW(a.GetMatrixMinor(0, -1), std::out_of_range);
  EXPECT_THROW(a.GetMatrixMinor(0, 2), std::out_of_range);
}

TEST(Operations, SumMatrixNullCheck) {
  S21Matrix a(2, 2);
  S21Matrix b(2, 2);
  a.FreeMemory();
  EXPECT_THROW(a.SumMatrix(b), std::logic_error);
}

TEST(Operations, SumMatrixSimple) {
  S21Matrix L(2, 2), B(2, 2), T(2, 2);
  L(0, 0) = 1;
  L(0, 1) = 2;
  L(1, 0) = 3;
  L(1, 1) = 4;
  B(0, 0) = 5;
  B(0, 1) = 6;
  B(1, 0) = 7;
  B(1, 1) = 8;
  T(0, 0) = 6;
  T(0, 1) = 8;
  T(1, 0) = 10;
  T(1, 1) = 12;
  L.SumMatrix(B);
  ASSERT_TRUE(L == T);
}

TEST(Operations, SumMatrixInvalid) {
  S21Matrix a(2, 2), b(3, 3);
  EXPECT_THROW(a.SumMatrix(b), std::invalid_argument);
}

TEST(Operations, SubMatrixSimple) {
  S21Matrix S(2, 2), b(2, 2), K(2, 2);
  S(0, 0) = 9;
  S(0, 1) = 8;
  S(1, 0) = 7;
  S(1, 1) = 6;
  b(0, 0) = 1;
  b(0, 1) = 2;
  b(1, 0) = 3;
  b(1, 1) = 4;
  K(0, 0) = 8;
  K(0, 1) = 6;
  K(1, 0) = 4;
  K(1, 1) = 2;
  S.SubMatrix(b);
  ASSERT_TRUE(S == K);
}

TEST(Operations, SubMatrixInvalid) {
  S21Matrix a(2, 2), b(3, 3);
  EXPECT_THROW(a.SubMatrix(b), std::invalid_argument);
}

TEST(Operations, MulByNumberSimple) {
  S21Matrix a(2, 2), b(2, 2);
  a(0, 0) = 1;
  a(0, 1) = 2;
  a(1, 0) = 3;
  a(1, 1) = 4;
  b(0, 0) = 2;
  b(0, 1) = 4;
  b(1, 0) = 6;
  b(1, 1) = 8;
  a.MulNumber(2);
  ASSERT_TRUE(a == b);
}

TEST(Operations, MulMatrixSimple) {
  S21Matrix a(2, 3), b(3, 2), c(2, 2);
  a(0, 0) = 1;
  a(0, 1) = 2;
  a(0, 2) = 3;
  a(1, 0) = 4;
  a(1, 1) = 5;
  a(1, 2) = 6;
  b(0, 0) = 7;
  b(0, 1) = 8;
  b(1, 0) = 9;
  b(1, 1) = 10;
  b(2, 0) = 11;
  b(2, 1) = 12;
  c(0, 0) = 58;
  c(0, 1) = 64;
  c(1, 0) = 139;
  c(1, 1) = 154;
  a.MulMatrix(b);
  ASSERT_TRUE(a == c);
}

TEST(Operations, MulMatrixInvalid) {
  S21Matrix a(2, 2), b(3, 3);
  EXPECT_THROW(a.MulMatrix(b), std::invalid_argument);
}

TEST(Operations, TransposeSimple) {
  S21Matrix a(2, 3);
  a(0, 0) = 1;
  a(0, 1) = 2;
  a(0, 2) = 3;
  a(1, 0) = 4;
  a(1, 1) = 5;
  a(1, 2) = 6;
  S21Matrix t = a.Transpose();
  EXPECT_EQ(t.get_rows(), 3);
  EXPECT_EQ(t.get_cols(), 2);
  EXPECT_EQ(t(0, 0), 1);
  EXPECT_EQ(t(1, 0), 2);
  EXPECT_EQ(t(0, 1), 4);
  EXPECT_EQ(t(1, 1), 5);
  EXPECT_EQ(t(2, 0), 3);
  EXPECT_EQ(t(2, 1), 6);
}

TEST(Operations, CalcComplementsSimple) {
  S21Matrix a(2, 2);
  a(0, 0) = 1;
  a(0, 1) = 2;
  a(1, 0) = 3;
  a(1, 1) = 4;
  S21Matrix c = a.CalcComplements();
  EXPECT_EQ(c(0, 0), 4);
  EXPECT_EQ(c(0, 1), -3);
  EXPECT_EQ(c(1, 0), -2);
  EXPECT_EQ(c(1, 1), 1);
}

TEST(Operations, CalcComplements1x1) {
  S21Matrix a(1, 1);
  a(0, 0) = 5;
  S21Matrix c = a.CalcComplements();
  EXPECT_EQ(c(0, 0), 1.0);
}

TEST(Operations, CalcComplementsInvalid) {
  S21Matrix a(2, 3);
  EXPECT_THROW(a.CalcComplements(), std::out_of_range);
}

TEST(Operations, DeterminantSimple) {
  S21Matrix a(2, 2);
  a(0, 0) = 1;
  a(0, 1) = 2;
  a(1, 0) = 3;
  a(1, 1) = 4;
  EXPECT_DOUBLE_EQ(a.Determinant(), -2.0);
}

TEST(Operations, Determinant1x1) {
  S21Matrix a(1, 1);
  a(0, 0) = 5;
  EXPECT_DOUBLE_EQ(a.Determinant(), 5.0);
}

TEST(Operations, Determinant3x3) {
  S21Matrix a(3, 3);
  a(0, 0) = 1;
  a(0, 1) = 2;
  a(0, 2) = 3;
  a(1, 0) = 0;
  a(1, 1) = 4;
  a(1, 2) = 5;
  a(2, 0) = 1;
  a(2, 1) = 0;
  a(2, 2) = 6;
  EXPECT_DOUBLE_EQ(a.Determinant(), 22.0);
}

TEST(Operations, DeterminantInvalid) {
  S21Matrix a(2, 3);
  EXPECT_THROW(a.Determinant(), std::out_of_range);
}

TEST(Operations, InverseMatrixSimple) {
  S21Matrix a(2, 2);
  a(0, 0) = 4;
  a(0, 1) = 7;
  a(1, 0) = 2;
  a(1, 1) = 6;
  S21Matrix inv = a.InverseMatrix();
  S21Matrix expected(2, 2);
  expected(0, 0) = 0.6;
  expected(0, 1) = -0.7;
  expected(1, 0) = -0.2;
  expected(1, 1) = 0.4;
  ASSERT_TRUE(inv.EqMatrix(expected));
}

TEST(Operations, InverseMatrixInvalid) {
  S21Matrix a(2, 2);
  a(0, 0) = 0;
  a(0, 1) = 0;
  a(1, 0) = 0;
  a(1, 1) = 0;
  EXPECT_THROW(a.InverseMatrix(), std::out_of_range);
}

TEST(Operations, AddEqualOperator) {
  S21Matrix a(2, 2), b(2, 2), c(2, 2);
  a(0, 0) = 1;
  a(0, 1) = 2;
  a(1, 0) = 3;
  a(1, 1) = 4;
  b(0, 0) = 5;
  b(0, 1) = 6;
  b(1, 0) = 7;
  b(1, 1) = 8;
  c(0, 0) = 6;
  c(0, 1) = 8;
  c(1, 0) = 10;
  c(1, 1) = 12;
  a += b;
  ASSERT_TRUE(a == c);
}

TEST(Operations, ScalarMulGlobal) {
  S21Matrix a(2, 2);
  a(0, 0) = 1;
  a(0, 1) = 2;
  a(1, 0) = 3;
  a(1, 1) = 4;
  S21Matrix b = 2.0 * a;
  S21Matrix expected(2, 2);
  expected(0, 0) = 2;
  expected(0, 1) = 4;
  expected(1, 0) = 6;
  expected(1, 1) = 8;
  ASSERT_TRUE(b == expected);
}

TEST(OperatorTests, RandomMultiplyWithNearEquality) {
  int rows = GetRandInt(1, 5), cols = GetRandInt(1, 5);
  int common = GetRandInt(1, 5);
  S21Matrix a(rows, common), b(common, cols);
  FillMatrix(a, GetRandDouble(-10, 10));
  FillMatrix(b, GetRandDouble(-10, 10));
  S21Matrix result = a * b;
  S21Matrix manual(rows, cols);
  for (int i = 0; i < rows; ++i)
    for (int j = 0; j < cols; ++j)
      for (int k = 0; k < common; ++k) manual(i, j) += a(i, k) * b(k, j);
  for (int i = 0; i < rows; ++i)
    for (int j = 0; j < cols; ++j)
      ASSERT_NEAR(result(i, j), manual(i, j), 1e-10);
}

TEST(Operations, RandomDeterminantWithNearEquality) {
  int size = GetRandInt(2, 5);
  S21Matrix a(size, size);
  FillMatrix(a, GetRandDouble(-10, 10));
  double det = a.Determinant();
  if (size == 2) {
    double manualDet = a(0, 0) * a(1, 1) - a(0, 1) * a(1, 0);
    ASSERT_NEAR(det, manualDet, 1e-10);
  } else if (size == 3) {
    double manualDet = a(0, 0) * (a(1, 1) * a(2, 2) - a(1, 2) * a(2, 1)) -
                       a(0, 1) * (a(1, 0) * a(2, 2) - a(1, 2) * a(2, 0)) +
                       a(0, 2) * (a(1, 0) * a(2, 1) - a(1, 1) * a(2, 0));
    ASSERT_NEAR(det, manualDet, 1e-10);
  }
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}