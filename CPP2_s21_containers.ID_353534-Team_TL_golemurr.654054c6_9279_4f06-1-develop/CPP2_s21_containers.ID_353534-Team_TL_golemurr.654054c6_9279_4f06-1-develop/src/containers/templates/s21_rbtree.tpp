#ifndef S21_CONTAINERS_RBTREE_TPP_
#define S21_CONTAINERS_RBTREE_TPP_

template <typename K, typename V>
Node<K, V>::Node()
    : key{},
      value{},
      left{nullptr},
      right{nullptr},
      parent{nullptr},
      color{s21::Color::RED},
      isLeftChild{} {};

template <typename K, typename V>
Node<K, V>::Node(K key, V value)
    : key{key},
      value{value},
      left{nullptr},
      right{nullptr},
      parent{nullptr},
      color{s21::Color::RED},
      isLeftChild{} {};

template <typename K, typename V>
Node<K, V>::Node(Node *node, K &key, V &value)
    : key{key},
      value{value},
      left{node->left},
      right{node->right},
      parent{node->parent},
      color{node->color},
      isLeftChild{node->isLeftChild} {
  if (this->left) this->left->parent = this;
  if (this->right) this->right->parent = this;
  if (this->parent) {
    if (isLeftChild) {
      this->parent->left = this;
    } else {
      this->parent->right = this;
    }
  }
};

template <typename K, typename V>
Node<K, V>::Node(Node *node, Node *parent)
    : key{node->key},
      value{node->value},
      left{nullptr},
      right{nullptr},
      parent{parent},
      color{node->color},
      isLeftChild{node->isLeftChild} {};

template <typename K, typename V>
void RBTree<K, V>::leftRotate(Node<K, V> *node) {
  Node<K, V> *temp = node->right;
  node->right = temp->left;
  if (node->right) node->right->isLeftChild = false;
  if (temp->left != nullptr) {
    temp->left->parent = node;
  }
  temp->parent = node->parent;
  if (node->parent == nullptr) {
    this->root = temp;
  } else if (node->isLeftChild == true) {
    node->parent->left = temp;
    temp->isLeftChild = true;
  } else {
    node->parent->right = temp;
    temp->isLeftChild = false;
  }

  temp->left = node;
  node->isLeftChild = true;
  node->parent = temp;
}

template <typename K, typename V>
void RBTree<K, V>::rightRotate(Node<K, V> *node) {
  Node<K, V> *temp = node->left;
  node->left = temp->right;
  if (node->left) node->left->isLeftChild = true;

  if (temp->right != nullptr) {
    temp->right->parent = node;
  }

  temp->parent = node->parent;
  if (node->parent == nullptr) {
    this->root = temp;
  } else if (node->isLeftChild == false) {
    node->parent->right = temp;
    temp->isLeftChild = false;
  } else {
    node->parent->left = temp;
    temp->isLeftChild = true;
  }
  temp->right = node;
  node->isLeftChild = false;
  node->parent = temp;
}

template <typename K, typename V>
void RBTree<K, V>::insertFixup(Node<K, V> *node) {
  Node<K, V> *temp;
  while ((root != node) && node->parent->color == s21::Color::RED) {
    if (node->parent->isLeftChild == false) {
      temp = node->parent->parent->left;
      if (temp && temp->color == s21::Color::RED) {
        temp->color = node->parent->color = s21::Color::BLACK;
        node->parent->parent->color = s21::Color::RED;
        node = node->parent->parent;
      } else {
        if (node->isLeftChild == true) {
          node = node->parent;
          rightRotate(node);
        }
        node->parent->color = s21::Color::BLACK;
        node->parent->parent->color = s21::Color::RED;
        leftRotate(node->parent->parent);
      }
    } else {
      temp = node->parent->parent->right;
      if (temp && temp->color == s21::Color::RED) {
        temp->color = node->parent->color = s21::Color::BLACK;
        node->parent->parent->color = s21::Color::RED;
        node = node->parent->parent;
      } else {
        if (node->isLeftChild == false) {
          node = node->parent;
          leftRotate(node);
        }
        node->parent->color = s21::Color::BLACK;
        node->parent->parent->color = s21::Color::RED;
        rightRotate(node->parent->parent);
      }
    }
  }
  root->color = s21::Color::BLACK;
}

