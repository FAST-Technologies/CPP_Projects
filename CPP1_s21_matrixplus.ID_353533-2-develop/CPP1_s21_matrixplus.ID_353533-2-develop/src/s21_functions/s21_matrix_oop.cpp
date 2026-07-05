#include "../s21_include/s21_matrix_oop.h"

S21Matrix::S21Matrix() noexcept
    : S21Matrix(S21Matrix::MATRIX_SIZE, S21Matrix::MATRIX_SIZE) {}

S21Matrix::S21Matrix(int rows, int cols) {
  if (rows <= 0 || cols <= 0) {
    throw std::invalid_argument("Matrix dimensions must be positive!");
  }
  rows_ = rows;
  cols_ = cols;
  allocator_memory();
}

S21Matrix::S21Matrix(const S21Matrix &other) {
  rows_ = other.rows_;
  cols_ = other.cols_;
  allocator_memory();
  for (int i = 0; i < rows_; ++i) {
    for (int j = 0; j < cols_; ++j) {
      matrix_[i][j] = other.matrix_[i][j];
    }
  }
}

S21Matrix::S21Matrix(S21Matrix &&other) noexcept {
  rows_ = other.rows_;
  cols_ = other.cols_;
  matrix_ = other.matrix_;
  other.rows_ = 0;
  other.cols_ = 0;
  other.matrix_ = nullptr;
}

S21Matrix::~S21Matrix() noexcept { FreeMemory(); }

bool S21Matrix::EqMatrix(const S21Matrix &other) const noexcept {
  if (matrix_ == nullptr || other.matrix_ == nullptr) return false;
  if (cols_ != other.cols_ || rows_ != other.rows_) return false;
  for (int i = 0; i < rows_; ++i) {
    for (int j = 0; j < cols_; ++j) {
      if (std::fabs(matrix_[i][j] - other.matrix_[i][j]) > TOLERANCE) {
        return false;
      }
    }
  }
  return true;
}

void S21Matrix::SumMatrix(const S21Matrix &other) {
  CheckMatrix(other);
  for (int i = 0; i < rows_; ++i) {
    for (int j = 0; j < cols_; ++j) {
      matrix_[i][j] += other.matrix_[i][j];
    }
  }
}

void S21Matrix::SubMatrix(const S21Matrix &other) {
  CheckMatrix(other);
  for (int i = 0; i < rows_; ++i) {
    for (int j = 0; j < cols_; ++j) {
      matrix_[i][j] -= other.matrix_[i][j];
    }
  }
}

void S21Matrix::MulNumber(const double number) noexcept {
  for (int i = 0; i < rows_; ++i) {
    for (int j = 0; j < cols_; ++j) {
      matrix_[i][j] *= number;
    }
  }
}

void S21Matrix::MulMatrix(const S21Matrix &other) {
  if (cols_ != other.rows_ || matrix_ == nullptr || other.matrix_ == nullptr) {
    throw std::invalid_argument(
        "Matrix dimensions are incompatible for multiplication or memory is "
        "unallocated!");
  }
  S21Matrix temp_matrix(rows_, other.cols_);
  for (int i = 0; i < rows_; ++i) {
    for (int j = 0; j < other.cols_; ++j) {
      double sum = 0.0;
      for (int k = 0; k < cols_; ++k) {
        sum += matrix_[i][k] * other.matrix_[k][j];
      }
      temp_matrix.matrix_[i][j] = sum;
    }
  }
  *this = std::move(temp_matrix);
}

S21Matrix S21Matrix::Transpose() const {
  S21Matrix temp_matrix(cols_, rows_);
  for (int i = 0; i < rows_; ++i) {
    for (int j = 0; j < cols_; ++j) {
      temp_matrix.matrix_[j][i] = matrix_[i][j];
    }
  }
  return temp_matrix;
}

S21Matrix S21Matrix::CalcComplements() const {
  if (matrix_ == nullptr || rows_ != cols_) {
    throw std::out_of_range(
        "Matrix must be square and allocated to compute complements!");
  }
  S21Matrix temp_matrix(rows_, cols_);
  if (rows_ == 1) {
    temp_matrix.matrix_[0][0] = 1.0;
  } else {
    for (int i = 0; i < rows_; ++i) {
      for (int j = 0; j < cols_; ++j) {
        S21Matrix minor = GetMatrixMinor(i, j);
        temp_matrix.matrix_[i][j] = std::pow(-1.0, i + j) * minor.Determinant();
      }
    }
  }
  return temp_matrix;
}

double S21Matrix::Determinant() const {
  if (matrix_ == nullptr || rows_ != cols_) {
    throw std::out_of_range(
        "Matrix must be square and allocated to compute determinant!");
  }

  double determ = 0.0;
  if (rows_ == 1) {
    determ = matrix_[0][0];
  } else if (rows_ == 2) {
    determ = matrix_[0][0] * matrix_[1][1] - matrix_[0][1] * matrix_[1][0];
  } else {
    for (int i = 0; i < cols_; ++i) {
      if (fabs(matrix_[0][i]) > TOLERANCE) {
        S21Matrix minor = GetMatrixMinor(0, i);
        determ += minor.Determinant() * std::pow(-1.0, i) * matrix_[0][i];
      }
    }
  }
  return determ;
}

S21Matrix S21Matrix::InverseMatrix() const {
  double determ = Determinant();
  if (std::fabs(determ) < TOLERANCE) {
    throw std::out_of_range("Determinant is too close to zero for inversion!");
  }
  S21Matrix complements = CalcComplements();
  S21Matrix transposed = complements.Transpose();
  transposed.MulNumber(1.0 / determ);
  return transposed;
}

