## Asosiy konteynerlar 

### List

<details>
  <summary>Umumiy ma’lumot</summary>
<br />

List (ro’yxat) – bu ixtiyoriy o'lchamdagi elementlar to'plamini ko'rsatkichlar bilan ketma-ket bog'langan tugunlar sifatida saqlaydigan ketma-ket konteynerdir. Har bir tugun ro'yxat elementiga mos keladigan qiymatni va keyingi elementga ko'rsatgichni saqlaydi. Idishning bunday joylashishi, masalan, statik massivda bo'lgani kabi, qat'iy belgilangan o'lchamdan uzoqlashishga imkon beradi va konteynerga yangi element qo'shish jarayonini yanada intuitiv qiladi.

![](misc/images/list_01.png)

Yuqorida to'rtta elementdan iborat ro'yxatga misol keltirilgan. Har bir ro'yxat elementi ikkita maydonga ega struktura sifatida taqdim etiladi: tugun qiymati va keyingi ro'yxat elementiga ko'rsatgich. Ro'yxatning oxirgi elementi hech narsani bildirmaydi.

![](misc/images/list_02.png)

Bunday ro’yxat moslamasi ro'yxatning oxiriga ham, o'rtasiga ham oddiy usulda (kaskadli siljishsiz) qo'shish imkonini beradi. Ro'yxatdagi ma'lum bir pozitsiyaga element qo'shganda, ushbu pozitsiyadan keyingi keyingi elementga ishora qilish uchun yangi tugun yaratiladi, shundan so'ng oldingi elementning ko'rsatkichi yangisiga o'tkaziladi.

![](misc/images/list_03.png)

Element ro’yxatdan o’chirilganda, ma’lum bir tugun bo’shatiladi, qo’shni elementlarning ko'rsatkichlari esa qiymatini o'zgartiradi: oldingi element ko'rsatgichni o'chirilgandan keyin keyingi elementga o'tkazadi.

Ro’yxatlar bir bog’lamchili yoki ikki bog’lamchili bo’lad. Bir bog’lamchili ro’yxat – bu ro’yxat bo'lib, uning har bir tugunida faqat bitta ko'rsatgich saqlanadi: ro'yxatning keyingi elementiga (yuqoridagi misol). Ikki bog’lamchili ro'yxatda har bir tugun oldingi elementga qo'shimcha ko'rsatgichni saqlaydi. C++ da ro'yxat konteynerining standart bajarilishi ikki bog’lamchili ro'yxatdan foydalanadi.

Konteyner sinfi obyekti ro'yxatning “boshi” va “dumi” elementlariga ishora qiluvchi ro'yxat boshi va quyruq ko'rsatkichlarini saqlaydi. List konteyneri faqat “bosh” va “dum”ga to'g'ridan-to'g'ri kirishni ta'minlaydi, lekin ro'yxatning istalgan joyiga elementlarni qo'shish va olib tashlash imkonini beradi.
</details>

<details>

<summary>Spetsifikatsiya</summary>
<br />

*List Member type*

Ushbu jadvalda sinf kodini o'qishni osonlashtirish uchun qabul qilingan sinf ichidagi turdagi bekor qilishlar (standart STL kutubxonasi uchun odatiy) ko’rsatilgan:

| Member type            | definition                                                                             |
|------------------------|----------------------------------------------------------------------------------------|
| `value_type`             | `T` defines the type of an element (T is a template parameter)                                  |
| `reference`              | `T &` defines the type of the reference to an element                                                             |
| `const_reference`        | `const T &` defines the type of the constant reference                                         |
| `iterator`               | internal class `ListIterator<T>` defines the type for iterating through the container                                                 |
| `const_iterator`         | internal class `ListConstIterator<T>` defines the constant type for iterating through the container                                           |
| `size_type`              | `size_t` defines the type of the container size (standard type is size_t) |

*List Functions*

Ushbu jadvalda sinf bilan o’zaro aloqa qilishning asosiy ommaviy usullari keltirilgan:

| Functions      | Definition                                      |
|----------------|-------------------------------------------------|
| `list()`  | default constructor, creates an empty list                                  |
| `list(size_type n)`  | parameterized constructor, creates the list of size n                                 |
| `list(std::initializer_list<value_type> const &items)`  | initializer list constructor, creates a list initizialized using std::initializer_list<T>    |
| `list(const list &l)`  | copy constructor  |
| `list(list &&l)`  | move constructor  |
| `~list()`  | destructor  |
| `operator=(list &&l)`      | assignment operator overload for moving an object                                |

