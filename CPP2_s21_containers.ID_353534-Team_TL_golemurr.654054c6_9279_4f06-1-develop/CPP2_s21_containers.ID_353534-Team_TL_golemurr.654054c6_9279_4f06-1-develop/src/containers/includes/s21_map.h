#ifndef S21_CONTAINERS_MAP_H_
#define S21_CONTAINERS_MAP_H_

#include <climits>
#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <utility>

#include "s21_rbtree.h"

namespace s21 {

template <typename Key, typename T>
class map {
 private:
  // Member types
  using key_type = Key;
  using mapped_type = T;
  using value_type = std::pair<const key_type, mapped_type>;
  using reference = value_type &;
  using const_reference = const value_type &;
  using size_type = size_t;
  using BinaryTree = RBTree<key_type, value_type>;
  using iterator = typename BinaryTree::iterator;
  using const_iterator = typename BinaryTree::const_iterator;
  RBTree<key_type, value_type> rb_tree;

 public:
  // Constructors and destructor
  map();
  map(std::initializer_list<value_type> const &items);
  map(const map<Key, T> &m);
  map(map<Key, T> &&m);
  ~map();

  // Assignment operator
  map<Key, T> &operator=(map<Key, T> &&m);
  map<Key, T> &operator=(const map<Key, T> &m);

  // Element access
  T &at(const Key &key);
  T &operator[](const Key &key);

  // Iterators
  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;

  // Capacity
  bool empty() const;
  size_type size() const;
  size_type max_size() const;

  // Modifiers
  void clear();
  std::pair<iterator, bool> insert(const value_type &value);
  std::pair<iterator, bool> insert(const Key &key, const T &obj);
  std::pair<iterator, bool> insert_or_assign(const Key &key, const T &obj);
  void erase(iterator pos);
  void erase(iterator pos1, iterator pos2);
  void erase(const Key &key);
  void swap(map<Key, T> &other);
  void merge(map<Key, T> &other);

  // Lookup
  bool contains(const Key &key) const;
  iterator find(const Key &key);
  size_type count(const Key &key);
  iterator lower_bound(const Key &key);
  iterator upper_bound(const Key &key);

  template <typename... Args>
  std::vector<std::pair<iterator, bool>> insert_many(Args &&...args);
};
#include "../templates/s21_map.tpp"
}  // namespace s21

#endif  // S21_CONTAINERS_MAP_H_