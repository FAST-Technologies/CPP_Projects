#ifndef S21_CONTAINERS_MAP_TPP_
#define S21_CONTAINERS_MAP_TPP_

// Конструктор по умолчанию
template <typename Key, typename T>
map<Key, T>::map() : rb_tree{} {}

// Конструктор от списка инициализации
template <typename Key, typename T>
map<Key, T>::map(std::initializer_list<value_type> const &items) : map{} {
  for (const_reference i : items) {
    insert(i);
  }
}

// Конструктор копирования
template <typename Key, typename T>
map<Key, T>::map(const map<Key, T> &s) : rb_tree{} {
  rb_tree.root = rb_tree.copyTree(s.rb_tree.root);
  rb_tree.size = s.rb_tree.size;
}

// Конструктор перемещения
template <typename Key, typename T>
map<Key, T>::map(map<Key, T> &&s) : rb_tree{std::move(s.rb_tree)} {}

// Деструктор класса
template <typename Key, typename T>
map<Key, T>::~map() {
  if (rb_tree.root != nullptr) {
    rb_tree.freeTree(rb_tree.root);
    rb_tree.root = nullptr;
  }
}

// Оператор присваивания копированием
template <typename Key, typename T>
map<Key, T> &map<Key, T>::operator=(const map &s) {
  if (this != &s) {
    if (rb_tree.root != nullptr) rb_tree.freeTree(rb_tree.root);
    rb_tree = s.rb_tree;
  }
  return *this;
}

// Оператор присваивания перемещением
template <typename Key, typename T>
map<Key, T> &map<Key, T>::operator=(map<Key, T> &&s) {
  if (this != &s) {
    if (rb_tree.root != nullptr) rb_tree.freeTree(rb_tree.root);
    rb_tree.moveTree(std::move(s.rb_tree));
  }
  return *this;
}

// Функция доступа к элементу с проверкой границ
template <typename Key, typename T>
T &map<Key, T>::at(const Key &key) {
  Node<key_type, value_type> *node = rb_tree.search(key);
  if (node == nullptr)
    throw std::out_of_range("Key wasn't found in map structure");
  return node->value.second;
}

// Доступ к элементу или его вставка
template <typename Key, typename T>
T &map<Key, T>::operator[](const Key &key) {
  Node<key_type, value_type> *node = rb_tree.search(key);
  if (node == nullptr) {
    value_type new_pair(key, T());
    node = rb_tree.insert(key, new_pair);
  }
  return node->value.second;
}

// Возврат итератора на первый элемент
template <typename Key, typename T>
typename map<Key, T>::iterator map<Key, T>::begin() {
  return iterator(rb_tree.minimum(rb_tree.getRoot()), rb_tree);
}

// Возврат итератора на позицию после конечного элемента
template <typename Key, typename T>
typename map<Key, T>::iterator map<Key, T>::end() {
  return iterator(nullptr, rb_tree);
}

// Возврат константного итератора на первый элемент
template <typename Key, typename T>
typename map<Key, T>::const_iterator map<Key, T>::begin() const {
  return const_iterator(rb_tree.minimum(rb_tree.getRoot()),
                        const_cast<RBTree<key_type, value_type> &>(rb_tree));
}

// Возврат константного итератора на позицию после конечного элемента
template <typename Key, typename T>
typename map<Key, T>::const_iterator map<Key, T>::end() const {
  return const_iterator(nullptr,
                        const_cast<RBTree<key_type, value_type> &>(rb_tree));
}

// Проверка map на пустоту
template <typename Key, typename T>
bool map<Key, T>::empty() const {
  return rb_tree.size == 0;
}

// Возврат текущего размера map
template <typename Key, typename T>
typename map<Key, T>::size_type map<Key, T>::size() const {
  return rb_tree.getSize();
}

// Возврат максимального возможного размера map
template <typename Key, typename T>
typename map<Key, T>::size_type map<Key, T>::max_size() const {
  return size_t(-1) / (sizeof(BinaryTree)) / 9;
}

// Очистка map
template <typename Key, typename T>
void map<Key, T>::clear() {
  if (rb_tree.root != nullptr) {
    rb_tree.freeTree(rb_tree.root);
    rb_tree.root = nullptr;
    rb_tree.size = 0;
  }
}

