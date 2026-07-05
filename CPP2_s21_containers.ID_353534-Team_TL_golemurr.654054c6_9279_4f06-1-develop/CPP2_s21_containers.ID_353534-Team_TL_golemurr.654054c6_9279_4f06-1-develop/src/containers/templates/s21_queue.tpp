#ifndef S21_QUEUE_TPP
#define S21_QUEUE_TPP

#include "../includes/s21_queue.h"

namespace s21 {

// Queue Member functions

// default constructor, creates empty queue
template <typename T>
queue<T>::queue() : cont_() {}

// initializer list constructor, creates queue initialized using
// std::initializer_list
template <typename T>
queue<T>::queue(std::initializer_list<value_type> const &items)
    : cont_(items) {}

// copy constructor, creates a copy of another queue
template <typename T>
queue<T>::queue(const queue &q) : cont_(q.cont_) {}

// move constructor, transfers resources from another queue
template <typename T>
queue<T>::queue(queue &&q) noexcept : cont_(std::move(q.cont_)) {}

// destructor, clears the queue
template <typename T>
queue<T>::~queue() {}

// assignment operator overload for moving object
template <typename T>
queue<T> &queue<T>::operator=(queue &&q) noexcept {
  if (this != &q) {
    cont_ = std::move(q.cont_);
  }
  return *this;
}

// Queue Element access

// access the first element
template <typename T>
typename queue<T>::const_reference queue<T>::front() const {
  return cont_.front();
}

// access the last element
template <typename T>
typename queue<T>::const_reference queue<T>::back() const {
  return cont_.back();
}

// Queue Capacity

// checks whether the container is empty
template <typename T>
bool queue<T>::empty() const noexcept {
  return cont_.empty();
}

// returns the number of elements
template <typename T>
typename queue<T>::size_type queue<T>::size() const noexcept {
  return cont_.size();
}

// Queue Modifiers

// inserts element at the end
template <typename T>
void queue<T>::push(const_reference value) {
  cont_.push_back(value);
}

// removes the first element
template <typename T>
void queue<T>::pop() {
  cont_.pop_front();
}

// swaps the contents
template <typename T>
void queue<T>::swap(queue &other) {
  cont_.swap(other.cont_);
}

template <typename T>
template <typename... Args>
void queue<T>::insert_many_back(Args &&...args) {
  (push(std::forward<Args>(args)), ...);
}

}  // namespace s21

#endif  // S21_QUEUE_TPP