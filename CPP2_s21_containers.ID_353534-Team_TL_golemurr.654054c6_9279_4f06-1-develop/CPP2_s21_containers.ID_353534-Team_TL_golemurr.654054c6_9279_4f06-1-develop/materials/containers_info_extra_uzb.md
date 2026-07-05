## Qo’shimcha konteynerlar

### Array

<details>
    <summary>Umumiy ma’lumot</summary>
<br />

Array (massiv) - bu statik massivni o’zida inkapsulalovchi ketma-ket konteyner. array konteyneriga yangi elementlarni qo'shish mumkin emas, faqat dastlab ko'rsatilganlarning qiymatini o'zgartirish mumkin. O'zaro ta'sir nuqtai nazaridan array konteyneri statik massivning aniq xususiyatlarini konteyner sinflarining asosiy afzalligi - ma'lumotlarning aniqroq tashkil etilishini o’zida mujassamlashtiradi. Masalan, Array konteyneri massiv o’lchamini saqlaydi va iteratorlarni taqdim etadi. vector kabi array ham xotiraning ketma-ketlik qismini egallaydi va standart C massivi kabi funksiyaga uzatilishi mumkin. array sinfining ikkinchi shablon argumenti uning haqiqiy o’lchamidir.

</details>

<details>
    <summary>Tasnifi</summary>
<br />

*Array Member type*

Ushbu jadvalda sinf kodining idrok qilinishini osonlashtirish uchun qabul qilingan sinf ichidagi turlarning qayta aniqlanishi (standart STL kutubxonasi uchun xos) sanab o’tilgan:

| Member type            | definition                                                                             |
|------------------------|----------------------------------------------------------------------------------------|
| `value_type`             | `T` defines the type of an element (T is template parameter)                                  |
| `reference`              | `T &` defines the type of the reference to an element                                                             |
| `const_reference`        | `const T &` defines the type of the constant reference                                         |
| `iterator`               | `T *` defines the type for iterating through the container                                                 |
| `const_iterator`         | `const T *` defines the constant type for iterating through the container                                           |
| `size_type`              | `size_t` defines the type of the container size (standard type is size_t) |

*Array Member functions*

Ushbu jadvalda sinflar bilan o’zaro aloqa qilish uchun asosiy ommaviy usullar sanab o’tilgan:


| Functions      | Definition                                      |
|----------------|-------------------------------------------------|
| `array()`  | default constructor, creates empty array                                 |
| `array(std::initializer_list<value_type> const &items)`  | initializer list constructor, creates array initizialized using std::initializer_list<T>    |
| `array(const array &a)`  | copy constructor  |
| `array(array &&a)`  | move constructor  |
| `~array()`  | destructor  |
| `operator=(array &&a)`      | assignment operator overload for moving object                                |

*Array Element access*

Ushbu jadvalda sinf elementlariga kirish uchun ommaviy usullar sanab o’tilgan:

| Element access         | Definition                                                                             |
|------------------------|----------------------------------------------------------------------------------------|
| `reference at(size_type pos)`                     | access specified element with bounds checking                                          |
| `reference operator[](size_type pos)`             | access specified element                                                               |
| `const_reference front()`          | access the first element                        |
| `const_reference back()`           | access the last element                         |
| `iterator data()`                   | direct access to the underlying array  

*Array Iterators*

Ushbu jadvalda sinf elementlari bo’yicha iteratsiyalash (iteratorlarga kirish) uchun ommaviy usullar sanab o’tilgan:


| Iterators      | Definition                                      |
|----------------|-------------------------------------------------|
| `iterator begin()`    | returns an iterator to the beginning            |
| `iterator end()`        | returns an iterator to the end                  |

*Array Capacity*

Ushbu jadvalda konteynerni to’ldirish to’g’risidagi ma’lumotga kirish uchun ommaviy usullar sanab o’tilgan:


| Capacity               | Definition                                                                             |
|------------------------|----------------------------------------------------------------------------------------|
| `bool empty()`          | checks whether the container is empty           |
| `size_type size()`           | returns the number of elements                  |
| `size_type max_size()`       | returns the maximum possible number of elements |

*Array Modifiers*

Ushbu jadvalda konteynerni o’zgartirish uchun ommaviy usullar sanab o’tilgan:


В этой таблице перечислены публичные методы для изменения контейнера:

| Modifiers      | Definition                                      |
|----------------|-------------------------------------------------|
| `void swap(array& other)`                   | swaps the contents                                |
| `void fill(const_reference value);`         | assigns the given value value to all elements in the container. |

</details>

### Multiset

<details>
    <summary>Umumiy ma'lumot</summary>
<br />

