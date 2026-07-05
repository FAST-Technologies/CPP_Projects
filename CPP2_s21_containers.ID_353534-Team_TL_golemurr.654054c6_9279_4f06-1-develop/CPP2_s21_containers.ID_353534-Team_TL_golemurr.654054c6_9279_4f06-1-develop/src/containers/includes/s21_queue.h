#ifndef S21_QUEUE_H
#define S21_QUEUE_H

#include <initializer_list>

#include "s21_list.h"

namespace s21 {

template <typename T>
class queue {
 public:
  // Queue Member type
  using value_type = T;   // T the template parameter T
  using reference = T &;  // defines the type of the reference to an element
  using const_reference =
      const T &;  // defines the type of the constant reference
  using size_type =
      typename list<T>::size_type;  // defines the type of the container size

  // Queue Member functions
  queue();  // default constructor, creates empty queue
  queue(std::initializer_list<value_type> const
            &items);          // initializer list constructor
  queue(const queue &q);      // copy constructor
  queue(queue &&q) noexcept;  // move constructor
  ~queue();                   // destructor
  queue &operator=(
      queue &&q) noexcept;  // assignment operator overload for moving object

  // Queue Element access
  const_reference front() const;  // access the first element
  const_reference back() const;   // access the last element

  // Queue Capacity
  bool empty() const noexcept;      // checks whether the container is empty
  size_type size() const noexcept;  // returns the number of elements

  // Queue Modifiers
  void push(const_reference value);  // inserts element at the end
  void pop();                        // removes the first element
  void swap(queue &other);           // swaps the contents

  template <typename... Args>
  void insert_many_back(Args &&...args);

 private:
  list<T> cont_;  // internal container using list
};

}  // namespace s21

#include "../templates/s21_queue.tpp"
#endif  // S21_QUEUE_H