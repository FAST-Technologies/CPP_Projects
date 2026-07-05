#ifndef S21_CONTAINERSPLUS_MULTISET_H_
#define S21_CONTAINERSPLUS_MULTISET_H_

#include "../../containers/includes/s21_rbtree.h"

namespace s21 {
template <typename Key>
class multiset {
 private:
  // Member types
  using value_type = Key;
  using key_type = Key;
  using reference = value_type&;
  using const_reference = const value_type&;
  using BinaryTree = RBTree<Key, Key>;
  using iterator = typename BinaryTree::iterator;
  using const_iterator = typename BinaryTree::const_iterator;
  using size_type = std::size_t;
  RBTree<key_type, value_type> tree;

 public:
  // Constructors and destructor
  multiset();
  multiset(std::initializer_list<value_type> const& items);
  multiset(const multiset& s);
  multiset(multiset&& s);
  ~multiset();
  multiset operator=(multiset&& s);

  // Iterators
  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;

  // Capacity
  bool empty();
  size_type size();
  size_type max_size();
  size_type size() const;

  // Modifiers
  void clear();
  iterator insert(const value_type& value);
  void erase(iterator pos);
  void swap(multiset& other);
  void merge(multiset& other);

  // Lookup
  size_type count(const Key& key);
  iterator find(const Key& key);
  bool contains(const Key& key);
  std::pair<iterator, iterator> equal_range(const Key& key);
  iterator lower_bound(const Key& key);
  iterator upper_bound(const Key& key);

  template <typename... Args>
  std::vector<std::pair<iterator, bool>> insert_many(Args&&... args);
};

#include "../templates/s21_multiset.tpp"
}  // namespace s21

#endif  // S21_CONTAINERSPLUS_MULTISET_H_