template <typename K, typename V>
void RBTree<K, V>::transplant(Node<K, V> *n, Node<K, V> *child) {
  if (n->parent == nullptr) {
    root = child;
  } else if (n->isLeftChild == true) {
    n->parent->left = child;
    if (child) child->isLeftChild = true;
  } else {
    n->parent->right = child;
    if (child) child->isLeftChild = false;
  }
  if (child) child->parent = n->parent;
}

template <typename K, typename V>
void RBTree<K, V>::deleteNodeWithChild(Node<K, V> *node) {
  Node<K, V> *child = node->left ? node->left : node->right;
  transplant(node, child);
  if (node->color == s21::Color::BLACK) {
    if (child && child->color == s21::Color::RED) {
      child->color = s21::Color::BLACK;
    } else {
      fixDoubleBlackAtRoot(node);
    }
  }
  delete node;
}

template <typename K, typename V>
void RBTree<K, V>::fixDoubleBlackAtRoot(Node<K, V> *node) {
  if (node->parent != nullptr) fixDoubleBlackWithRedSibling(node);
}

template <typename K, typename V>
void RBTree<K, V>::fixDoubleBlackWithRedSibling(Node<K, V> *node) {
  Node<K, V> *brother = sibling(node);
  if (brother && brother->color == s21::Color::RED) {
    node->parent->color = s21::Color::RED;
    brother->color = s21::Color::BLACK;
    if (node->isLeftChild == true) {
      leftRotate(node->parent);
    } else {
      rightRotate(node->parent);
    }
  }
  fixDoubleBlackWithBlackSiblingAndBlackNephews(node);
}

template <typename K, typename V>
void RBTree<K, V>::fixDoubleBlackWithBlackSiblingAndBlackNephews(
    Node<K, V> *node) {
  Node<K, V> *brother = sibling(node);
  if ((node->parent->color == s21 ::Color::BLACK) &&
      (brother->color == s21 ::Color::BLACK) &&
      (brother->left == nullptr ||
       brother->left->color == s21 ::Color::BLACK) &&
      (brother->right == nullptr ||
       brother->right->color == s21 ::Color::BLACK)) {
    brother->color = s21::Color::RED;
    fixDoubleBlackAtRoot(node->parent);
  } else {
    fixDoubleBlackWithBlackSiblingAndRedParent(node);
  }
}

template <typename K, typename V>
void RBTree<K, V>::fixDoubleBlackWithBlackSiblingAndRedParent(
    Node<K, V> *node) {
  Node<K, V> *brother = sibling(node);
  if ((node->parent->color == s21::Color::RED) &&
      (brother->color == s21 ::Color::BLACK) &&
      (brother->left == nullptr ||
       brother->left->color == s21 ::Color::BLACK) &&
      (brother->right == nullptr ||
       brother->right->color == s21 ::Color::BLACK)) {
    brother->color = s21::Color::RED;
    node->parent->color = s21::Color::BLACK;
  } else {
    fixDoubleBlackWithBlackSiblingAndRedNephew(node);
  }
}

template <typename K, typename V>
void RBTree<K, V>::fixDoubleBlackWithBlackSiblingAndRedNephew(
    Node<K, V> *node) {
  Node<K, V> *brother = sibling(node);
  if (brother->color == s21::Color::BLACK) {
    if ((node->isLeftChild == true) &&
        (brother->left && brother->left->color == s21::Color::RED) &&
        (brother->right == nullptr ||
         brother->right->color == s21 ::Color::BLACK)) {
      brother->color = s21::Color::RED;
      brother->left->color = s21::Color::BLACK;
      rightRotate(brother);
    } else if ((node->isLeftChild == false) &&
               (brother->right && brother->right->color == s21::Color::RED) &&
               (brother->left == nullptr ||
                brother->left->color == s21 ::Color::BLACK)) {
      brother->color = s21::Color::RED;
      brother->right->color = s21::Color::BLACK;
      leftRotate(brother);
    }
  }
  fixDoubleBlackFinalFix(node);
}