*List Element access*

Ushbu jadvalda sinf a’zolariga kirishning ommaviy usullari keltirilgan:

| Element access | Definition                                      |
|----------------|-------------------------------------------------|
| `const_reference front()`          | access the first element                        |
| `const_reference back()`           | access the last element                         |

*List Iterators*

Ushbu jadvalda sinf elementlarini takrorlashning umumiy usullari ro'yxati keltirilgan (iteratorlarga kirish):


| Iterators      | Definition                                      |
|----------------|-------------------------------------------------|
| `iterator begin()`    | returns an iterator to the beginning            |
| `iterator end()`        | returns an iterator to the end                  |

*List Capacity*

Ushbu jadvalda konteyner to’lganligi to’g’risidagi ma'lumotlariga kirishning umumiy usullari keltirilgan:

| Capacity       | Definition                                      |
|----------------|-------------------------------------------------|
| `bool empty()`          | checks whether the container is empty           |
| `size_type size()`           | returns the number of elements                  |
| `size_type max_size()`       | returns the maximum possible number of elements |

*List Modifiers*

Ushbu jadvalda konteynerni o'zgartirishning umumiy usullari keltirilgan:

| Modifiers      | Definition                                      |
|----------------|-------------------------------------------------|
| `void clear()`          | clears the contents                             |
| `iterator insert(iterator pos, const_reference value)`         | inserts element into concrete pos and returns the iterator that points to the new element     |
| `void erase(iterator pos)`          | erases an element at pos                                 |
| `void push_back(const_reference value)`      | adds an element to the end                      |
| `void pop_back()`   | removes the last element        |
| `void push_front(const_reference value)`      | adds an element to the head                      |
| `void pop_front()`   | removes the first element        |
| `void swap(list& other)`                   | swaps the contents                                                                     |
| `void merge(list& other)`                   | merges two sorted lists                                                                      |
| `void splice(const_iterator pos, list& other)`                   | transfers elements from list other starting from pos             |
| `void reverse()`                   | reverses the order of the elements              |
| `void unique()`                   | removes consecutive duplicate elements               |
| `void sort()`                   | sorts the elements                |

</details>

### Map

<details>
  <summary>Umumiy ma’lumot </summary>
<br />

Map (lug’at) – o’sish bo'yicha tartiblangan kalit-qiymat juftlarini o'z ichiga olgan assotsiativ konteyner. Ya'ni, har bir element qandaydir noyob kalit bilan bog'langan va uning lug'atdagi o'rni kaliti bilan belgilanadi. Lug'atlar elementlarni boshqa qiymat bilan (indeks emas) bog'lash kerak bo'lganda foydalidir. Misol uchun, korxona asbob-uskuna sotib oladi va har bir element qayta-qayta sotib olinishi kerak. Bunday holda, bir juft pozitsiya identifikatori - xarid hajmi bilan lug'atdan foydalanish qulay. Bu yerda identifikator nafaqat raqam, balki satr ham bo'lishi mumkin. Shunday qilib, lug'atda qidirish massivdagi kabi indeks bo'yicha emas, balki identifikator - so'z bo'yicha amalga oshiriladi.

![](misc/images/map_01.png)

Qanday qilib lug'at kalit bo'yicha juftliklarga kirishga imkon beradi va har doim saralangan bo’ladi? Aslida, lug'at binar qidiruv daraxti tuzilishiga ega (C++ dasturida bu qizil-qora daraxt), bu sizga lug'atga elementlarni darhol tartibda qo'shish va lug'at elementlarni barcha elementlarni to'g'ridan-to'g'ri ko'rib chiqishdan ko'ra samaraliroq topish imkonini beradi.

![](misc/images/map_02.png)