Multiset (multito’plam) – assotsiativ konteyner bo'lib, u to'plam mantig'ini takrorlaydi, lekin o’xshash elementlarni saqlashga imkon beradi. Bunday konteyner ro'yxat yoki vektordan, elementlar multito’plamga tushishda to'plamdagi kabi darhol saralanishi bilan farq qiladi. Biroq, xuddi to'plam kabi, multito’plam ham indeks bo'yicha elementga murojaat qilishga ruxsat bermaydi, balki multito’plamda takrorlanishi mumkin bo'lgan qiymat bo'yicha murojaat qilishni talab qiladi.

</details>

<details>
    <summary>Tasnif</summary>
<br />

*Multiset Member type*

Ushbu jadvalda sinf kodining idrok qilinishini osonlashtirish uchun qabul qilingan sinf ichidagi turlarning qayta aniqlanishi (standart STL kutubxonasi uchun xos) sanab o’tilgan:

| Member type            | Definition                                                                             |
|------------------------|----------------------------------------------------------------------------------------|
| `key_type`               | `Key` the first template parameter (Key)                                                     |
| `value_type`             | `Key` value type (the value itself is a key)                                                    |
| `reference`              | `value_type &` defines the type of the reference to an element                                                             |
| `const_reference`        | `const value_type &` defines the type of the constant reference                                         |
| `iterator`               | internal class `MultisetIterator<T>` or `BinaryTree::iterator` as internal iterator of tree subclass; defines the type for iterating through the container                                                 |
| `const_iterator`         | internal class `MultisetConstIterator<T>` or `BinaryTree::const_iterator` as internal const iterator of tree subclass; defines the constant type for iterating through the container                                           |
| `size_type`              | `size_t` defines the type of the container size (standard type is size_t) |

*Multiset Member functions*

Ushbu jadvalda sinflar bilan o’zaro aloqa qilish uchun asosiy ommaviy usullar sanab o’tilgan:

| Member functions      | Definition                                      |
|----------------|-------------------------------------------------|
| `multiset()`  | default constructor, creates empty set                                 |
| `multiset(std::initializer_list<value_type> const &items)`  | initializer list constructor, creates the set initizialized using std::initializer_list<T>    |
| `multiset(const multiset &ms)`  | copy constructor  |
| `multiset(multiset &&ms)`  | move constructor  |
| `~multiset()`  | destructor  |
| `operator=(multiset &&ms)`      | assignment operator overload for moving object                                |

*Multiset Iterators*

Ushbu jadvalda sinf elementlari bo’yicha iteratsiyalash (iteratorlarga kirish) uchun ommaviy usullar sanab o’tilgan:

| Iterators              | Definition                                                                             |
|------------------------|----------------------------------------------------------------------------------------|
| `iterator begin()`            | returns an iterator to the beginning                                                   |
| `iterator end()`                | returns an iterator to the end                                                         |

*Multiset Capacity*

Ushbu jadvalda konteynerni to’ldirish to’g’risidagi ma’lumotga kirish uchun ommaviy usullar sanab o’tilgan:

| Capacity       | Definition                                      |
|----------------|-------------------------------------------------|
| `bool empty()`          | checks whether the container is empty           |
| `size_type size()`           | returns the number of elements                  |
| `size_type max_size()`       | returns the maximum possible number of elements |

*Multiset Modifiers*

Ushbu jadvalda konteynerni o’zgartirish uchun ommaviy usullar sanab o’tilgan:

| Modifiers              | Definition                                                                             |
|------------------------|----------------------------------------------------------------------------------------|
| `void clear()`                  | clears the contents                                                                    |
| `iterator insert(const value_type& value)`                 | inserts node and returns iterator to where the element is in the container                                        |
| `void erase(iterator pos)`                  | erases element at pos                                                                        |
| `void swap(multiset& other)`                   | swaps the contents                                                                     |
| `void merge(multiset& other)`                  | splices nodes from another container                                                   |

*Multiset Lookup*

Ushbu jadvalda konteynerni ko’zdan kechirishni amalga oshiradigan ommaviy usullar sanab o’tilgan:

| Lookup                 | Definition                                                                             |
|------------------------|----------------------------------------------------------------------------------------|
| `size_type count(const Key& key)`                  | returns the number of elements matching specific key                                   |
| `iterator find(const Key& key)`                   | finds element with specific key                                                        |
| `bool contains(const Key& key)`               | checks if the container contains element with specific key                             |
| `std::pair<iterator,iterator> equal_range(const Key& key)`            | returns range of elements matching a specific key                                      |
| `iterator lower_bound(const Key& key)`            | returns an iterator to the first element not less than the given key                   |
| `iterator upper_bound(const Key& key)`            | returns an iterator to the first element greater than the given key                    |

</details>
