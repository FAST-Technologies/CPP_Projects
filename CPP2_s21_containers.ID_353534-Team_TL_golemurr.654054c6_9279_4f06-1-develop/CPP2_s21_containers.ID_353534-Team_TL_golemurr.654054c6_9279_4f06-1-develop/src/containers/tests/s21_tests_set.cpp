#include <gtest/gtest.h>

#include <set>
#include <string>

#include "../includes/s21_set.h"

using s21::set;

TEST(Set, DefaultConstructor) {
  set<int> a;
  std::set<int> b;
  EXPECT_EQ(a.size(), b.size());
  EXPECT_TRUE(a.empty());
}

TEST(Set, InitializerConstructor) {
  set<int> a{1, 2, 3, 3, 4, 3, 4};
  std::set<int> b{1, 2, 3, 3, 4, 1, 1};
  EXPECT_EQ(a.size(), b.size());
  auto it1 = a.begin();
  auto it2 = b.begin();
  for (; it1 != a.end() && it2 != b.end(); ++it1, ++it2) {
    EXPECT_EQ(*it1, *it2);
  }
}

TEST(Set, CopyConstructor) {
  set<int> a{1, 2, 3, 4, 3};
  set<int> b = a;
  EXPECT_EQ(a.size(), b.size());
  auto it1 = a.begin();
  auto it2 = b.begin();
  for (; it1 != a.end(); ++it1, ++it2) {
    EXPECT_EQ(*it1, *it2);
  }
}

TEST(Set, MoveConstructor) {
  set<int> a{1, 2, 3};
  set<int> b = std::move(a);
  std::set<int> ref{1, 2, 3};
  EXPECT_EQ(b.size(), ref.size());
  EXPECT_TRUE(a.empty());
  auto it1 = b.begin();
  auto it2 = ref.begin();
  for (; it1 != b.end() && it2 != ref.end(); ++it1, ++it2) {
    EXPECT_EQ(*it1, *it2);
  }
}

TEST(Set, CopyAssignment) {
  set<int> a{1, 2, 3};
  set<int> b;
  b = a;
  EXPECT_EQ(a.size(), b.size());
  auto it1 = a.begin();
  auto it2 = b.begin();
  for (; it1 != a.end(); ++it1, ++it2) {
    EXPECT_EQ(*it1, *it2);
  }
}

TEST(Set, MoveAssignment) {
  set<int> a{1, 2, 3};
  set<int> b{4, 5};
  b = std::move(a);
  EXPECT_EQ(b.size(), 3);
  EXPECT_TRUE(a.empty());
  auto it = b.begin();
  std::set<int> ref{1, 2, 3};
  auto it_ref = ref.begin();
  for (; it != b.end() && it_ref != ref.end(); ++it, ++it_ref) {
    EXPECT_EQ(*it, *it_ref);
  }
}

TEST(Set, Erase) {
  set<int> a{1, 4, 9, 16, 25};
  auto it = a.begin();
  ++it;
  a.erase(it);
  EXPECT_EQ(a.size(), 4);
  EXPECT_FALSE(a.contains(4));
}

TEST(Set, Swap) {
  set<int> a{9, 18};
  set<int> b{4, 7};
  a.swap(b);
  EXPECT_EQ(a.size(), 2);
  EXPECT_EQ(b.size(), 2);
  EXPECT_EQ(*a.begin(), 4);
  EXPECT_EQ(*b.begin(), 9);
}

TEST(Set, Merge) {
  set<int> a{1, 2, 3};
  set<int> b{2, 3};
  a.merge(b);
  EXPECT_EQ(a.size(), 3);
  EXPECT_FALSE(b.empty());
  EXPECT_TRUE(a.contains(1));
  EXPECT_TRUE(a.contains(2));
  EXPECT_TRUE(a.contains(3));
}

TEST(Set, Find) {
  set<int> a{2, 4, 6, 8};
  auto it = a.find(2);
  EXPECT_NE(it, a.end());
  EXPECT_EQ(*it, 2);
  auto it_not_found = a.find(10);
  EXPECT_EQ(it_not_found, a.end());
}

TEST(Set, Contains) {
  set<int> a{9, 18, 27};
  EXPECT_TRUE(a.contains(27));
  EXPECT_FALSE(a.contains(88));
}

TEST(Set, MaxSize) {
  set<int> a;
  EXPECT_GT(a.max_size(), a.size());
}

TEST(Set, Clear) {
  set<int> a{234, 567, 876};
  a.clear();
  EXPECT_TRUE(a.empty());
  EXPECT_EQ(a.size(), 0);
}

TEST(Set, BeginEnd) {
  set<int> a{2, 4, 5, 9};
  auto it = a.begin();
  EXPECT_EQ(*it, 2);
  auto end = a.end();
  --end;
  EXPECT_EQ(*end, 9);
}