Binar qidiruv daraxti – ham tugunlardan tashkil topgan tuzilma, lekin har bir tugunda ikkita boshqa tugunga - "avlodlarga" ikkita ko'rsatgich mavjud. Bunday holda, joriy tugun "ota-ona" deb ataladi. Umuman olganda, ikkilik qidiruv daraxti joriy tugunning bolalari bo'lsa, chap bola pastki qiymatga ega elementni va o'ngdagi eng yuqori qiymatga ega elementni o'z ichiga olishini ta'minlaydi. Shunday qilib, daraxtdan ma'lum bir elementni qidirish uchun maxsus komporator funktsiyasidan foydalangan holda qidirilayotgan qiymatni joriy tugunning qiymati bilan solishtirish kifoya qiladi (lug'atda bu funktsiya kalit turiga bog'liq). Agar u kattaroq bo'lsa, siz “o’ng” bolaga, kamroq - chapga o'tishingiz kerak va agar qiymat teng bo'lsa, kerakli element topiladi.

![](misc/images/map_03.png)

</details>

<details>
  <summary>Spetsifikatsiya</summary>
<br />

*Map Member type*

Ushbu jadvalda sinf kodini o'qishni osonlashtirish uchun qabul qilingan sinf ichidagi turdagi bekor qilishlar (standart STL kutubxonasi) ko'rsatilgan:

| Member type            | Definition                                                                             |
|------------------------|----------------------------------------------------------------------------------------|
| `key_type`               | `Key` the first template parameter (Key)                                                     |
| `mapped_type`           | `T` the second template parameter (T)                                                      |
| `value_type`             | `std::pair<const key_type,mapped_type>` Key-value pair                                                      |
| `reference`              | `value_type &` defines the type of the reference to an element                                                             |
| `const_reference`        | `const value_type &` defines the type of the constant reference                                         |
| `iterator`               | internal class `MapIterator<K, T>` or `BinaryTree::iterator` as internal iterator of tree subclass; defines the type for iterating through the container                                                 |
| `const_iterator`         | internal class `MapConstIterator<K, T>` or `BinaryTree::const_iterator` as internal const iterator of tree subclass; defines the constant type for iterating through the container                                           |
| `size_type`              | `size_t` defines the type of the container size (standard type is size_t) |

*Map Member functions*

Ushbu jadvalda sinf bilan o’zaro aloqa qilishning asosiy ommaviy usullari keltirilgan:

| Member functions      | Definition                                      |
|----------------|-------------------------------------------------|
| `map()`  | default constructor, creates an empty map                                 |
| `map(std::initializer_list<value_type> const &items)`  | initializer list constructor, creates the map initizialized using std::initializer_list<T>    |
| `map(const map &m)`  | copy constructor  |
| `map(map &&m)`  | move constructor  |
| `~map()`  | destructor  |
| `operator=(map &&m)`      | assignment operator overload for moving an object                                |

*Map Element access*

Ushbu jadvalda sinf elementlariga kirishning ommaviy usullari keltirilgan:

| Element access         | Definition                                                                             |
|------------------------|----------------------------------------------------------------------------------------|
| `T& at(const Key& key)`                     | access a specified element with bounds checking                                          |
| `T& operator[](const Key& key)`             | access or insert specified element                                                     |

*Map Iterators*

Ushbu jadvalda sinf elementlarini takrorlashning ommaviy usullari ro'yxati keltirilgan (iteratorlarga kirish):

| Iterators              | Definition                                                                             |
|------------------------|----------------------------------------------------------------------------------------|
| `iterator begin()`            | returns an iterator to the beginning                                                   |
| `iterator end()`                | returns an iterator to the end                                                         |

*Map Capacity*

Ushbu jadvalda konteyner to’lganligi to’g’risidagi ma’lumotlariga kirishning ommaviy usullari keltirilgan:

| Capacity               | Definition                                                                             |
|------------------------|----------------------------------------------------------------------------------------|
| `bool empty()`                  | checks whether the container is empty                                                  |
| `size_type size()`                   | returns the number of elements                                                         |
| `size_type max_size()`               | returns the maximum possible number of elements                                        |

*Map Modifiers*

Ushbu jadvalda konteynerni o'zgartirishning ommaviy usullari keltirilgan:

