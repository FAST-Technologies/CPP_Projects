#ifndef S21_VECTOR_H
#define S21_VECTOR_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>

namespace s21 {

template <typename T>
class vector {
 public:
  // Vector Member type
  using value_type =
      T;  // T defines the type of an element (T is template parameter)
  using reference = T &;  // T & defines the type of the reference to an element
  using const_reference =
      const T &;         // const T & defines the type of the constant reference
  using iterator = T *;  // defines the type for iterating through the container
  using const_iterator = const T
      *;  // defines the constant type for iterating through the container
  using size_type = std::size_t;  // defines the type of the container size

  value_type *data_;
  size_type size_;
  size_type capacity_;

  // Vector Member functions
  vector();  // default constructor, creates empty vector
  vector(
      size_type n);  // parameterized constructor, creates the vector of size n
  vector(std::initializer_list<value_type> const
             &items);           // initializer list constructor
  vector(const vector &v);      // copy constructor
  vector(vector &&v) noexcept;  // move constructor
  ~vector();                    // destructor
  vector &operator=(
      vector &&v) noexcept;  // assignment operator overload for moving object

  // Vector Element access
  reference at(size_type pos);  // access specified element with bounds checking
  reference operator[](size_type pos);  // access specified element
  const_reference front() const;        // access the first element
  const_reference back() const;         // access the last element
  T *data() noexcept;                   // direct access to the underlying array

  // Vector Iterators
  iterator begin() noexcept;  // returns an iterator to the beginning
  iterator end() noexcept;    // returns an iterator to the end
  const_iterator cbegin() const noexcept;
  const_iterator cend() const noexcept;
  const_iterator begin() const noexcept;
  const_iterator end() const noexcept;

  // Vector Capacity
  bool empty() const noexcept;      // checks whether the container is empty
  size_type size() const noexcept;  // returns the number of elements
  size_type max_size()
      const noexcept;  // returns the maximum possible number of elements
  void reserve(
      size_type size);  // allocate storage of size elements and copies current
  size_type capacity()
      const noexcept;    // returns the number of elements that can be held in
  void shrink_to_fit();  // reduces memory usage by freeing unused memory

  // Vector Modifiers
  void clear();  // clears the contents
  iterator insert(
      const iterator &pos,
      const_reference value);  // inserts elements into concrete pos and returns
  void erase(const iterator &pos);        // erases element at pos
  void push_back(const_reference value);  // adds an element to the end
  void pop_back();                        // removes the last element
  void swap(vector &other) noexcept;      // swaps the contents

  template <typename... Args>
  iterator insert_many(const_iterator pos, Args &&...args);

  template <typename... Args>
  void insert_many_back(Args &&...args);
};

}  // namespace s21

#include "../templates/s21_vector.tpp"
#endif  // S21_VECTOR_H