template <typename K, typename V>
void RBTree<K, V>::fixDoubleBlackFinalFix(Node<K, V> *node) {
  Node<K, V> *brother = sibling(node);
  brother->color = node->parent->color;
  node->parent->color = s21::Color::BLACK;
  if (node->isLeftChild == true) {
    brother->right->color = s21::Color::BLACK;
    leftRotate(node->parent);
  } else {
    brother->left->color = s21::Color::BLACK;
    rightRotate(node->parent);
  }
}

template <typename K, typename V>
RBTree<K, V>::RBTree() : root{nullptr}, size{} {}

template <typename K, typename V>
RBTree<K, V>::RBTree(const RBTree &other_tree) {
  root = copyTree(other_tree.root);
  size = other_tree.size;
}

template <typename K, typename V>
RBTree<K, V>::RBTree(RBTree &&other_tree) {
  moveTree(std::move(other_tree));
}

template <typename K, typename V>
RBTree<K, V>::~RBTree() {
  freeTree(this->root);
}

template <typename K, typename V>
void RBTree<K, V>::moveTree(RBTree<K, V> &&other_tree) {
  root = other_tree.root;
  size = other_tree.size;

  other_tree.root = nullptr;
  other_tree.size = 0;
}

template <typename K, typename V>
void RBTree<K, V>::freeTree(Node<K, V> *node) {
  if (node) {
    if (node->left) freeTree(node->left);
    if (node->right) freeTree(node->right);
    delete node;
  }
}

template <typename K, typename V>
Node<K, V> *RBTree<K, V>::copyTree(Node<K, V> *node) {
  if (node == nullptr) {
    return nullptr;
  }
  Node<K, V> *newNode = new Node<K, V>(node, node->parent);
  newNode->left = copyTree(node->left);
  newNode->right = copyTree(node->right);
  return newNode;
}

template <typename K, typename V>
Node<K, V> *RBTree<K, V>::getRoot(void) {
  return this->root;
}

template <typename K, typename V>
Node<K, V> *RBTree<K, V>::getRoot() const {
  return this->root;
}

template <typename K, typename V>
unsigned RBTree<K, V>::getSize(void) const {
  return this->size;
}

template <typename K, typename V>
Node<K, V> *RBTree<K, V>::insert(const K &key, const V &value,
                                 bool allowDuplicates) {
  Node<K, V> *newNode = new Node<K, V>{key, value};
  Node<K, V> *parent = nullptr;
  Node<K, V> *x = root;
  this->size++;
  while (x != nullptr) {
    parent = x;
    if constexpr (!std::is_same_v<V, K>) {
      if (key < x->key) {
        x = x->left;
      } else if (key > x->key) {
        x = x->right;
      } else {
        if (!allowDuplicates) {
          delete newNode;
          this->size--;
          return x;
        }
        parent = x;
        x = x->right;
      }
    } else {
      if (value < x->value) {
        x = x->left;
      } else if (value > x->value) {
        x = x->right;
      } else {
        if (!allowDuplicates) {
          delete newNode;
          this->size--;
          return x;
        }
        parent = x;
        x = x->right;
      }
    }
  }
  newNode->parent = parent;
  if (parent == nullptr) {
    this->root = newNode;
  } else {
    if constexpr (!std::is_same_v<V, K>) {
      if (key < parent->key) {
        parent->left = newNode;
        newNode->isLeftChild = true;
      } else {
        parent->right = newNode;
        newNode->isLeftChild = false;
      }
    } else {
      if (value < parent->value) {
        parent->left = newNode;
        newNode->isLeftChild = true;
      } else {
        parent->right = newNode;
        newNode->isLeftChild = false;
      }
    }
  }
  if (newNode->parent == nullptr) {
    newNode->color = s21::Color::BLACK;
  } else if (newNode->parent->parent != nullptr) {
    insertFixup(newNode);
  }
  return newNode;
}