|------------------------|----------------------------------------------------------------------------------------|
| `void clear()`                  | clears the contents                                                                    |
| `std::pair<iterator, bool> insert(const value_type& value)`                 | inserts a node and returns an iterator to where the element is in the container and bool denoting whether the insertion took place                                        |
| `std::pair<iterator, bool> insert(const Key& key, const T& obj)`                 | inserts a value by key and returns an iterator to where the element is in the container and bool denoting whether the insertion took place    |
| `std::pair<iterator, bool> insert_or_assign(const Key& key, const T& obj);`       | inserts an element or assigns to the current element if the key already exists         |
| `void erase(iterator pos)`                  | erases an element at pos                                                                        |
| `void swap(map& other)`                   | swaps the contents                                                                     |
| `void merge(map& other);`                  | splices nodes from another container                                                   |

*Map Lookup*

Ushbu jadvalda konteynerlarni ko'rib chiqishni amalga oshiradigan ommaviy usullar ro'yxati keltirilgan:

| Lookup                 | Definition                                                                             |
|------------------------|----------------------------------------------------------------------------------------|
| `bool contains(const Key& key)`                  | checks if there is an element with key equivalent to key in the container                                   |

</details>

### Queue

<details>
  <summary>Umumiy ma’lumot</summary>
<br />

Queue (navbat) - FIFO (First-In, First-Out) printsipiga muvofiq tashkil etilgan elementlarga ega konteyner. Xuddi ro’yxat kabi, navbat konteyner sinfining obyekti navbatning “dumi” va “boshi” ga ko'rsatgichlarni o'z ichiga oladi, ammo o'chirish “bosh” dan qat'iy ravishda amalga oshiriladi va yozish, ya'ni yangi elementlarni qo'shish qat'iy ravishda amalga oshiriladi. “dumga” Navbatni elementlar bir uchidan kirib, ikkinchi uchidan chiqib ketadigan quvur turi deb o'ylash qulay.

![](misc/images/queue01.png)

</details>

<details>
  <summary>Spesifitatsiya</summary>
<br /> 

*Queue Member type*

Ushbu jadvalda sinf kodini o'qishni osonlashtirish uchun qabul qilingan sinf ichidagi turdagi bekor qilishlar (standart STL kutubxonasi) ko'rsatilgan:

| Member type      | Definition                                       |
|------------------|--------------------------------------------------|
| `value_type`       | `T` the template parameter T                   |
| `reference`              | `T &` defines the type of the reference to an element                                                             |
| `const_reference`        | `const T &` defines the type of the constant reference                                         |
| `size_type`        | `size_t` defines the type of the container size (standard type is size_t) |

*Queue Member functions*

Ushbu jadvalda sinf bilan o'zaro aloqa qilishning asosiy ommaviy usullari keltirilgan:

| Functions      | Definition                                      |
|----------------|-------------------------------------------------|
| `queue()`  | default constructor, creates an empty queue                                 |
| `queue(std::initializer_list<value_type> const &items)`  | initializer list constructor, creates queue initizialized using std::initializer_list<T>    |
| `queue(const queue &q)`  | copy constructor  |
| `queue(queue &&q)`  | move constructor  |
| `~queue()`  | destructor  |
| `operator=(queue &&q)`      | assignment operator overload for moving an object                                |

*Queue Element access*

Ushbu jadvalda sinf elementlariga kirishning ommaviy usullari keltirilgan:

| Element access | Definition                                      |
|----------------|-------------------------------------------------|
| `const_reference front()`          | access the first element                        |
| `const_reference back()`           | access the last element                         |

*Queue Capacity*

Ushbu jadvalda konteyner to’lganligi to’g’risida ma’lumotlariga kirishning ommaviy usullari keltirilgan:

| Capacity       | Definition                                      |
|----------------|-------------------------------------------------|
| `bool empty()`          | checks whether the container is empty           |
| `size_type size()`           | returns the number of elements                  |

*Queue Modifiers*

Ushbu jadvalda konteynerni o'zgartirishning ommaviy usullari keltirilgan:

| Modifiers        | Definition                                       |
|------------------|--------------------------------------------------|
| `void push(const_reference value)`             | inserts an element at the end                       |
| `void pop()`              | removes the first element                        |
| `void swap(queue& other)`             | swaps the contents                               |

</details>

### Set

<details>
  <summary>Umumiy ma’lumot </summary>
<br />

