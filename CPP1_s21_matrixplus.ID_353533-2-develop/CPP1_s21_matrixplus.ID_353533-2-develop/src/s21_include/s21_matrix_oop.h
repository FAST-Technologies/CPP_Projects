#ifndef CPP1_S21_MATRIXPLUS_1_S21_MATRIX_OOP_H_
#define CPP1_S21_MATRIXPLUS_1_S21_MATRIX_OOP_H_

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <utility>

using namespace std;

class S21Matrix {
 public:
  S21Matrix() noexcept;
  explicit S21Matrix(int rows, int cols);
  S21Matrix(const S21Matrix& other);
  S21Matrix(S21Matrix&& other) noexcept;
  ~S21Matrix() noexcept;

  bool EqMatrix(const S21Matrix& other) const noexcept;
  void SumMatrix(const S21Matrix& other);
  void SubMatrix(const S21Matrix& other);
  void MulNumber(const double number) noexcept;
  void MulMatrix(const S21Matrix& other);
  S21Matrix Transpose() const;
  S21Matrix CalcComplements() const;
  double Determinant() const;
  S21Matrix InverseMatrix() const;
  S21Matrix GetMatrixMinor(const int row, const int column) const;
  void CheckMatrix(const S21Matrix& other) const;

  int get_rows() const noexcept;
  int get_cols() const noexcept;
  void set_rows(int rows);
  void set_cols(int cols);

  double& operator()(int row, int col) &;
  double& operator()(int row, int col) && = delete;
  const double& operator()(int row, int col) const&;
  const double& operator()(int row, int col) const&& = delete;
  bool operator==(const S21Matrix& other) const noexcept;

  S21Matrix operator+(const S21Matrix& other) const;
  S21Matrix& operator+=(const S21Matrix& other);
  S21Matrix operator-(const S21Matrix& other) const;
  S21Matrix& operator-=(const S21Matrix& other);
  S21Matrix operator*(double number) const noexcept;
  friend S21Matrix operator*(double number, const S21Matrix& matrix) noexcept;
  S21Matrix& operator*=(const double number);
  S21Matrix operator*(const S21Matrix& other) const;
  S21Matrix& operator*=(const S21Matrix& other);
  S21Matrix& operator=(const S21Matrix& other);
  S21Matrix& operator=(S21Matrix&& other) noexcept;

  void FreeMemory() noexcept;

  static constexpr double TOLERANCE = 1e-7;
  static constexpr int MATRIX_SIZE = 10;

 private:
  int rows_{0};
  int cols_{0};
  double** matrix_{nullptr};
  void allocator_memory();
  void CopyMatrix(const S21Matrix& other);
};

#endif  // CPP1_S21_MATRIXPLUS_1_S21_MATRIX_OOP_H_
