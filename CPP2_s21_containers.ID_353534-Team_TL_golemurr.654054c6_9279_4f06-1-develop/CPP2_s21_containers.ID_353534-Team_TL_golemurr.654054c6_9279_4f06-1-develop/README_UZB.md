# s21_containers

s21_containers.h kutubxonasini amalga oshirish. 

💡 **Ushbu loyiha haqida biz bilan fikr-mulohazalaringizni baham ko’rish uchun [bu yerni bosing](https://new.oprosso.net/p/4cb31ec3f47a4596bc758ea1861fb624).** Bu anonim bo’lib, jamoamizga ta’limni yaxshilashga yordam beradi. Loyihani tugatgandan so'ng darhol so'rovnomani to'ldirishni tavsiya qilamiz.

## Contents

1. [Chapter I](#chapter-i) \
    1.1. [Introduction](#introduction)
2. [Chapter II](#chapter-ii) \
    2.1. [Information](#information)
3. [Chapter III](#chapter-iii) \
    3.1. [Part 1](#part-1-s21_containersh-kutubxonasini-amalga-oshirish)  
    3.2. [Part 2](#part-2-qoshimcha-s21_containersplush-kutubxonasini-amalga-oshirish)  
    3.3. [Part 3](#part-3-qoshimcha-insert_many-usullarni-amalga-oshirish)


## Chapter I

![s21_containers](misc/images/s21_containers.png)

Yer sayyorasi, katta konteynerlar orasidagi portning qayeridadir, 1993-yil 29-oktabr.*

*– Seningcha, Byorn buni standartga qo'shishga rozi bo'ladimi?*

*-- Albatta. Bundan tashqari, u bir necha yil oldin shablonlarga qiziqqan edi, lekin o’shanda ishlab chiqilayotgan kutubxonaning yetarlicha ishonchliligiga erisha olmagan edik, – dedi oq ko'ylak kiygan va “A. Stepanov” ismi yozilgan HP Labs beyjiki taqqan o'rta yoshli erkak.*

*– Umumlashtirilgan dasturlash haqidagi taqdimotingni tomosha qildim. Bu g'oya haqiqatan ham ta'sirli, ammo yangi standartni yil oxirigacha chiqarishni xohlaymiz. Bu sezilarli yaxshilanishlarni talab qiladi ...*

*-- Menimcha, yangi standart ustida ishlashni to'liq bajarish uchun ularga aynan shu narsa kerak edi. Bundan tashqari, aytganingdek, g'oya ta'sirli. Biroq, yaxshilanishlar kerak. Taqdimotning muvaffaqiyatiga to'liq ishonch hosil qilish uchun biz umuman yondashuv va shablonlardan foydalanishning bir nechta misollarini keltirishimiz kerak. Yagona bog'langan ro'yxatlar tuzilishi bilan tanishsan-ku, to’g’rimi?*

*– Ha, menimcha, sening fikringni tushuna boshladim. Misol sifatida umumiy ro'yxatlarni amalga oshirishni xohlaysan, shundaymi? Barcha turlar uchun bitta shablon sinfimi?*

*-- Nafaqat. Tasavvur qiling-a, har qanday konteynerni bir marta ta'riflash va keyin turli xil ma'lumotlar turlari va sinflari bilan ishlatish kifoya. Bu qancha vaqt, kuch va resurslarni tejaydi! Ro'yxatlar, lug'atlar, to’plamlar! – Embarkadero qirg'og'i bo'ylab sayr qilish yanada qiziqarli bo'ldi.*

*– Navbatlar va steklar... Jin ursin, bu ajoyib.*

*-- Aynan shunday. Bunday misollardan keyin kim kutubxonani o'z tili standartiga kiritishdan bosh tortadi?*

*– Ishga kirishishni boshladim. Hatto qiziqqan yigitlardan iborat kichik jamoani yig'ish mumkin bo'ladi. Ushbu misollarni amalga oshirish uchun qancha vaqtimiz bor?*

*-- San-Xosedagi taqdimot uchrashuvigacha taxminan ikki hafta qoldi, keyin...*


### Introduction

Ushbu loyiha doirasida sizdan C++ tilining asosiy standart konteyner sinflarini amalga oshiradigan shaxsiy kutubxonani yozish so'raladi: `list` (ro'yxat), `map` (lug'at), `queue` (navbat), `set` (to'plam), `stack` (stek) va `vector` (vektor). Amalga oshirish elementlar bilan ishlash, konteyner to'la yoki yo'qligini tekshirish va iteratsiyalash uchun standart usullar va atributlarning to'liq to'plamini taqdim etishi kerak. Qo'shimcha vazifa sifatida C++ konteyner kutubxonasidan yana bir nechta kamroq darajada qo'llaniladigan, lekin amalga oshirish tafsilotlari bilan farq qiladigan konteyner sinflarini amalga oshirish taklif etiladi.


## Chapter II

### Information
  
Ko'pchilik uchun "konteyner" so'zi intuitiv ravishda tushunarli bo'lib, inglizcha so’z contain – "saqlash" so'zidan kelib chiqqan. Dasturlashda ham xuddi shunday: konteynerlar bir xil turdagi obyektlar to'plamini – elementlarni saqlash uchun ishlatiladi. Biroq, juda ko'p konteyner sinflari mavjud. Buning sababi shundaki, konteyner sinflari saqlanadigan obyektlar to'plamini tashkil qilish va ular bilan o'zaro ta'sir qilish uchun taqdim etiluvchi usullar bilan farqlanadi. Masalan, ro'yxatlar (`list`) har qanday ob'ektni saqlaydi va to'plamlar (`set`) – faqat noyoblarini.

Turli xil vazifalarni yechish uchun bir xil konteynerdan foydalanish o'rniga, konteynerlarni ajratish zarurati shunchaki aniq funksional farqlargagina borib taqalmaydi. Ba'zi hollarda ro'yxatlardan foydalanish samaraliroq bo'ladi, masalan, berilgan masalani yechish jarayonida konteynerning o'rtasiga tez-tez elementlarni kiritish kerak bo'lganda, agar yangi elementlar faqat oxiriga qo'shilsa, navbatdan foydalanish tavsiya etiladi.
Har bir turdagi konteyner foydalanuvchiga quyidagi usullarni taqdim etishi kerak:


- standart konstruktorlar (asl sozlamalar bo’yicha konstruktor, nusxa ko‘chirish konstruktori, ko‘chirish konstruktori, ishga tushirish ro‘yxatiga ega konstruktor, “materials”ga qarang);

- konteyner elementlariga kirish usullari (masalan, i indeksli elementga kirishni amalga oshirish);

- konteynerning to‘liqligini tekshirish usullari (masalan, konteynerdagi elementlar soni, konteyner bo‘sh yoki yo‘qligini tekshirish);

- konteynerni o’zgartirish usullari (yangi elementlarni olib tashlash va qo'shish, konteynerni tozalash);

- konteyner iteratori bilan ishlash usullari.
  
Iteratorlar konteyner elementlariga kirishni ta'minlaydi. Muayyan iterator turi har bir konteyner uchun har xil bo'ladi. Bu konteyner sinflarida obyektlar to'plamini tashkil qilishning turli usullari, shuningdek, konteynerning haqiqiy amalga oshirilishi bilan bog'liq. Iteratorlar, C tilidagi massiv elementiga ko'rsatgichga o'xshash tarzda ishlaydigan shaklda amalga oshiriladi. Aynan mana shunday yondashuv iteratorlardan foydalanish orqali har qanday konteynerlar bilan xuddi shu tarzda o'zaro aloqada bo'lishga imkon beradi. `begin()` va `end()` usullari orqali konteynerlar mos ravishda konteynerning birinchi va oxirgi elementlaridan keyingisiga ishora qiluvchi iteratorlarni taqdim etadi.
iter iteratori ustida quyidagi operatsiyalar aniqlanadi:

- `*iter`: iterator tomonidan ko'rsatiluvchi elementni olish;

- `++iter`: keyingi elementga murojaat qilish uchun iteratorni oldinga siljitish;

- `--iter`: oldingi elementga murojaat qilish uchun iteratorni orqaga siljitish;

- `iter1 == iter2`: ikkita iterator bir xil elementga ishora qilsa, ular tengdir

- `iter1 != iter2`: ikkita iterator har xil elementga ishora qilsa, ular teng emasdir


Obyektlarni maxsus tashkillashtirish va kerakli usullarni taqdim etishdan tashqari, konteyner sinflarini amalga oshirish obyektlarni shablonlashni talab qiladi.

Shablonli sinflar yoki sinf shablonlari qo'shimcha tashqi parametrlarga bog'liq bo'lgan sinf yaratish kerak bo'lganda foydalaniladi, ular boshqa sinflar yoki ma'lumotlar turlari bo'lishi mumkin. Misol uchun, agar ro'yxat sinfini yaratish kerak bo'lsa, unda barcha mumkin bo'lgan element turlari uchun ro'yxatni amalga oshirishni qayta yozishdan qochish talab qilinadi. Albatta, parametr bilan bitta sinf yozish va bir vaqtning o'zida bir nechta aniq ro'yxat sinflarini olish qulayroqdir (belgilar ro'yxati, butun sonlar, haqiqiy, foydalanuvchi turlari va boshqalar). C++ da konteynerlar iteratorlar va ba'zi algoritmlar bilan birga aynan shu sababdan Standart Shablonlar kutubxonasiga (STL) kiritilgan.

Konteynerlar ikkita asosiy turga bo'linadi: ketma-ket va assotsiativ. Elementni ketma-ket konteynerlarda (`list`, `vector`, `array`, `stack`, `queue`) topish uchun butun konteynerni ketma-ket ko’rish kerak, assotsiativ konteynerlarda esa (`map`, `set`, `multiset`) qiymat bilan bog'liq kalitga murojaat qilish kifoya.


## Chapter III

- Dastur C++20 standarti C++ tilida gcc kompilyatoridan foydalangan holda ishlab chiqilishi kerak.

- Dastur kodi src papkasida joylashgan bo'lishi kerak.

- Kod yozishda Google Stylega amal qilinishi kerak.

- Iteratorlardan albatta foydalaning.

- Sinflar albatta shablonli bo'lishi kerak.

- Sinflar `s21` nom maydoni ichida amalga oshirilishi kerak

- GTest kutubxonasidan foydalanib, konteyner sinfi usullarining to'liq unit-testlar bilan qoplanishini tayyorlang.

- Standart shablonlar kutubxonasi (STL) amalga oshirilishini nusxalash taqiqlanadi.

- Standart shablonlar kutubxonasi (STL) ish mantig'iga rioya qilish kerak (tekshirish, xotira bilan ishlash va favqulodda vaziyatlarda xatti-harakatlar nuqtai nazaridan).

### Part 1. s21_containers.h kutubxonasini amalga oshirish

Siz s21_containers.h kutubxonasining sinflarini amalga oshirishingiz kerak (xususiyatlar materiallarning tegishli bo'limlarida ko'rsatilgan, "Asosiy konteynerlar" bandiga qarang).
Sinflar ro'yxati: `list` (ro'yxat), `map` (lug'at), `queue` (navbat), `set` (to'plam), `stack` (stek), `vector`
- Yechimni kerakli konteynerlar (`s21_list.h`, `s21_map.h` va boshqalar) ni amalga oshirish bilan boshqa sarlavha fayllarini o'z ichiga oluvchi `s21_containers.h` sarlavhali fayli sifatida loyihalang.
- Yozma kutubxona testlari uchun Makefile ni nazarda tuting (clean, test maqsadlari bilan).
- Konteynerlarning klassik amalga oshirilishini asos qilib olgan yaxshidir, lekin amalga oshirishning yakuniy tanlovi erkin bo'lib qoladi. Ro'yxatdan tashqari – u massiv orqali emas, balki ro'yxat tuzilmasi orqali amalga oshirilishi kerak.

*Maslahat*: Siz bir xil konteyner usullarini amalga oshirilishini asosiy sinflarga ajratishingiz mumkin. Masalan, navbat va stek uchun yoki ro'yxat va vektor uchun. Ierarxik qurilishning mumkin bo'lgan misollaridan biri sifatida, materiallarda STL kutubxonasining UML diagrammasi taqdim etiladi. Biroq, sizning amalga oshirishingiz ushbu UML diagrammasi bilan qattiq bog'langan bo'lishi shart emas.

### Part 2. Qo'shimcha. s21_containersplus.h kutubxonasini amalga oshirish

Siz `s21_containersplus.h` kutubxonasining funksiyalarini amalga oshirishingiz kerak (xususiyatlar materiallarning tegishli bo'limlarida ko'rsatilgan, **"Qo'shimcha konteynerlar"** bandiga qarang). Qo'shimcha ravishda amalga oshirilishi kerak bo'lgan sinflar ro'yxati: `array` (massiv), `multiset` (multito’plam).
- Yechimni kerakli konteynerlar (`s21_array.h`, `s21_multiset.h`) ni amalga oshirish bilan boshqa sarlavha fayllarini o'z ichiga oluvchi `s21_containersplus.h` sarlavhali fayli sifatida loyihalang. 
- Yozma kutubxona testlari uchun Makefile ni nazarda tuting (clean, test maqsadlari bilan).
- Konteynerlarning klassik amalga oshirilishini asos qilib olgan yaxshidir, lekin amalga oshirishning yakuniy tanlovi erkin bo'lib qoladi. 


### Part 3. Qo'shimcha. `insert_many` usullarni amalga oshirish

Jadvalga muvofiq sinflarni tegishli usullar bilan to'ldirishingiz kerak:

| Modifiers      | Definition                                      | Containers |
|----------------|-------------------------------------------------| -------------------------------------------|
| `iterator insert_many(const_iterator pos, Args&&... args)`          | Inserts new elements into the container directly before `pos`.  | List, Vector. |
| `void insert_many_back(Args&&... args)`          | Appends new elements to the end of the container.  | List, Vector, Queue, Stack. |
| `void insert_many_front(Args&&... args)`          | Appends new elements to the top of the container.  | List. |
| `vector<std::pair<iterator,bool>> insert_many(Args&&... args)`          | Inserts new elements into the container.  | Map, Set, Multiset. |

E'tibor bering, tegishli konteynerning allaqachon yaratilgan elementlari ushbu konteynerga kiritilishi kerak bo'lgan argumentlar sifatida uzatiladi.

*1-maslahat*: Shuni yodda tutingki, ushbu usullarning har biri Args&&... args – Parameter pack konstruksiyasidan foydalanadi. Bu konstruksiya funksiya yoki usulga o‘zgaruvchan sonli parametrlarni o‘tkazish imkonini beradi. Ya'ni, iterator sifatida belgilangan usulni chaqirganda `iterator insert_many(const_iterator pos, Args&&... args)`, `insert_many(pos, arg1, arg2)`, va `insert_many(pos, arg1, arg2, arg3)` sifatida yozilishi mumkin.

*2-maslahat*: Turli holatlar, shu jumladan chetki holatlar uchun usullarni sinab ko'rishni unutmang.
