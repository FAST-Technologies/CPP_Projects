#ifndef S21_CONTAINERS_SET_TPP_
#define S21_CONTAINERS_SET_TPP_

// КОнструктор по умолчанию
template <typename Key>
set<Key>::set() : rb_tree{} {}

// Конструктор от списка инициализации
template <typename Key>
set<Key>::set(std::initializer_list<value_type> const &items) : set{} {
  for (const_reference i : items) {
    insert(i);
  }
}

// Конструктор копирования
template <typename Key>
set<Key>::set(const set &s) : rb_tree(s.rb_tree) {}

// Конструктор перемещения
template <typename Key>
set<Key>::set(set &&s) : rb_tree{std::move(s.rb_tree)} {}

// Деструктор класса
template <typename Key>
set<Key>::~set() {
  if (rb_tree.root != nullptr) {
    rb_tree.freeTree(rb_tree.root);
    rb_tree.root = nullptr;
  }
}

// Оператор присваивания копированием
template <typename Key>
set<Key> &set<Key>::operator=(const set &s) {
  if (this != &s) {
    if (rb_tree.root != nullptr) rb_tree.freeTree(rb_tree.root);
    rb_tree = s.rb_tree;
  }
  return *this;
}

// Оператор присваивания перемещением
template <typename Key>
set<Key> &set<Key>::operator=(set<Key> &&s) {
  if (this != &s) {
    if (rb_tree.root != nullptr) rb_tree.freeTree(rb_tree.root);
    rb_tree.moveTree(std::move(s.rb_tree));
  }
  return *this;
}

// Возврат итератора на первый элемент
template <typename Key>
typename set<Key>::iterator set<Key>::begin() {
  return iterator(rb_tree.minimum(rb_tree.getRoot()), rb_tree);
}

// Возврат итератора на позицию после конечного элемента
template <typename Key>
typename set<Key>::iterator set<Key>::end() {
  return iterator(nullptr, rb_tree);
}

template <typename Key>
typename set<Key>::const_iterator set<Key>::begin() const {
  return const_iterator(rb_tree.minimum(rb_tree.getRoot()),
                        const_cast<RBTree<key_type, value_type> &>(rb_tree));
}

template <typename Key>
typename set<Key>::const_iterator set<Key>::end() const {
  return const_iterator(nullptr,
                        const_cast<RBTree<key_type, value_type> &>(rb_tree));
}

// Проверка множества на пустоту
template <typename Key>
bool set<Key>::empty() const {
  return rb_tree.size == 0;
}

// Возврат текущегшо размера множества
template <typename Key>
typename set<Key>::size_type set<Key>::size() const {
  return rb_tree.size;
}

// Возврат максимального возможного размера множества
template <typename Key>
typename set<Key>::size_type set<Key>::max_size() const {
  return size_t(-1) / (sizeof(BinaryTree)) / 5;
}

// Очистка множества
template <typename Key>
void set<Key>::clear() {
  rb_tree.freeTree(rb_tree.root);
  rb_tree.root = nullptr;
  rb_tree.size = 0;
}

// Вставка нового элемента в множество
template <typename Key>
std::pair<typename set<Key>::iterator, bool> set<Key>::insert(
    const value_type &value) {
  Node<Key, Key> *node = rb_tree.search(value);
  if (node == nullptr) {
    node = rb_tree.insert(value, value, false);
    return {iterator(node, rb_tree), true};
  }
  return {iterator(node, rb_tree), false};
}

// Удаление элемента по заданному итератору
template <typename Key>
void set<Key>::erase(iterator pos) {
  if (pos.current != nullptr && pos.tree.getRoot() == rb_tree.getRoot()) {
    rb_tree.removeByNode(pos.current);
  }
}

// Изменение содержимого двух множеств
template <typename Key>
void set<Key>::swap(set &other) {
  std::swap(rb_tree.root, other.rb_tree.root);
  std::swap(rb_tree.size, other.rb_tree.size);
}

// Объединение содержимого двух множеств
template <typename Key>
void set<Key>::merge(set &other) {
  rb_tree.merge(other.rb_tree);
}

// Поиск итератора на элемент с заданным ключом
template <typename Key>
typename set<Key>::iterator set<Key>::find(const Key &key) {
  Node<Key, Key> *node = rb_tree.search(key);
  return iterator(node, rb_tree);
}

// Проверка наличия элемента с заданным ключом
template <typename Key>
bool set<Key>::contains(const Key &key) const {
  return (rb_tree.search(key)) ? true : false;
}

// Вставка сразу нескольких элементов с помощью perfect forwarding
template <typename Key>
template <typename... Args>
std::vector<std::pair<typename set<Key>::iterator, bool>> set<Key>::insert_many(
    Args &&...args) {
  std::vector<std::pair<iterator, bool>> result;
  if constexpr (sizeof...(Args) > 0) {
    (([&] {
       auto [it, inserted] = insert(std::forward<Args>(args));
       result.emplace_back(it, inserted);
     }()),
     ...);
  }
  return result;
}

#endif  // S21_CONTAINERS_SET_TPP_