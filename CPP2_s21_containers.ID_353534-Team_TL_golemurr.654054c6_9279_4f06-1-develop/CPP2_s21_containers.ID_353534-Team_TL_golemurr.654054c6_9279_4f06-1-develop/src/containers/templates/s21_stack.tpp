#ifndef S21_STACK_TPP
#define S21_STACK_TPP

#include "../includes/s21_stack.h"

namespace s21 {

// Stack Member functions

// default constructor, creates empty stack
template <typename T>
stack<T>::stack() : cont_() {}

// initializer list constructor, creates stack initialized using
template <typename T>
stack<T>::stack(std::initializer_list<value_type> const &items)
    : cont_(items) {}

// copy constructor, creates a copy of another stack
template <typename T>
stack<T>::stack(const stack &s) : cont_(s.cont_) {}

// move constructor, transfers resources from another stack
template <typename T>
stack<T>::stack(stack &&s) noexcept : cont_(std::move(s.cont_)) {}

// destructor, clears the stack
template <typename T>
stack<T>::~stack() {}

// assignment operator overload for moving object
template <typename T>
stack<T> &stack<T>::operator=(stack &&s) noexcept {
  if (this != &s) {
    cont_ = std::move(s.cont_);
  }
  return *this;
}

// Stack Element access

// accesses the top element
template <typename T>
typename stack<T>::const_reference stack<T>::top() const {
  return cont_.back();
}

// Stack Capacity

// checks whether the container is empty
template <typename T>
bool stack<T>::empty() const noexcept {
  return cont_.empty();
}

// returns the number of elements
template <typename T>
typename stack<T>::size_type stack<T>::size() const noexcept {
  return cont_.size();
}

// Stack Modifiers

// inserts element at the top
template <typename T>
void stack<T>::push(const_reference value) {
  cont_.push_back(value);
}

// removes the top element
template <typename T>
void stack<T>::pop() {
  cont_.pop_back();
}

// swaps the contents
template <typename T>
void stack<T>::swap(stack &other) {
  cont_.swap(other.cont_);
}

template <typename T>
template <typename... Args>
void stack<T>::insert_many_back(Args &&...args) {
  (push(std::forward<Args>(args)), ...);
}

}  // namespace s21

#endif  // S21_STACK_TPP