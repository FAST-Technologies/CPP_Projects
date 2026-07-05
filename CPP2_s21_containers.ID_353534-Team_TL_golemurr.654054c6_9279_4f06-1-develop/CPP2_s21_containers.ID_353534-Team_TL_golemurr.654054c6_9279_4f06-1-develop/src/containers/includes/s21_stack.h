#ifndef S21_STACK_H
#define S21_STACK_H

#include <initializer_list>

#include "s21_list.h"

namespace s21 {

template <typename T>
class stack {
 public:
  // Stack Member type
  using value_type = T;   // T the template parameter T
  using reference = T &;  // defines the type of the reference to an element
  using const_reference =
      const T &;  // defines the type of the constant reference
  using size_type =
      typename list<T>::size_type;  // defines the type of the container size

  // Stack Member functions
  stack();  // default constructor, creates empty stack
  stack(std::initializer_list<value_type> const
            &items);          // initializer list constructor
  stack(const stack &s);      // copy constructor
  stack(stack &&s) noexcept;  // move constructor
  ~stack();                   // destructor
  stack &operator=(
      stack &&s) noexcept;  // assignment operator overload for moving object

  // Stack Element access
  const_reference top() const;  // accesses the top element

  // Stack Capacity
  bool empty() const noexcept;      // checks whether the container is empty
  size_type size() const noexcept;  // returns the number of elements

  // Stack Modifiers
  void push(const_reference value);  // inserts element at the top
  void pop();                        // removes the top element
  void swap(stack &other);           // swaps the contents

  template <typename... Args>
  void insert_many_back(Args &&...args);

 private:
  list<T> cont_;  // internal container using list
};

}  // namespace s21

#include "../templates/s21_stack.tpp"
#endif  // S21_STACK_H