template <typename K, typename V>
Node<K, V> *RBTree<K, V>::sibling(Node<K, V> *node) {
  Node<K, V> *res = nullptr;
  if (node->isLeftChild) {
    res = node->parent->right;
  } else {
    res = node->parent->left;
  }
  return res;
}

template <typename K, typename V>
void RBTree<K, V>::remove(const K &key) {
  Node<K, V> *node = search(key);
  if (node) {
    Node<K, V> *delNode = nullptr;
    if (node->left && node->right) {
      delNode = maximum(node->left);
      Node<K, V> *tmp = new Node<K, V>{delNode, node->key, node->value};
      new Node<K, V>{node, delNode->key, delNode->value};
      delete delNode;
      delete node;
      delNode = tmp;
    } else {
      delNode = node;
    }
    deleteNodeWithChild(delNode);
    size--;
  }
}

template <typename K, typename V>
void RBTree<K, V>::removeByNode(Node<K, V> *node) {
  if (node) {
    Node<K, V> *delNode = nullptr;
    if (node->left && node->right) {
      Node<K, V> *pred = maximum(node->left);

      std::swap(node->key, pred->key);
      std::swap(node->value, pred->value);

      delNode = pred;
    } else {
      delNode = node;
    }
    deleteNodeWithChild(delNode);
    size--;
  }
}

template <typename K, typename V>
Node<K, V> *RBTree<K, V>::minimum(Node<K, V> *subTree) {
  if (subTree) {
    while (subTree->left != nullptr) {
      subTree = subTree->left;
    }
  }
  return subTree;
}

template <typename K, typename V>
Node<K, V> *RBTree<K, V>::maximum(Node<K, V> *subTree) {
  if (subTree) {
    while (subTree->right != nullptr) {
      subTree = subTree->right;
    }
  }
  return subTree;
}

template <typename K, typename V>
Node<K, V> *RBTree<K, V>::minimum(Node<K, V> *subTree) const {
  if (subTree) {
    while (subTree->left != nullptr) {
      subTree = subTree->left;
    }
  }
  return subTree;
}

template <typename K, typename V>
Node<K, V> *RBTree<K, V>::maximum(Node<K, V> *subTree) const {
  if (subTree) {
    while (subTree->right != nullptr) {
      subTree = subTree->right;
    }
  }
  return subTree;
}

template <typename K, typename V>
Node<K, V> *RBTree<K, V>::search(const K &key) const {
  Node<K, V> *res = root;
  while (res != nullptr && res->key != key) {
    if (key < res->key) {
      res = res->left;
    } else {
      res = res->right;
    }
  }
  return res;
}

template <typename K, typename V>
Node<K, V> *RBTree<K, V>::lower_bound_node(const K &key) const {
  Node<K, V> *res = nullptr;
  Node<K, V> *curr = root;
  while (curr != nullptr) {
    if constexpr (std::is_same_v<V, K>) {
      if (curr->value < key) {
        curr = curr->right;
      } else {
        res = curr;
        curr = curr->left;
      }
    } else {
      if (curr->value.first < key) {
        curr = curr->right;
      } else {
        res = curr;
        curr = curr->left;
      }
    }
  }
  if (res && res->value == key) {
    while (res->left && res->left->value == key) {
      res = res->left;
    }
  }
  return res;
}

template <typename K, typename V>
Node<K, V> *RBTree<K, V>::upper_bound_node(const K &key) const {
  Node<K, V> *res = nullptr;
  Node<K, V> *curr = root;
  while (curr != nullptr) {
    if constexpr (std::is_same_v<V, K>) {
      if (curr->value <= key) {
        curr = curr->right;
      } else {
        res = curr;
        curr = curr->left;
      }
    } else {
      if (curr->value.first <= key) {
        curr = curr->right;
      } else {
        res = curr;
        curr = curr->left;
      }
    }
  }
  if (res && res->value == key) {
    while (res->right && res->right->value == key) {
      res = res->right;
    }
    ++res;
  }
  return res;
}

