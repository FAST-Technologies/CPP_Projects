#ifndef S21_CONTAINERSPLUS_MULTISET_TPP_
#define S21_CONTAINERSPLUS_MULTISET_TPP_

template <typename Key>
multiset<Key>::multiset() : tree{} {}

template <typename Key>
multiset<Key>::multiset(std::initializer_list<value_type> const& items)
    : multiset{} {
  for (const_reference i : items) {
    tree.insert(i, i, true);
  }
}

template <typename Key>
multiset<Key>::multiset(const multiset& s) : multiset() {
  tree.root = tree.copyTree(s.tree.root);
  tree.size = s.tree.size;
}

template <typename Key>
multiset<Key>::multiset(multiset&& s) : tree{std::move(s.tree)} {}

template <typename Key>
multiset<Key>::~multiset() {}

template <typename Key>
multiset<Key> multiset<Key>::operator=(multiset&& s) {
  if (tree.root != nullptr) tree.freeTree(tree.root);
  tree.moveTree(std::move(s.tree));
  return *this;
}

template <typename Key>
typename multiset<Key>::iterator multiset<Key>::begin() {
  return iterator(tree.minimum(tree.root), tree);
}

template <typename Key>
typename multiset<Key>::iterator multiset<Key>::end() {
  return iterator(nullptr, tree);
}

template <typename Key>
typename multiset<Key>::const_iterator multiset<Key>::begin() const {
  return const_iterator(tree.minimum(tree.getRoot()),
                        const_cast<RBTree<key_type, value_type>&>(tree));
}

template <typename Key>
typename multiset<Key>::const_iterator multiset<Key>::end() const {
  return const_iterator(nullptr,
                        const_cast<RBTree<key_type, value_type>&>(tree));
}

template <typename Key>
bool multiset<Key>::empty() {
  return tree.size == 0;
}

template <typename Key>
typename multiset<Key>::size_type multiset<Key>::size() {
  return tree.size;
}

template <typename Key>
typename multiset<Key>::size_type multiset<Key>::size() const {
  return tree.getSize();
}

template <typename Key>
typename multiset<Key>::size_type multiset<Key>::max_size() {
  return size_t(-1) / (sizeof(BinaryTree)) / 5;
}

template <typename Key>
void multiset<Key>::clear() {
  tree.freeTree(tree.root);
  tree.root = nullptr;
  tree.size = 0;
}

template <typename Key>
typename multiset<Key>::iterator multiset<Key>::insert(
    const value_type& value) {
  auto node = tree.insert(value, value, true);
  iterator iter = iterator(node, tree);
  return iter;
}

template <typename Key>
void multiset<Key>::erase(iterator pos) {
  if (pos.current != nullptr) {
    auto node = pos.current;
    tree.removeByNode(node);
  }
}

template <typename Key>
void multiset<Key>::swap(multiset& other) {
  auto tmp = other.tree.root;
  other.tree.root = tree.root;
  tree.root = tmp;

  unsigned size_tmp = other.tree.size;
  other.tree.size = tree.size;
  tree.size = size_tmp;
}

template <typename Key>
void multiset<Key>::merge(multiset& other) {
  for (auto iter = other.begin(); iter != other.end(); ++iter) {
    insert(*iter);
  }
  other.clear();
}

template <typename Key>
typename multiset<Key>::size_type multiset<Key>::count(const Key& key) {
  return tree.countElementsEqualToKey(tree.getRoot(), key);
}

template <typename Key>
typename multiset<Key>::iterator multiset<Key>::find(const Key& key) {
  Node<Key, Key>* node = tree.search(key);
  if (node) {
    return iterator(node, tree);
  }
  return end();
}

template <typename Key>
bool multiset<Key>::contains(const Key& key) {
  return (tree.search(key)) ? true : false;
}

template <typename Key>
std::pair<typename multiset<Key>::iterator, typename multiset<Key>::iterator>
multiset<Key>::equal_range(const Key& key) {
  std::pair<iterator, iterator> res = {lower_bound(key), upper_bound(key)};
  return res;
}

template <typename Key>
typename multiset<Key>::iterator multiset<Key>::lower_bound(const Key& key) {
  auto node = tree.lower_bound_node(key);
  return iterator(node, tree);
}

template <typename Key>
typename multiset<Key>::iterator multiset<Key>::upper_bound(const Key& key) {
  auto node = tree.upper_bound_node(key);
  return iterator(node, tree);
}

template <typename Key>
template <typename... Args>
std::vector<std::pair<typename multiset<Key>::iterator, bool>>
multiset<Key>::insert_many(Args&&... args) {
  std::vector<std::pair<iterator, bool>> res;
  if (sizeof...(Args) > 0) {
    (res.push_back(
         std::pair<iterator, bool>(insert(std::forward<Args>(args)), true)),
     ...);
  }
  return res;
}

#endif  // S21_CONTAINERSPLUS_MULTISET_TPP_