S21Matrix S21Matrix::GetMatrixMinor(const int row, const int col) const {
  if (matrix_ == nullptr || row < 0 || row >= rows_ || col < 0 ||
      col >= cols_) {
    throw std::out_of_range(
        "Invalid row or column index for minor extraction!");
  }
  S21Matrix matrix_minor(rows_ - 1, cols_ - 1);
  for (int i = 0, minor_i = 0; i < rows_; ++i) {
    if (i == row) continue;
    int minor_j = 0;
    for (int j = 0; j < cols_; ++j) {
      if (j == col) continue;
      matrix_minor.matrix_[minor_i][minor_j] = matrix_[i][j];
      ++minor_j;
    }
    ++minor_i;
  }
  return matrix_minor;
}

void S21Matrix::CheckMatrix(const S21Matrix &other) const {
  if (matrix_ == nullptr || other.matrix_ == nullptr)
    throw std::logic_error("Warning: matrix memory is not allocated properly!");
  if (cols_ != other.cols_ || rows_ != other.rows_)
    throw std::invalid_argument("Warning: matrix dimensions have a mismatch!");
}

int S21Matrix::get_rows() const noexcept { return rows_; }
int S21Matrix::get_cols() const noexcept { return cols_; }

void S21Matrix::set_rows(int rows) {
  if (rows <= 0) {
    throw std::invalid_argument("Rows must be positive!");
  }
  if (rows != rows_) {
    S21Matrix temp_matrix(rows, cols_);
    int min_rows = std::min(rows_, rows);
    for (int i = 0; i < min_rows; ++i) {
      for (int j = 0; j < cols_; ++j) {
        temp_matrix.matrix_[i][j] = matrix_[i][j];
      }
    }
    *this = std::move(temp_matrix);
  }
}

void S21Matrix::set_cols(int cols) {
  if (cols > 0 && cols != cols_) {
    S21Matrix temp_matrix(rows_, cols);
    int min_cols = std::min(cols_, cols);
    for (int i = 0; i < rows_; ++i) {
      for (int j = 0; j < min_cols; ++j) {
        temp_matrix.matrix_[i][j] = matrix_[i][j];
      }
    }
    *this = std::move(temp_matrix);
  }
}

void S21Matrix::allocator_memory() {
  matrix_ = new double *[rows_]();
  for (int i = 0; i < rows_; ++i) {
    matrix_[i] = new double[cols_]();
    if (matrix_[i] == nullptr) throw std::bad_alloc();
  }
}

void S21Matrix::FreeMemory() noexcept {
  if (matrix_ != nullptr) {
    for (int i = 0; i < rows_; ++i) {
      delete[] matrix_[i];
    }
    delete[] matrix_;
  }
  matrix_ = nullptr;
  rows_ = 0;
  cols_ = 0;
}

void S21Matrix::CopyMatrix(const S21Matrix &other) {
  rows_ = other.rows_;
  cols_ = other.cols_;
  allocator_memory();
  for (int i = 0; i < rows_; ++i) {
    for (int j = 0; j < cols_; ++j) {
      matrix_[i][j] = other.matrix_[i][j];
    }
  }
}

double &S21Matrix::operator()(int row, int col) & {
  if (row < 0 || col < 0 || row >= rows_ || col >= cols_) {
    throw std::out_of_range("Index out of range!");
  }
  return matrix_[row][col];
}

const double &S21Matrix::operator()(int row, int col) const & {
  if (row < 0 || col < 0 || row >= rows_ || col >= cols_) {
    throw std::out_of_range("Index out of range!");
  }
  return matrix_[row][col];
}

bool S21Matrix::operator==(const S21Matrix &other) const noexcept {
  return this->EqMatrix(other);
}

S21Matrix S21Matrix::operator+(const S21Matrix &other) const {
  S21Matrix result(*this);
  result.SumMatrix(other);
  return result;
}

S21Matrix S21Matrix::operator-(const S21Matrix &other) const {
  S21Matrix result(*this);
  result.SubMatrix(other);
  return result;
}

S21Matrix S21Matrix::operator*(const S21Matrix &other) const {
  S21Matrix result(*this);
  result.MulMatrix(other);
  return result;
}

S21Matrix S21Matrix::operator*(const double number) const noexcept {
  S21Matrix result(*this);
  result.MulNumber(number);
  return result;
}

S21Matrix &S21Matrix::operator=(const S21Matrix &other) {
  if (this != &other) {
    FreeMemory();
    CopyMatrix(other);
  }
  return *this;
}

S21Matrix &S21Matrix::operator=(S21Matrix &&other) noexcept {
  if (this != &other) {
    FreeMemory();
    rows_ = other.rows_;
    cols_ = other.cols_;
    matrix_ = other.matrix_;
    other.rows_ = 0;
    other.cols_ = 0;
    other.matrix_ = nullptr;
  }
  return *this;
}

S21Matrix &S21Matrix::operator+=(const S21Matrix &other) {
  SumMatrix(other);
  return *this;
}

S21Matrix &S21Matrix::operator-=(const S21Matrix &other) {
  SubMatrix(other);
  return *this;
}

S21Matrix &S21Matrix::operator*=(const S21Matrix &other) {
  MulMatrix(other);
  return *this;
}

S21Matrix &S21Matrix::operator*=(const double number) {
  MulNumber(number);
  return *this;
}

S21Matrix operator*(double number, const S21Matrix &matrix) noexcept {
  return matrix * number;
}