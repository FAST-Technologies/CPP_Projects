#ifndef S21_LIST_H
#define S21_LIST_H

#include <cstddef>
#include <initializer_list>

namespace s21 {

template <typename T>
class list {
 private:
  struct Node {
    T data;
    Node* next;
    Node* prev;
    Node(const T& value) : data(value), next(nullptr), prev(nullptr) {}
  };

  Node* head_;
  Node* tail_;
  int size_;

 public:
  // Member type
  using value_type = T;
  using reference = T&;
  using const_reference = const T&;

  class iterator {
   public:
    iterator(Node* ptr = nullptr) : ptr_(ptr) {}

    reference operator*() const { return ptr_->data; }

    iterator& operator++() {
      if (ptr_) ptr_ = ptr_->next;
      return *this;
    }

    iterator operator++(int) {
      iterator temp = *this;
      if (ptr_) ptr_ = ptr_->next;
      return temp;
    }

    iterator& operator--() {
      if (ptr_) ptr_ = ptr_->prev;
      return *this;
    }

    iterator operator--(int) {
      iterator temp = *this;
      if (ptr_) ptr_ = ptr_->prev;
      return temp;
    }

    bool operator==(const iterator& other) const { return ptr_ == other.ptr_; }
    bool operator!=(const iterator& other) const { return ptr_ != other.ptr_; }

    Node* ptr_;
  };

  using const_iterator = const iterator;

  using size_type = std::size_t;

  // List Functions
  list();
  list(size_type n);
  list(std::initializer_list<value_type> const& items);
  list(const list& l);
  list(list&& l);
  ~list();
  list& operator=(list&& l);

  // List Element access
  const_reference front() const;
  const_reference back() const;

  // List Iterators
  iterator begin();
  iterator end();
  const_iterator cbegin() const noexcept;
  const_iterator cend() const noexcept;
  const_iterator begin() const noexcept;
  const_iterator end() const noexcept;

  // List Capacity
  bool empty() const;
  size_type size() const;
  size_type max_size() const;

  // List Modifiers
  void clear();
  iterator insert(iterator pos, const_reference value);
  void erase(iterator pos);
  void push_back(const_reference value);
  void pop_back();
  void push_front(const_reference value);
  void pop_front();
  void swap(list& other);
  void merge(list& other);
  void splice(const_iterator pos, list& other);
  void reverse();
  void unique();
  void sort();

  template <typename... Args>
  iterator insert_many(const_iterator pos, Args&&... args);
  template <typename... Args>
  void insert_many_back(Args&&... args);
  template <typename... Args>
  void insert_many_front(Args&&... args);
};

}  // namespace s21

#include "../templates/s21_list.tpp"
#endif  // S21_LIST_H