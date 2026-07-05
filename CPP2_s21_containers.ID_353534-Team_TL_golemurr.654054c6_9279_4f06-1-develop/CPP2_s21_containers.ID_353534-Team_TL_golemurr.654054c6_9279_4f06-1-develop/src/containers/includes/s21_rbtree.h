#ifndef S21_CONTAINERS_RBTREE_H_
#define S21_CONTAINERS_RBTREE_H_

#include <functional>

#include "s21_vector.h"

namespace s21 {

// Node Red-Black Tree
enum class Color { RED, BLACK };
template <typename K, typename V>
class Node {
 public:
  K key;
  V value;
  Node *left;
  Node *right;
  Node *parent;
  Color color;
  bool isLeftChild;

  // Constructors
  Node();
  Node(K key, V value);
  Node(Node *node, K &key, V &value);
  Node(Node *node, Node *parent);
};

// Red-Black Tree container
template <typename K, typename V>
class RBTreeConstIterator;
template <typename K, typename V>
class RBTreeIterator;
template <typename K, typename V>
class RBTree {
 public:
  using iterator = RBTreeIterator<K, V>;
  using const_iterator = RBTreeConstIterator<K, V>;
  Node<K, V> *root;
  unsigned size;

  // Constructors and destructor
  RBTree();
  RBTree(const RBTree &other_tree);
  RBTree(RBTree &&other_tree);
  ~RBTree();

  // Assignment operators
  RBTree &operator=(const RBTree &other_tree);

  // Tree balancing operations
  void leftRotate(Node<K, V> *node);
  void rightRotate(Node<K, V> *node);
  void insertFixup(Node<K, V> *node);
  void transplant(Node<K, V> *n, Node<K, V> *child);
  void deleteNodeWithChild(Node<K, V> *node);
  void fixDoubleBlackAtRoot(Node<K, V> *node);
  void fixDoubleBlackWithRedSibling(Node<K, V> *node);
  void fixDoubleBlackWithBlackSiblingAndBlackNephews(Node<K, V> *node);
  void fixDoubleBlackWithBlackSiblingAndRedParent(Node<K, V> *node);
  void fixDoubleBlackWithBlackSiblingAndRedNephew(Node<K, V> *node);
  void fixDoubleBlackFinalFix(Node<K, V> *node);

  // Tree utility functions
  void moveTree(RBTree<K, V> &&other_tree);
  void freeTree(Node<K, V> *node);
  Node<K, V> *copyTree(Node<K, V> *node);
  void clear();

  // Accessors
  Node<K, V> *getRoot();
  Node<K, V> *getRoot() const;
  unsigned getSize() const;

  // Modifiers
  Node<K, V> *insert(const K &key, const V &value,
                     bool allowDuplicates = false);
  Node<K, V> *sibling(Node<K, V> *node);
  void remove(const K &key);
  void removeByNode(Node<K, V> *node);

  Node<K, V> *lower_bound_node(const K &key) const;
  Node<K, V> *upper_bound_node(const K &key) const;
  void merge(RBTree<K, V> &other_tree);

  // Search operations
  Node<K, V> *minimum(Node<K, V> *subTree);
  Node<K, V> *maximum(Node<K, V> *subTree);
  Node<K, V> *minimum(Node<K, V> *subTree) const;
  Node<K, V> *maximum(Node<K, V> *subTree) const;
  Node<K, V> *search(const K &key) const;
  int countElementsEqualToKey(Node<K, V> *root, const K &key) const;
};

// Const (read-only) iterator
template <typename K, typename V>
class RBTreeConstIterator {
 public:
  using value_type = V;
  using const_reference = const value_type &;
  RBTree<K, V> &tree;   // Reference to the tree
  Node<K, V> *current;  // Current node

  // Constructors
  RBTreeConstIterator(Node<K, V> *node, RBTree<K, V> &tree);

  // Iterator operations
  const_reference operator*() const;
  const RBTreeConstIterator &operator++();
  const RBTreeConstIterator &operator--();
  RBTreeConstIterator operator++(int);
  RBTreeConstIterator operator--(int);
  bool operator==(const RBTreeConstIterator &iter);
  bool operator!=(const RBTreeConstIterator &iter);
};

// Non-const (modifiable) iterator
template <typename K, typename V>
class RBTreeIterator {
 public:
  using value_type = V;
  using reference = value_type &;
  using pointer = value_type *;
  using difference_type = std::ptrdiff_t;
  RBTree<K, V> &tree;   // Reference to the tree
  Node<K, V> *current;  // Current node

  // Constructors
  RBTreeIterator(Node<K, V> *node, RBTree<K, V> &tree);
  RBTreeIterator(const RBTreeIterator &other);

  // Iterator operations
  reference operator*() const;
  pointer operator->() const;
  RBTreeIterator &operator++();
  RBTreeIterator &operator--();
  RBTreeIterator operator++(int);
  RBTreeIterator operator--(int);
  bool operator==(const RBTreeIterator &iter) const;
  bool operator!=(const RBTreeIterator &iter) const;
  RBTreeIterator &operator=(const RBTreeIterator &other);
};

#include "../templates/s21_rbtree.tpp"

}  // namespace s21

#endif  // S21_CONTAINERS_RBTREE_H_
