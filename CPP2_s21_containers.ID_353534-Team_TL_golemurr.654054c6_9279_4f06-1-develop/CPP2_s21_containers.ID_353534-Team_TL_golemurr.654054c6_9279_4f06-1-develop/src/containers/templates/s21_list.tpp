#ifndef S21_LIST_TPP
#define S21_LIST_TPP

#include <limits>

#include "../includes/s21_list.h"

namespace s21 {

// List Functions

// default constructor, creates empty list
template <typename T>
list<T>::list() : head_(nullptr), tail_(nullptr), size_(0) {}

// parameterized constructor, creates the list of size n
template <typename T>
list<T>::list(size_type n) : list() {
  for (size_type i = 0; i < n; ++i) {
    push_back(T());
  }
}

// initializer list constructor, creates list initialized using
// std::initializer_list
template <typename T>
list<T>::list(std::initializer_list<value_type> const &items) : list() {
  for (const auto &item : items) {
    push_back(item);
  }
}

// copy constructor, creates a copy of another list
template <typename T>
list<T>::list(const list &l) : list() {
  for (auto it = l.begin(); it != l.end(); ++it) {
    push_back(*it);
  }
}

// move constructor, transfers resources from another list
template <typename T>
list<T>::list(list &&l) : head_(l.head_), tail_(l.tail_), size_(l.size_) {
  l.head_ = nullptr;
  l.tail_ = nullptr;
  l.size_ = 0;
}

// destructor, clears the list memory
template <typename T>
list<T>::~list() {
  clear();
}

// assignment operator overload for moving object
template <typename T>
list<T> &list<T>::operator=(list &&other) {
  if (this != &other) {
    clear();
    head_ = other.head_;
    tail_ = other.tail_;
    size_ = other.size_;
    other.head_ = nullptr;
    other.tail_ = nullptr;
    other.size_ = 0;
  }
  return *this;
}

// List Element access

// access the first element
template <typename T>
typename list<T>::const_reference list<T>::front() const {
  return head_->data;
}

// access the last element
template <typename T>
typename list<T>::const_reference list<T>::back() const {
  return tail_->data;
}

// List Iterators

// returns an iterator to the beginning
template <typename T>
typename list<T>::iterator list<T>::begin() {
  return iterator(head_);
}

// returns an iterator to the end
template <typename T>
typename list<T>::iterator list<T>::end() {
  return iterator(nullptr);
}

// returns a constant iterator to the beginning
template <typename T>
typename list<T>::const_iterator list<T>::begin() const noexcept {
  return iterator(head_);
}

// returns a constant iterator to the end
template <typename T>
typename list<T>::const_iterator list<T>::end() const noexcept {
  return iterator(nullptr);
}

template <typename T>
typename list<T>::const_iterator list<T>::cbegin() const noexcept {
  return iterator(head_);
}

template <typename T>
typename list<T>::const_iterator list<T>::cend() const noexcept {
  return iterator(nullptr);
}

// List Capacity

// checks whether the container is empty
template <typename T>
bool list<T>::empty() const {
  return size_ == 0;
}

// returns the number of elements
template <typename T>
typename list<T>::size_type list<T>::size() const {
  return static_cast<size_type>(size_);
}

// returns the maximum possible number of elements
template <typename T>
typename list<T>::size_type list<T>::max_size() const {
  return std::numeric_limits<size_type>::max() / (2 * sizeof(Node));
}

// List Modifiers

// clears the contents
template <typename T>
void list<T>::clear() {
  while (!empty()) {
    pop_front();
  }
}

// inserts element into concrete pos and returns the iterator that points to the
// new element
template <typename T>
typename list<T>::iterator list<T>::insert(iterator pos,
                                           const_reference value) {
  Node *current = pos.ptr_;
  Node *new_node = new Node(value);

  if (!current) {
    push_back(value);
    delete new_node;
    return iterator(tail_);
  } else if (!current->prev) {
    push_front(value);
    delete new_node;
    return iterator(head_);
  } else {
    new_node->next = current;
    new_node->prev = current->prev;
    current->prev->next = new_node;
    current->prev = new_node;
    ++size_;
    return iterator(new_node);
  }
}

// erases element at pos
template <typename T>
void list<T>::erase(iterator pos) {
  Node *current = pos.ptr_;
  if (!current) return;

  if (current == head_) {
    pop_front();
  } else if (current == tail_) {
    pop_back();
  } else {
    current->prev->next = current->next;
    current->next->prev = current->prev;
    delete current;
    --size_;
  }
}

// adds an element to the end
template <typename T>
void list<T>::push_back(const_reference value) {
  Node *new_node = new Node(value);
  if (!tail_) {
    head_ = tail_ = new_node;
  } else {
    tail_->next = new_node;
    new_node->prev = tail_;
    tail_ = new_node;
  }
  ++size_;
}

// removes the last element
template <typename T>
void list<T>::pop_back() {
  if (!tail_) return;
  Node *to_delete = tail_;
  tail_ = tail_->prev;
  if (tail_) {
    tail_->next = nullptr;
  } else {
    head_ = nullptr;
  }
  delete to_delete;
  --size_;
}

// adds an element to the head
template <typename T>
void list<T>::push_front(const_reference value) {
  Node *new_node = new Node(value);
  if (!head_) {
    head_ = tail_ = new_node;
  } else {
    new_node->next = head_;
    head_->prev = new_node;
    head_ = new_node;
  }
  ++size_;
}

// removes the first element
template <typename T>
void list<T>::pop_front() {
  if (!head_) return;
  Node *to_delete = head_;
  head_ = head_->next;
  if (head_) {
    head_->prev = nullptr;
  } else {
    tail_ = nullptr;
  }
  delete to_delete;
  --size_;
}

// swaps the contents
template <typename T>
void list<T>::swap(list &other) {
  std::swap(head_, other.head_);
  std::swap(tail_, other.tail_);
  std::swap(size_, other.size_);
}

// merges two sorted lists
template <typename T>
void list<T>::merge(list &other) {
  if (this == &other) return;

  Node *current1 = head_;
  Node *current2 = other.head_;

  // Создаём новый список для объединения
  Node *new_head = nullptr;
  Node *new_tail = nullptr;

  while (current1 && current2) {
    if (current1->data <= current2->data) {
      if (!new_head) {
        new_head = new_tail = current1;
      } else {
        new_tail->next = current1;
        current1->prev = new_tail;
        new_tail = current1;
      }
      current1 = current1->next;
    } else {
      if (!new_head) {
        new_head = new_tail = current2;
      } else {
        new_tail->next = current2;
        current2->prev = new_tail;
        new_tail = current2;
      }
      current2 = current2->next;
    }
  }

  while (current1) {
    new_tail->next = current1;
    current1->prev = new_tail;
    new_tail = current1;
    current1 = current1->next;
  }
  while (current2) {
    new_tail->next = current2;
    current2->prev = new_tail;
    new_tail = current2;
    current2 = current2->next;
  }

  if (new_head) {
    head_ = new_head;
    new_tail->next = nullptr;
    tail_ = new_tail;
  }

  size_ += other.size_;
  other.head_ = nullptr;
  other.tail_ = nullptr;
  other.size_ = 0;
}

// transfers elements from list other starting from pos
template <typename T>
void list<T>::splice(const_iterator pos, list &other) {
  for (auto it = other.begin(); it != other.end(); ++it) {
    insert(pos, *it);
  }
  other.clear();
}

// reverses the order of the elements
template <typename T>
void list<T>::reverse() {
  Node *current = head_;
  Node *temp = nullptr;
  while (current) {
    temp = current->prev;
    current->prev = current->next;
    current->next = temp;
    current = current->prev;
  }
  std::swap(head_, tail_);
}

// removes consecutive duplicate elements
template <typename T>
void list<T>::unique() {
  if (!head_) return;
  Node *current = head_;
  while (current->next) {
    if (current->data == current->next->data) {
      Node *to_delete = current->next;
      current->next = to_delete->next;
      if (to_delete->next) {
        to_delete->next->prev = current;
      } else {
        tail_ = current;
      }
      delete to_delete;
      --size_;
    } else {
      current = current->next;
    }
  }
}

// sorts the elements
template <typename T>
void list<T>::sort() {
  if (size_ < 2) return;
  for (Node *i = head_; i; i = i->next) {
    for (Node *j = i->next; j; j = j->next) {
      if (j->data < i->data) {
        std::swap(j->data, i->data);
      }
    }
  }
}

template <typename T>
template <typename... Args>
typename list<T>::iterator list<T>::insert_many(const_iterator pos,
                                                Args &&...args) {
  auto array = {T(std::forward<Args>(args))...};
  iterator result = pos;
  for (const auto &value : array) {
    result = insert(result, value);
    iterator next = result;
    ++next;
    result = next;
  }
  return result;
}

// insert_many_back
template <typename T>
template <typename... Args>
void list<T>::insert_many_back(Args &&...args) {
  insert_many(end(), std::forward<Args>(args)...);
}

// insert_many_front
template <typename T>
template <typename... Args>
void list<T>::insert_many_front(Args &&...args) {
  insert_many(begin(), std::forward<Args>(args)...);
}

}  // namespace s21

#endif  // S21_LIST_TPP