template <typename K, typename V>
void RBTree<K, V>::clear() {
  freeTree(root);
  root = nullptr;
  size = 0;
}

template <typename K, typename V>
void RBTree<K, V>::merge(RBTree<K, V> &other_tree) {
  if (other_tree.root == nullptr) return;

  if (this->root == nullptr) {
    this->root = other_tree.root;
    this->size = other_tree.size;
    other_tree.root = nullptr;
    other_tree.size = 0;
    return;
  }

  std::vector<Node<K, V> *> moved_nodes;
  std::vector<V> remaining_values;

  std::function<void(Node<K, V> *)> traverse = [&](Node<K, V> *node) {
    if (!node) return;
    traverse(node->left);

    if constexpr (std::is_same_v<V, K>) {
      if (!this->search(node->value)) {
        moved_nodes.push_back(node);
      } else {
        remaining_values.push_back(node->value);
      }
    } else {
      if (!this->search(node->value.first)) {
        moved_nodes.push_back(node);
      } else {
        remaining_values.push_back(node->value);
      }
    }

    traverse(node->right);
  };

  traverse(other_tree.root);

  other_tree.clear();

  for (const auto &value : remaining_values) {
    if constexpr (std::is_same_v<V, K>) {
      other_tree.insert(value, value);  // set
    } else {
      other_tree.insert(value.first, value);  // map
    }
  }

  for (Node<K, V> *node : moved_nodes) {
    node->parent = node->left = node->right = nullptr;

    Node<K, V> *parent = nullptr;
    Node<K, V> *x = this->root;

    while (x != nullptr) {
      parent = x;
      if constexpr (std::is_same_v<V, K>) {
        x = (node->value < x->value) ? x->left : x->right;
      } else {
        x = (node->value.first < x->value.first) ? x->left : x->right;
      }
    }

    node->parent = parent;
    if (parent == nullptr) {
      this->root = node;
    } else if constexpr (std::is_same_v<V, K>) {
      if (node->value < parent->value) {
        parent->left = node;
        node->isLeftChild = true;
      } else {
        parent->right = node;
        node->isLeftChild = false;
      }
    } else {
      if (node->value.first < parent->value.first) {
        parent->left = node;
        node->isLeftChild = true;
      } else {
        parent->right = node;
        node->isLeftChild = false;
      }
    }

    ++this->size;
    if (node->parent && node->parent->parent) {
      insertFixup(node);
    }
  }
}

template <typename K, typename V>
int RBTree<K, V>::countElementsEqualToKey(Node<K, V> *root,
                                          const K &key) const {
  if (root == nullptr) return 0;
  int count = 0;
  if constexpr (std::is_same_v<V, K>) {
    if (root->value == key) count++;
  } else {
    if (root->value.first == key) count++;
  }
  count += countElementsEqualToKey(root->left, key);
  count += countElementsEqualToKey(root->right, key);
  return count;
}

template <typename K, typename V>
RBTreeConstIterator<K, V>::RBTreeConstIterator(Node<K, V> *node,
                                               RBTree<K, V> &tree)
    : tree{tree}, current{node} {};

template <typename K, typename V>
typename RBTreeConstIterator<K, V>::const_reference
RBTreeConstIterator<K, V>::operator*() const {
  if (current != nullptr) {
    return current->value;
  } else {
    static V res{};
    return res;
  }
}

template <typename K, typename V>
const RBTreeConstIterator<K, V> &RBTreeConstIterator<K, V>::operator++() {
  if (current != nullptr) {
    if (current->right == nullptr) {
      Node<K, V> *tmp = nullptr;
      while ((tmp = current->parent) != nullptr && (current == tmp->right)) {
        current = tmp;
      }
      current = tmp;
    } else {
      current = tree.minimum(current->right);
    }
  }
  return *this;
}

template <typename K, typename V>
const RBTreeConstIterator<K, V> &RBTreeConstIterator<K, V>::operator--() {
  if (current != nullptr) {
    if (current->left == nullptr) {
      Node<K, V> *tmp = nullptr;
      while ((tmp = current->parent) != nullptr && (current == tmp->left)) {
        current = tmp;
      }
      current = tmp;
    } else {
      current = tree.maximum(current->left);
    }
  } else {
    current = tree.maximum(tree.getRoot());
  }
  return *this;
}

