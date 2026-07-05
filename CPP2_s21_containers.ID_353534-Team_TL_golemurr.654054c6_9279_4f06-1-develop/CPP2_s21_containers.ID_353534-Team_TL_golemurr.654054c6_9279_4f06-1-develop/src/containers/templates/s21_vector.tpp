#ifndef S21_VECTOR_TPP
#define S21_VECTOR_TPP

#include <algorithm>
#include <limits>

#include "../includes/s21_vector.h"

namespace s21 {

// Vector Member functions

// default constructor, creates empty vector
template <typename T>
vector<T>::vector() : data_(nullptr), size_(0), capacity_(0) {}

// parameterized constructor, creates the vector of size n
template <typename T>
vector<T>::vector(size_type n)
    : data_(new value_type[n]()), size_(n), capacity_(n) {}

// initializer list constructor
template <typename T>
vector<T>::vector(std::initializer_list<value_type> const &items)
    : vector(items.size()) {
  std::copy(items.begin(), items.end(), data_);
}

// copy constructor
template <typename T>
vector<T>::vector(const vector &v)
    : data_(new value_type[v.capacity_]),
      size_(v.size_),
      capacity_(v.capacity_) {
  std::copy(v.data_, v.data_ + v.size_, data_);
}

// move constructor
template <typename T>
vector<T>::vector(vector &&v) noexcept
    : data_(v.data_), size_(v.size_), capacity_(v.capacity_) {
  v.data_ = nullptr;
  v.size_ = 0;
  v.capacity_ = 0;
}

// destructor
template <typename T>
vector<T>::~vector() {
  delete[] data_;
}

// assignment operator overload for moving object
template <typename T>
vector<T> &vector<T>::operator=(vector &&v) noexcept {
  if (this != &v) {
    delete[] data_;
    data_ = v.data_;
    size_ = v.size_;
    capacity_ = v.capacity_;
    v.data_ = nullptr;
    v.size_ = 0;
    v.capacity_ = 0;
  }
  return *this;
}

// Vector Element access

// access specified element with bounds checking
template <typename T>
typename vector<T>::reference vector<T>::at(size_type pos) {
  if (pos >= size_) throw std::out_of_range("Index out of range");
  return data_[pos];
}

// access specified element
template <typename T>
typename vector<T>::reference vector<T>::operator[](size_type pos) {
  return data_[pos];
}

// access the first element
template <typename T>
typename vector<T>::const_reference vector<T>::front() const {
  return data_[0];
}

// access the last element
template <typename T>
typename vector<T>::const_reference vector<T>::back() const {
  return data_[size_ - 1];
}

// direct access to the underlying array
template <typename T>
T *vector<T>::data() noexcept {
  return data_;
}

// Vector Iterators

// returns an iterator to the beginning
template <typename T>
typename vector<T>::iterator vector<T>::begin() noexcept {
  return data_;
}

// returns an iterator to the end
template <typename T>
typename vector<T>::iterator vector<T>::end() noexcept {
  return data_ + size_;
}

template <typename T>
typename vector<T>::const_iterator vector<T>::cbegin() const noexcept {
  return data_;
}

template <typename T>
typename vector<T>::const_iterator vector<T>::cend() const noexcept {
  return data_ + size_;
}

template <typename T>
typename vector<T>::const_iterator vector<T>::begin() const noexcept {
  return data_;
}

template <typename T>
typename vector<T>::const_iterator vector<T>::end() const noexcept {
  return data_ + size_;
}

// Vector Capacity

// checks whether the container is empty
template <typename T>
bool vector<T>::empty() const noexcept {
  return size_ == 0;
}

// returns the number of elements
template <typename T>
typename vector<T>::size_type vector<T>::size() const noexcept {
  return size_;
}

// returns the maximum possible number of elements
template <typename T>
typename vector<T>::size_type vector<T>::max_size() const noexcept {
  return std::numeric_limits<size_type>::max() / (2 * sizeof(value_type));
}

// allocate storage of size elements and copies current array elements to a
// newely allocated array
template <typename T>
void vector<T>::reserve(size_type new_cap) {
  if (new_cap > capacity_) {
    value_type *new_data = new value_type[new_cap]();
    std::copy(data_, data_ + size_, new_data);
    delete[] data_;
    data_ = new_data;
    capacity_ = new_cap;
  }
}

// returns the number of elements that can be held in currently allocated
// storage
template <typename T>
typename vector<T>::size_type vector<T>::capacity() const noexcept {
  return capacity_;
}

// reduces memory usage by freeing unused memory
template <typename T>
void vector<T>::shrink_to_fit() {
  if (size_ < capacity_) {
    value_type *new_data = new value_type[size_];
    std::copy(data_, data_ + size_, new_data);
    delete[] data_;
    data_ = new_data;
    capacity_ = size_;
  }
}

// Vector Modifiers

// clears the contents
template <typename T>
void vector<T>::clear() {
  size_ = 0;
}

// inserts elements into concrete pos and returns the iterator that points to
// the new element
template <typename T>
typename vector<T>::iterator vector<T>::insert(const iterator &pos,
                                               const_reference value) {
  size_type index = pos - data_;
  if (size_ >= capacity_) reserve(capacity_ == 0 ? 1 : capacity_ * 2);
  for (size_type i = size_; i > index; --i) {
    data_[i] = data_[i - 1];
  }
  data_[index] = value;
  ++size_;
  return data_ + index;
}

// erases element at pos
template <typename T>
void vector<T>::erase(const iterator &pos) {
  size_type index = pos - data_;
  for (size_type i = index; i < size_ - 1; ++i) {
    data_[i] = data_[i + 1];
  }
  --size_;
}

// adds an element to the end
template <typename T>
void vector<T>::push_back(const_reference value) {
  if (size_ >= capacity_) reserve(capacity_ == 0 ? 1 : capacity_ * 2);
  data_[size_++] = value;
}

// removes the last element
template <typename T>
void vector<T>::pop_back() {
  if (size_ > 0) --size_;
}

// swaps the contents
template <typename T>
void vector<T>::swap(vector &other) noexcept {
  std::swap(data_, other.data_);
  std::swap(size_, other.size_);
  std::swap(capacity_, other.capacity_);
}

// insert_many
template <typename T>
template <typename... Args>
typename vector<T>::iterator vector<T>::insert_many(const_iterator pos,
                                                    Args &&...args) {
  size_type index = pos - data_;
  auto array = {T(std::forward<Args>(args))...};

  reserve(size_ + array.size());

  size_type count = array.size();

  for (size_type i = size_; i > index; --i) {
    data_[i + count - 1] = data_[i - 1];
  }

  size_type shift = 0;
  for (auto it = array.begin(); it != array.end(); ++it, ++shift) {
    data_[index + shift] = *it;
  }

  size_ += count;

  return begin() + index;
}

template <typename T>
template <typename... Args>
void vector<T>::insert_many_back(Args &&...args) {
  const size_type new_elements = sizeof...(Args);

  if (size_ + new_elements > capacity_) {
    reserve(size_ + new_elements);
  }

  (push_back(std::forward<Args>(args)), ...);
}

}  // namespace s21

#endif  // S21_VECTOR_TPP