Set (ko’pchilik) – bu noyob elementlarning assotsiativ konteyneridir. Bu shuni anglatadiki, siz bir xil elementni to'plamga ikki marta qo'sha olmaysiz. O'rnatilgan konteyner assotsiativdir, chunki uning ichida map konteyneri (lug'at) kabi daraxt sifatida ham ifodalanadi va shunga mos ravishda elementlarni tartiblangan tartibda saqlaydi. Lug‘at va to‘plam o‘rtasidagi farq shundaki, to‘plamdagi yagona narsa kalit emas, balki qiymatning o‘zidir, xuddi daraxtdagi qiymatni izlash kalit bilan emas, balki qiymatning o‘zi tomonidan tekshirilganidek. Mavjud elementni to'plamga qo'shganda, tegishli istisno paydo bo'ladi.


Standart amalga oshirishda to'plamlar (kesishma, birlashma, ayirish va boshqalar) bo'yicha matematik amallar sinf darajasida amalga oshirilmaydi.

</details>

<details>
  <summary>Spesifikatsiya</summary>
<br />

*Set Member type*

Ushbu jadvalda sinf kodini o'qishni osonlashtirish uchun qabul qilingan sinf ichidagi turidagi bekor qilishlar (standart STL kutubxonasi uchun odatiy) ko'rsatilgan:

| Member type            | Definition                                                                             |
|------------------------|----------------------------------------------------------------------------------------|
| `key_type`               | `Key` the first template parameter (Key)                                                     |
| `value_type`             | `Key` value type (the value itself is a key)                                                    |
| `reference`              | `value_type &` defines the type of the reference to an element                                                             |
| `const_reference`        | `const value_type &` defines the type of the constant reference                                         |
| `iterator`               | internal class `SetIterator<T>` or `BinaryTree::iterator` as the internal iterator of tree subclass; defines the type for iterating through the container                                                 |
| `const_iterator`         | internal class `SetConstIterator<T>` or `BinaryTree::const_iterator` as the internal const iterator of tree subclass; defines the constant type for iterating through the container                                           |
| `size_type`              | `size_t` defines the type of the container size (standard type is size_t) |

*Set Member functions*

Ushbu jadvalda sinf bilan o'zaro aloqa qilishning asosiy ommaviy usullari keltirilgan:

| Member functions      | Definition                                      |
|----------------|-------------------------------------------------|
| `set()`  | default constructor, creates an empty set                                 |
| `set(std::initializer_list<value_type> const &items)`  | initializer list constructor, creates the set initizialized using std::initializer_list<T>    |
| `set(const set &s)`  | copy constructor  |
| `set(set &&s)`  | move constructor  |
| `~set()`  | destructor  |
| `operator=(set &&s)`      | assignment operator overload for moving an object                                |

*Set Iterators*

Ushbu jadvalda sinf elementlarini takrorlashning umumiy usullari ro'yxati keltirilgan (iteratorlarga kirish):

| Iterators              | Definition                                                                             |
|------------------------|----------------------------------------------------------------------------------------|
| `iterator begin()`            | returns an iterator to the beginning                                                   |
| `iterator end()`                | returns an iterator to the end                                                         |

*Set Capacity*

Ushbu jadvalda konteyner to’lganligi to’g’risida ma'lumotlariga kirishning umumiy usullari keltirilgan:

| Capacity       | Definition                                      |
|----------------|-------------------------------------------------|
| `bool empty()`          | checks whether the container is empty           |
| `size_type size()`           | returns the number of elements                  |
| `size_type max_size()`       | returns the maximum possible number of elements |

*Set Modifiers*

Ushbu jadvalda konteynerni o'zgartirishning umumiy usullari keltirilgan:

| Modifiers              | Definition                                                                             |
|------------------------|----------------------------------------------------------------------------------------|
| `void clear()`                  | clears the contents                                                                    |
| `std::pair<iterator, bool> insert(const value_type& value)`                 | inserts a node and returns an iterator to where the element is in the container and bool denoting whether the insertion took place                                        |
| `void erase(iterator pos)`                  | erases an element at pos                                                                        |
| `void swap(set& other)`                   | swaps the contents                                                                     |
| `void merge(set& other);`                  | splices nodes from another container                                                   |

*Set Lookup*

Ushbu jadvalda konteynerlarni ko'rib chiqishni amalga oshiradigan umumiy usullar ro'yxati keltirilgan:

| Lookup                 | Definition                                                                             |
|------------------------|----------------------------------------------------------------------------------------|
| `iterator find(const Key& key)`                   | finds an element with a specific key                                                        |
| `bool contains(const Key& key)`               | checks if the container contains an element with a specific key                             |

