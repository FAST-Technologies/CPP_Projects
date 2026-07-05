#ifndef S21_CONTAINERSPLUS_ARRAY_TPP_
#define S21_CONTAINERSPLUS_ARRAY_TPP_

template <typename T, size_t N>
array<T, N>::array() {}

template <typename T, size_t N>
array<T, N>::array(std::initializer_list<value_type> const &items) {
  if (items.size() > N) {
    throw std::length_error("Cannot create array larger than its size()");
  }

  size_t i = 0;
  auto it = items.begin();
  while (it != items.end()) {
    arr[i] = *it;
    ++i;
    ++it;
  }
}

template <typename T, size_t N>
array<T, N>::array(const array<T, N> &a) {
  size_t i = 0;
  while (i < N) {
    arr[i] = a.arr[i];
    ++i;
  }
}

template <typename T, size_t N>
array<T, N>::array(array<T, N> &&a) {
  for (size_t i = 0; i < N; ++i) {
    arr[i] = std::move(a.arr[i]);
  }
}

template <typename T, size_t N>
array<T, N>::~array() {}

template <typename T, size_t N>
array<T, N> &array<T, N>::operator=(array<T, N> &&a) {
  if (this != &a) {
    for (size_t i = 0; i < N; ++i) {
      arr[i] = std::move(a.arr[i]);
    }
  }
  return *this;
}

template <typename T, size_t N>
typename array<T, N>::reference array<T, N>::at(size_type pos) {
  if (pos >= N) {
    throw std::out_of_range("Error: Index out of range");
  }
  return arr[pos];
}

template <typename T, size_t N>
typename array<T, N>::reference array<T, N>::operator[](size_type pos) {
  return arr[pos];
}

template <typename T, size_t N>
typename array<T, N>::const_reference array<T, N>::operator[](
    size_type pos) const {
  return arr[pos];
}

template <typename T, size_t N>
typename array<T, N>::const_reference array<T, N>::front() {
  return arr[0];
}

template <typename T, size_t N>
typename array<T, N>::const_reference array<T, N>::back() {
  return arr[N - 1];
}

template <typename T, size_t N>
typename array<T, N>::const_reference array<T, N>::front() const {
  return arr[0];
}

template <typename T, size_t N>
typename array<T, N>::const_reference array<T, N>::back() const {
  return arr[N - 1];
}

template <typename T, size_t N>
typename array<T, N>::iterator array<T, N>::data() {
  return arr;
}

template <typename T, size_t N>
typename array<T, N>::iterator array<T, N>::begin() {
  return arr;
}

template <typename T, size_t N>
typename array<T, N>::iterator array<T, N>::end() {
  return arr + N;
}

template <typename T, size_t N>
typename array<T, N>::const_iterator array<T, N>::begin() const {
  return arr;
}

template <typename T, size_t N>
typename array<T, N>::const_iterator array<T, N>::end() const {
  return arr + N;
}

template <typename T, size_t N>
bool array<T, N>::empty() {
  return N == 0;
}

template <typename T, size_t N>
size_t array<T, N>::size() {
  return N;
}

template <typename T, size_t N>
size_t array<T, N>::max_size() {
  return N;
}

template <typename T, size_t N>
void array<T, N>::swap(array<T, N> &other) {
  if (other.size() != N) throw std::out_of_range("Error: Index out of range");
  size_t i = 0;
  while (i < N) {
    std::swap(arr[i], other.arr[i]);
    ++i;
  }
}

template <typename T, size_t N>
void array<T, N>::fill(const_reference value) {
  size_t i = 0;
  while (i < size()) {
    arr[i] = value;
    ++i;
  }
}

#endif  // S21_CONTAINERSPLUS_ARRAY_TPP_