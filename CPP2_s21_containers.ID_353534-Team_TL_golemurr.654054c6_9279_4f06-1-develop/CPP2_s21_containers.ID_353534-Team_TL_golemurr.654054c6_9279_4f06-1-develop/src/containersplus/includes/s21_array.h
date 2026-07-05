#ifndef S21_CONTAINERSPLUS_ARRAY_H_
#define S21_CONTAINERSPLUS_ARRAY_H_

namespace s21 {
template <typename T, size_t N>
class array {
 private:
  // Member types
  using value_type = T;
  using reference = T &;
  using const_reference = const T &;
  using iterator = T *;
  using const_iterator = const T *;
  using size_type = size_t;
  T arr[N > 0 ? N : 1]{};

 public:
  // Constructors and destructor
  array();
  explicit array(std::initializer_list<value_type> const &items);
  explicit array(const array<T, N> &a);
  explicit array(array<T, N> &&a);
  ~array();
  array<T, N> &operator=(array &&a);

  // Element access
  reference at(size_type pos);
  reference operator[](size_type pos);
  const_reference operator[](size_type pos) const;
  const_reference front();
  const_reference back();
  const_reference front() const;
  const_reference back() const;
  iterator data();

  // Iterators
  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;

  // Capacity
  bool empty();
  size_type size();
  size_type max_size();

  // Modifiers
  void swap(array<T, N> &other);
  void fill(const_reference value);
};
#include "../templates/s21_array.tpp"
}  // namespace s21

#endif  // S21_CONTAINERSPLUS_ARRAY_H_