</details>

### Stack

<details>
  <summary>Umumiy ma’lumot</summary>
<br />

Stack (stek) – Bu LIFO (Last-In, First-Out) tamoyiliga muvofiq tashkil etilgan elementlarga ega konteyner. Stack konteyner sinfi obyekti stekning "boshiga" ko'rsatgichlarni o'z ichiga oladi va "bosh" dan qat'iy ravishda qo'shiladi. Stakni bir uchi muhrlangan shisha yoki quvur deb o'ylash qulay: idishga birinchi bo'lib qo'yilgan elementga o'tish uchun avval yuqoridagi barcha elementlarni olib tashlashingiz kerak.

![](misc/images/stack01.png)

</details>

<details>
  <summary>Spesifikatsiya</summary>
<br />

*Stack Member type*

Ushbu jadvalda sinf kodini o'qishni osonlashtirish uchun qabul qilingan sinf ichidagi turdagi bekor qilishlar (standart STL kutubxonasi uchun odatiy) ko'rsatilgan:

| Member type      | Definition                                       |
|------------------|--------------------------------------------------|
| `value_type`       | `T` the template parameter T                   |
| `reference`              | `T &` defines the type of the reference to an element                                                             |
| `const_reference`        | `const T &` defines the type of the constant reference                                         |
| `size_type`        | `size_t` defines the type of the container size (standard type is size_t) |

*Stack Member functions*

Ushbu jadvalda sinf bilan o'zaro aloqa qilishning asosiy ommaviy usullari keltirilgan:

| Functions      | Definition                                      |
|----------------|-------------------------------------------------|
| `stack()`  | default constructor, creates an empty stack                                 |
| `stack(std::initializer_list<value_type> const &items)`  | initializer list constructor, creates stack initizialized using std::initializer_list<T>    |
| `stack(const stack &s)`  | copy constructor  |
| `stack(stack &&s)`  | move constructor  |
| `~stack()`  | destructor  |
| `operator=(stack &&s)`      | assignment operator overload for moving an object                                |

*Stack Element access*

Ushbu jadvalda sinf a'zolariga kirishning ommaviy usullari keltirilgan:

This table contains the public methods for accessing the elements of the class:

| Element access   | Definition                                       |
|------------------|--------------------------------------------------|
| `const_reference top()`              | accesses the top element                         |

*Stack Capacity*

Ushbu jadvalda konteyner mazmuni ma'lumotlariga kirishning umumiy usullari keltirilgan:

| Capacity       | Definition                                      |
|----------------|-------------------------------------------------|
| `bool empty()`          | checks whether the container is empty           |
| `size_type size()`           | returns the number of elements                  |

*Stack Modifiers*

Ushbu jadvalda konteynerni o'zgartirishning umumiy usullari keltirilgan:

| Modifiers        | Definition                                       |
|------------------|--------------------------------------------------|
| `void push(const_reference value)`             | inserts an element at the top                       |
| `void pop()`              | removes the top element                        |
| `void swap(stack& other)`             | swaps the contents                               |

</details>

### Vector

<details>
  <summary>Umumiy ma’lumot</summary>
<br />

Vector (vektor) – intuitivroq ishlash uchun dinamik massivni inkapsulyatsiya qiluvchi ketma-ket konteynerdir. Ushbu konteyner standart dinamik massivlar kabi qo'lda xotirani boshqarishni talab qilmaydi, buning o'rniga `push_back()` va `insert()` usullari orqali ixtiyoriy sonli elementlarni qo'shish imkonini beradi va ro'yxatdan farqli o'laroq, istalgan elementga kirishingiz mumkin; konteyner to'g'ridan-to'g'ri, indeks bo'yicha. Vektordagi elementlar ketma-ket saqlanadi, bu vektor ustida faqat berilgan iterator orqali emas, balki ko'rsatgichni vektor elementiga qo'lda siljitish orqali ham takrorlash imkonini beradi. Shunday qilib, vektorning birinchi elementiga ko‘rsatgich argument sifatida oddiy massivni kutayotgan har qanday funksiyaga argument sifatida uzatilishi mumkin. Dinamik massiv o'lchamini o'zgartirish har safar element qo'shilganda yoki o'chirilganda sodir bo'lmaydi, lekin faqat belgilangan bufer hajmi oshib ketganda. Shunday qilib, vektor o'lcham uchun javob beradigan ikkita qiymatni saqlaydi: saqlangan massivning o'lchami (`size()` usuli) va buferning o'lchami (`capacity()` usuli).