// Вставка нового элемента в map
template <typename Key, typename T>
std::pair<typename map<Key, T>::iterator, bool> map<Key, T>::insert(
    const value_type &value) {
  Node<key_type, value_type> *node = rb_tree.search(value.first);
  if (node == nullptr) {
    node = rb_tree.insert(value.first, value);
    return std::make_pair(iterator(node, rb_tree), true);
  }
  return std::make_pair(iterator(node, rb_tree), false);
}

// Вставка нового элемента по ключу и значению
template <typename Key, typename T>
std::pair<typename map<Key, T>::iterator, bool> map<Key, T>::insert(
    const Key &key, const T &obj) {
  Node<Key, std::pair<const Key, T>> *node = rb_tree.search(key);
  if (node == nullptr) {
    Node<Key, std::pair<const Key, T>> *newNode =
        rb_tree.insert(key, {key, obj}, false);
    return {iterator(newNode, rb_tree), true};
  }
  return {iterator(node, rb_tree), false};
}

// Вставка или присваивание элемента
template <typename Key, typename T>
std::pair<typename map<Key, T>::iterator, bool> map<Key, T>::insert_or_assign(
    const Key &key, const T &obj) {
  Node<key_type, value_type> *node = rb_tree.search(key);
  if (node == nullptr) {
    value_type new_pair(key, obj);
    node = rb_tree.insert(key, new_pair);
    return std::make_pair(iterator(node, rb_tree), true);
  } else {
    node->value.second = obj;
    return std::make_pair(iterator(node, rb_tree), false);
  }
}

// Удаление элемента по заданному итератору
template <typename Key, typename T>
void map<Key, T>::erase(iterator pos) {
  if (pos != end()) {
    rb_tree.remove(pos->first);
  }
}

// Удаление элемента в диапазоне двух итераторов
template <typename Key, typename T>
void map<Key, T>::erase(iterator pos1, iterator pos2) {
  while (pos1 != pos2) {
    iterator temp = pos1++;
    erase(temp);
  }
}

// Удаление элемента по ключу
template <typename Key, typename T>
void map<Key, T>::erase(const Key &key) {
  Node<key_type, value_type> *node = rb_tree.search(key);
  if (node != nullptr) rb_tree.removeByNode(node);
}

// Изменение содержимого двух map
template <typename Key, typename T>
void map<Key, T>::swap(map &other) {
  std::swap(rb_tree.root, other.rb_tree.root);
  std::swap(rb_tree.size, other.rb_tree.size);
}

// Объединение содержимого двух map
template <typename Key, typename T>
void map<Key, T>::merge(map &other) {
  rb_tree.merge(other.rb_tree);
}

// Проверка наличия элемента с заданным ключом
template <typename Key, typename T>
bool map<Key, T>::contains(const Key &key) const {
  return (rb_tree.search(key)) ? true : false;
}

// Поиск итератора на элемент с заданным ключом
template <typename Key, typename T>
typename map<Key, T>::iterator map<Key, T>::find(const Key &key) {
  Node<key_type, value_type> *node = rb_tree.search(key);
  return iterator(node, rb_tree);
}

// Поиск указанного значения и возврат количество раз его появления
template <typename Key, typename T>
typename map<Key, T>::size_type map<Key, T>::count(const Key &key) {
  return rb_tree.search(key) != nullptr ? 1 : 0;
}

// Функция бинарного поиска, находящая наименьший элемент за лог время больше
// заданного значения или равного ему
template <typename Key, typename T>
typename map<Key, T>::iterator map<Key, T>::lower_bound(const Key &key) {
  Node<key_type, value_type> *node = rb_tree.lower_bound_node(key);
  return iterator(node, rb_tree);
}

// Функция бинарного поиска, находящая наибольший элемент за лог время больше
// заданного значения или равного ему
template <typename Key, typename T>
typename map<Key, T>::iterator map<Key, T>::upper_bound(const Key &key) {
  Node<key_type, value_type> *node = rb_tree.upper_bound_node(key);
  return iterator(node, rb_tree);
}

// Вставка нескольких элементов в map с perfect forwarding
template <typename Key, typename T>
template <typename... Args>
std::vector<std::pair<typename map<Key, T>::iterator, bool>>
map<Key, T>::insert_many(Args &&...args) {
  std::vector<std::pair<iterator, bool>> result;
  if constexpr (sizeof...(Args) > 0) {
    (([&] {
       auto res = insert(std::forward<Args>(args));
       result.emplace_back(res);
     }()),
     ...);
  }
  return result;
}

#endif  // S21_CONTAINERS_MAP_TPP_