template <typename K, typename V>
RBTreeConstIterator<K, V> RBTreeConstIterator<K, V>::operator++(int) {
  RBTreeConstIterator temp(*this);
  ++(*this);
  return temp;
}

template <typename K, typename V>
RBTreeConstIterator<K, V> RBTreeConstIterator<K, V>::operator--(int) {
  RBTreeConstIterator temp(*this);
  --(*this);
  return temp;
}

template <typename K, typename V>
bool RBTreeConstIterator<K, V>::operator==(const RBTreeConstIterator &iter) {
  return current == iter.current;
}

template <typename K, typename V>
bool RBTreeConstIterator<K, V>::operator!=(const RBTreeConstIterator &iter) {
  return current != iter.current;
}

template <typename K, typename V>
RBTreeIterator<K, V>::RBTreeIterator(Node<K, V> *node, RBTree<K, V> &tree)
    : tree{tree}, current{node} {};

template <typename K, typename V>
RBTreeIterator<K, V>::RBTreeIterator(const RBTreeIterator &other)
    : tree{other.tree}, current{other.current} {};

template <typename K, typename V>
typename RBTreeIterator<K, V>::reference RBTreeIterator<K, V>::operator*()
    const {
  if (current != nullptr) {
    return current->value;
  } else {
    static V res{};
    return res;
  }
}

template <typename K, typename V>
typename RBTreeIterator<K, V>::pointer RBTreeIterator<K, V>::operator->()
    const {
  return &(operator*());
}

template <typename K, typename V>
RBTreeIterator<K, V> &RBTreeIterator<K, V>::operator++() {
  if (current != nullptr) {
    Node<K, V> *next = current->right;
    if (next && next->value == current->value) {
      current = next;
    } else {
      next = current->right ? tree.minimum(current->right) : nullptr;
      if (!next) {
        Node<K, V> *tmp = current->parent;
        while (tmp && current == tmp->right) {
          current = tmp;
          tmp = tmp->parent;
        }
        current = tmp;
      } else {
        current = next;
      }
    }
  }
  return *this;
}

template <typename K, typename V>
RBTreeIterator<K, V> RBTreeIterator<K, V>::operator++(int) {
  RBTreeIterator<K, V> temp(*this);
  ++(*this);
  return temp;
}

template <typename K, typename V>
RBTreeIterator<K, V> RBTreeIterator<K, V>::operator--(int) {
  RBTreeIterator temp(*this);
  --(*this);
  return temp;
}

template <typename K, typename V>
RBTreeIterator<K, V> &RBTreeIterator<K, V>::operator--() {
  if (current != nullptr) {
    if (current->left == nullptr) {
      Node<K, V> *tmp = current->parent;
      while (tmp != nullptr && current == tmp->left) {
        current = tmp;
        tmp = tmp->parent;
      }
      current = tmp;
    } else {
      current = tree.maximum(current->left);
    }
  } else {
    current = tree.maximum(tree.getRoot());
  }
  return *this;
}

template <typename K, typename V>
bool RBTreeIterator<K, V>::operator==(const RBTreeIterator &iter) const {
  return current == iter.current;
}

template <typename K, typename V>
bool RBTreeIterator<K, V>::operator!=(const RBTreeIterator &iter) const {
  return current != iter.current;
}

template <typename K, typename V>
RBTree<K, V> &RBTree<K, V>::operator=(const RBTree &other_tree) {
  if (this != &other_tree) {
    freeTree(this->root);
    this->root = copyTree(other_tree.root);
    this->size = other_tree.size;
  }
  return *this;
}

template <typename K, typename V>
RBTreeIterator<K, V> &RBTreeIterator<K, V>::operator=(
    const RBTreeIterator &other) {
  if (this != &other) {
    this->current = other.current;
  }
  return *this;
}

#endif  // S21_CONTAINERS_RBTREE_TPP_