</details>

<details>
  <summary>Spesifikatsiya</summary>
<br />

*Vector Member type*

Ushbu jadvalda sinf kodini o'qishni osonlashtirish uchun qabul qilingan sinf ichidagi turdagi bekor qilishlar (standart STL kutubxonasi uchun odatiy) ko'rsatilgan:

| Member type            | definition                                                                             |
|------------------------|----------------------------------------------------------------------------------------|
| `value_type`             | `T` defines the type of the element (T is template parameter)                                  |
| `reference`              | `T &` defines the type of the reference to an element                                                             |
| `const_reference`        | `const T &` defines the type of the constant reference                                         |
| `iterator`               | `T *` or internal class `VectorIterator<T>` defines the type for iterating through the container                                                 |
| `const_iterator`         | `const T *` or internal class `VectorConstIterator<T>` defines the constant type for iterating through the container                                           |
| `size_type`              | `size_t` defines the type of the container size (standard type is size_t) |


*Vector Member functions*

Ushbu jadvalda sinf bilan o'zaro aloqa qilishning asosiy ommaviy usullari keltirilgan:

| Functions      | Definition                                      |
|----------------|-------------------------------------------------|
| `vector()`  | default constructor, creates an empty vector                                 |
| `vector(size_type n)`  | parameterized constructor, creates the vector of size n                                 |
| `vector(std::initializer_list<value_type> const &items)`  | initializer list constructor, creates a vector initizialized using std::initializer_list<T>    |
| `vector(const vector &v)`  | copy constructor  |
| `vector(vector &&v)`  | move constructor  |
| `~vector()`  | destructor  |
| `operator=(vector &&v)`      | assignment operator overload for moving an object                                |

*Vector Element access*

Ushbu jadvalda sinf a'zolariga kirishning ommaviy usullari keltirilgan:

| Element access         | Definition                                                                             |
|------------------------|----------------------------------------------------------------------------------------|
| `reference at(size_type pos)`                     | access a specified element with bounds checking                                          |
| `reference operator[](size_type pos);`             | access a specified element                                                               |
| `const_reference front()`          | access the first element                        |
| `const_reference back()`           | access the last element                         |
| `T* data()`                   | direct access the underlying array                                                  |

*Vector Iterators*

Ushbu jadvalda sinf elementlarini takrorlashning ommaviy usullari ro'yxati keltirilgan (iteratorlarga kirish):

| Iterators      | Definition                                      |
|----------------|-------------------------------------------------|
| `iterator begin()`    | returns an iterator to the beginning            |
| `iterator end()`        | returns an iterator to the end                  |

*Vector Capacity*

Ushbu jadvalda konteyner mazmuni ma'lumotlariga kirishning umumiy usullari keltirilgan:

| Capacity               | Definition                                                                             |
|------------------------|----------------------------------------------------------------------------------------|
| `bool empty()`          | checks whether the container is empty           |
| `size_type size()`           | returns the number of elements                  |
| `size_type max_size()`       | returns the maximum possible number of elements |
| `void reserve(size_type size)`                | allocate storage of size elements and copies current array elements to a newely allocated array                                     |
| `size_type capacity()`               | returns the number of elements that can be held in currently allocated storage         |
| `void shrink_to_fit()`          | reduces memory usage by freeing unused memory                                          |

*Vector Modifiers*

Ushbu jadvalda konteynerni o'zgartirishning umumiy usullari keltirilgan:

| Modifiers      | Definition                                      |
|----------------|-------------------------------------------------|
| `void clear()`          | clears the contents                             |
| `iterator insert(iterator pos, const_reference value)`         | inserts elements into concrete pos and returns the iterator that points to the new element     |
| `void erase(iterator pos)`          | erases an element at pos                                 |
| `void push_back(const_reference value)`      | adds an element to the end                      |
| `void pop_back()`   | removes the last element        |
| `void swap(vector& other)`                   | swaps the contents                                                                     |

</details>
