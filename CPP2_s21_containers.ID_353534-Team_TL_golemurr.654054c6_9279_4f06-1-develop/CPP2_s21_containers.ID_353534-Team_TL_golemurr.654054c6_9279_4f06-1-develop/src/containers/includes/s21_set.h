#ifndef S21_CONTAINERS_SET_H_
#define S21_CONTAINERS_SET_H_

#include <cstddef>
#include <initializer_list>
#include <limits>

#include "s21_rbtree.h"

namespace s21 {

template <typename Key>
class set {
 private:
  // Member types
  using key_type = Key;
  using value_type = Key;
  using reference = value_type &;
  using const_reference = const value_type &;
  using size_type = std::size_t;
  using BinaryTree = RBTree<Key, Key>;
  using iterator = typename BinaryTree::iterator;
  using const_iterator = typename BinaryTree::const_iterator;
  RBTree<key_type, value_type> rb_tree;

 public:
  // Constructors and destructor
  set();
  set(std::initializer_list<value_type> const &items);
  set(const set &s);
  set(set &&s);
  ~set();

  // Assignment operator
  set &operator=(const set &s);
  set &operator=(set &&s);

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
  void erase(iterator pos);
  void swap(set &other);
  void merge(set &other);

  // Lookup
  iterator find(const Key &key);
  bool contains(const Key &key) const;

  template <typename... Args>
  std::vector<std::pair<iterator, bool>> insert_many(Args &&...args);
};
#include "../templates/s21_set.tpp"
}  // namespace s21

#endif  // S21_CONTAINERS_SET_H_