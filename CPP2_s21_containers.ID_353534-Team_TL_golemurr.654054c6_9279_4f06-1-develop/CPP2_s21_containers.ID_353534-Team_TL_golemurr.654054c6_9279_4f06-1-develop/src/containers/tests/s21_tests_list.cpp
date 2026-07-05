#include <gtest/gtest.h>

#include "../includes/s21_list.h"

using size_type = s21::list<int>::size_type;

// Проверяет push_back и size
TEST(ListTest, PushBackAndSize) {
  s21::list<int> l;
  l.push_back(1);
  l.push_back(2);
  l.push_back(3);
  EXPECT_EQ(l.size(), static_cast<size_type>(3));
}

// Проверяет front и back
TEST(ListTest, FrontAndBack) {
  s21::list<int> l = {1, 2, 3};
  EXPECT_EQ(l.front(), 1);
  EXPECT_EQ(l.back(), 3);
}

// Проверяет работу итераторов
TEST(ListTest, IteratorWorks) {
  s21::list<int> l = {10, 20, 30};
  auto it = l.begin();
  EXPECT_EQ(*it, 10);
  ++it;
  EXPECT_EQ(*it, 20);
  ++it;
  EXPECT_EQ(*it, 30);
}

// Проверяет insert и erase
TEST(ListTest, InsertErase) {
  s21::list<int> l = {1, 2, 4};
  auto it = l.begin();
  ++it;
  l.insert(it, 3);
  int result[] = {1, 3, 2, 4};
  int i = 0;
  for (auto val : l) {
    EXPECT_EQ(val, result[i++]);
  }
  l.erase(l.begin());
  EXPECT_EQ(l.front(), 3);
}

// Проверяет swap
TEST(ListTest, SwapWorks) {
  s21::list<int> l1 = {1, 2, 3};
  s21::list<int> l2 = {4, 5};
  l1.swap(l2);
  EXPECT_EQ(l1.size(), static_cast<size_type>(2));
  EXPECT_EQ(l2.size(), static_cast<size_type>(3));
  EXPECT_EQ(l1.front(), 4);
  EXPECT_EQ(l2.front(), 1);
}

// Проверяет merge
TEST(ListTest, MergeWorks) {
  s21::list<int> l1 = {1, 3, 5};
  s21::list<int> l2 = {2, 4};
  l1.merge(l2);
  int expected[] = {1, 2, 3, 4, 5};
  int i = 0;
  for (auto val : l1) {
    EXPECT_EQ(val, expected[i++]);
  }
  EXPECT_TRUE(l2.empty());
}

// Проверяет reverse, unique и sort
TEST(ListTest, ReverseUniqueSort) {
  s21::list<int> l = {3, 2, 1};
  l.reverse();
  int expected1[] = {1, 2, 3};
  int i = 0;
  for (auto val : l) {
    EXPECT_EQ(val, expected1[i++]);
  }

  l.push_back(3);
  l.unique();
  int expected2[] = {1, 2, 3};
  i = 0;
  for (auto val : l) {
    EXPECT_EQ(val, expected2[i++]);
  }

  l.sort();
  int expected3[] = {1, 2, 3};
  i = 0;
  for (auto val : l) {
    EXPECT_EQ(val, expected3[i++]);
  }
}

TEST(ListInsertManyTest, InsertManyMiddle) {
  s21::list<int> lst = {1, 2, 5};
  auto it = lst.begin();
  ++it;

  lst.insert_many(it, 10, 20);

  std::vector<int> expected = {1, 10, 20, 2, 5};
  ASSERT_EQ(lst.size(), static_cast<size_t>(expected.size()));
  auto list_it = lst.begin();
  for (int val : expected) {
    ASSERT_EQ(*list_it, val);
    ++list_it;
  }
}

TEST(ListInsertManyTest, InsertManyBack) {
  s21::list<int> lst = {1, 2};
  lst.insert_many_back(3, 4);

  std::vector<int> expected = {1, 2, 3, 4};
  ASSERT_EQ(lst.size(), static_cast<size_t>(expected.size()));
  auto list_it = lst.begin();
  for (int val : expected) {
    ASSERT_EQ(*list_it, val);
    ++list_it;
  }
}

TEST(ListInsertManyTest, InsertManyFront) {
  s21::list<int> lst = {3, 4};
  lst.insert_many_front(1, 2);

  std::vector<int> expected = {1, 2, 3, 4};
  ASSERT_EQ(lst.size(), static_cast<size_t>(expected.size()));
  auto list_it = lst.begin();
  for (int val : expected) {
    ASSERT_EQ(*list_it, val);
    ++list_it;
  }
}