TEST(Set, Empty) {
  set<int> a;
  EXPECT_TRUE(a.empty());
  a.insert(1);
  EXPECT_FALSE(a.empty());
}

TEST(Set, Size) {
  set<float> b{23.6, 54.6, 23.8};
  EXPECT_EQ(b.size(), 3);
  b.erase(b.begin());
  EXPECT_EQ(b.size(), 2);
}

TEST(Set, StringInitializerConstructor) {
  set<std::string> a{"abc", "def", "def", "ghi"};
  std::set<std::string> b{"abc", "def", "ghi"};
  EXPECT_EQ(a.size(), b.size());
  auto it1 = a.begin();
  auto it2 = b.begin();
  for (; it1 != a.end() && it2 != b.end(); ++it1, ++it2) {
    EXPECT_EQ(*it1, *it2);
  }
}

TEST(Set, CharInitializerConstructor) {
  set<char> a{'a', 'b', 'c', 'b'};
  std::set<char> b{'a', 'b', 'c'};
  EXPECT_EQ(a.size(), b.size());
  auto it1 = a.begin();
  auto it2 = b.begin();
  for (; it1 != a.end() && it2 != b.end(); ++it1, ++it2) {
    EXPECT_EQ(*it1, *it2);
  }
}

TEST(Set, InsertMany) {
  s21::set<int> my_set{1, 2};
  auto res = my_set.insert_many(3, 4, 2, 5);

  EXPECT_EQ(my_set.size(), 5);
  EXPECT_EQ(res.size(), 4);

  EXPECT_TRUE(res[0].second);
  EXPECT_TRUE(res[1].second);
  EXPECT_FALSE(res[2].second);
  EXPECT_TRUE(res[3].second);
}

TEST(SetInsertTest, InsertSingleElement) {
  s21::set<int> s;
  auto [it, inserted] = s.insert(42);
  EXPECT_TRUE(inserted);
  EXPECT_EQ(s.size(), 1);
}

TEST(SetInsertTest, InsertDuplicateElement) {
  s21::set<int> s{1, 2, 3};
  auto [it, inserted] = s.insert(2);
  EXPECT_FALSE(inserted);
  EXPECT_EQ(s.size(), 3);
}

TEST(SetInsertTest, InsertMultipleElements) {
  s21::set<int> s;
  s.insert(3);
  s.insert(1);
  s.insert(4);
  s.insert(1);  // duplicate

  EXPECT_EQ(s.size(), 3);
  auto it = s.begin();
  EXPECT_EQ(*it++, 1);
  EXPECT_EQ(*it++, 3);
  EXPECT_EQ(*it++, 4);
}

TEST(SetInsertTest, InsertStringElements) {
  s21::set<std::string> s;
  s.insert("apple");
  s.insert("banana");
  s.insert("apple");  // duplicate

  EXPECT_EQ(s.size(), 2);
  EXPECT_EQ(*s.begin(), "apple");
  EXPECT_EQ(*++s.begin(), "banana");
}

TEST(SetInsertTest, InsertToEmptySet) {
  s21::set<double> s;
  auto [it, inserted] = s.insert(3.14);

  EXPECT_TRUE(inserted);
  EXPECT_EQ(s.size(), 1);
  EXPECT_EQ(*s.begin(), 3.14);
}

TEST(SetInsertTest, InsertToLargeSet) {
  s21::set<int> s;
  for (int i = 0; i < 1000; ++i) {
    s.insert(i);
  }

  EXPECT_EQ(s.size(), 1000);
  EXPECT_EQ(*s.begin(), 0);
  EXPECT_EQ(*--s.end(), 999);
}

TEST(Set, Insert) {
  set<int> a;
  auto [it1, inserted1] = a.insert(7);
  EXPECT_TRUE(inserted1);
  EXPECT_EQ(a.size(), 1);
  EXPECT_EQ(*it1, 7);
  auto [it2, inserted2] = a.insert(7);
  EXPECT_FALSE(inserted2);
  EXPECT_EQ(a.size(), 1);
  EXPECT_EQ(*it2, 7);
  a.insert(3);
  a.insert(8);
  EXPECT_EQ(a.size(), 3);
}

TEST(Set, MergeWithEmpty) {
  s21::set<int> a{10, 20, 30};
  s21::set<int> b;

  a.merge(b);

  EXPECT_EQ(a.size(), 3);
  EXPECT_TRUE(b.empty());
  EXPECT_TRUE(a.contains(10));
  EXPECT_TRUE(a.contains(20));
  EXPECT_TRUE(a.contains(30));
}

TEST(Set, MergeIntoEmpty) {
  s21::set<int> a;
  s21::set<int> b{5, 15, 25};

  a.merge(b);

  EXPECT_EQ(a.size(), 3);
  EXPECT_TRUE(b.empty());
  EXPECT_TRUE(a.contains(5));
  EXPECT_TRUE(a.contains(15));
  EXPECT_TRUE(a.contains(25));
}
