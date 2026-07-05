#include <gtest/gtest.h>

#include "../includes/s21_vector.h"

using size_type = s21::vector<int>::size_type;

// Проверяет push_back и size
TEST(VectorTest, PushBackAndSize) {
  s21::vector<int> v;
  v.push_back(1);
  v.push_back(2);
  v.push_back(3);
  EXPECT_EQ(v.size(), static_cast<size_type>(3));
  EXPECT_EQ(v.back(), 3);
}

// Проверяет at и operator[]
TEST(VectorTest, AccessElements) {
  s21::vector<int> v = {10, 20, 30};
  EXPECT_EQ(v.at(0), 10);
  EXPECT_EQ(v[1], 20);
  EXPECT_EQ(v.front(), 10);
  EXPECT_EQ(v.back(), 30);
}

// Проверяет работу итераторов
TEST(VectorTest, Iterators) {
  s21::vector<int> v = {1, 2, 3};
  auto it = v.begin();
  EXPECT_EQ(*it, 1);
  ++it;
  EXPECT_EQ(*it, 2);
  ++it;
  EXPECT_EQ(*it, 3);
}

// Проверяет reserve, capacity, shrink_to_fit
TEST(VectorTest, CapacityReserveShrink) {
  s21::vector<int> v;
  v.reserve(10);
  EXPECT_GE(v.capacity(), static_cast<size_type>(10));
  v.push_back(1);
  v.shrink_to_fit();
  EXPECT_EQ(v.capacity(), v.size());
}

// Проверяет insert и erase
TEST(VectorTest, InsertErase) {
  s21::vector<int> v = {1, 2, 4};
  auto it = v.begin() + 2;
  v.insert(it, 3);
  int result[] = {1, 2, 3, 4};
  int i = 0;
  for (auto val : v) {
    EXPECT_EQ(val, result[i++]);
  }
  v.erase(v.begin());
  EXPECT_EQ(v.front(), 2);
}

// Проверяет swap
TEST(VectorTest, Swap) {
  s21::vector<int> v1 = {1, 2, 3};
  s21::vector<int> v2 = {4, 5};
  v1.swap(v2);
  EXPECT_EQ(v1.size(), static_cast<size_type>(2));
  EXPECT_EQ(v2.size(), static_cast<size_type>(3));
  EXPECT_EQ(v1.front(), 4);
  EXPECT_EQ(v2.front(), 1);
}

// Проверяет пустоту и clear
TEST(VectorTest, EmptyClear) {
  s21::vector<int> v;
  EXPECT_TRUE(v.empty());
  v.push_back(1);
  EXPECT_FALSE(v.empty());
  v.clear();
  EXPECT_TRUE(v.empty());
}

TEST(VectorInsertManyTest, InsertManyMiddle) {
  s21::vector<int> vec = {1, 2, 5};
  auto it = vec.begin() + 1;

  vec.insert_many(it, 10, 20);

  std::vector<int> expected = {1, 10, 20, 2, 5};
  ASSERT_EQ(vec.size(), expected.size());
  for (size_t i = 0; i < vec.size(); ++i) {
    ASSERT_EQ(vec[i], expected[i]);
  }
}

TEST(VectorInsertManyTest, InsertManyBack) {
  s21::vector<int> vec = {1, 2};
  vec.insert_many_back(3, 4);

  std::vector<int> expected = {1, 2, 3, 4};
  ASSERT_EQ(vec.size(), expected.size());
  for (size_t i = 0; i < vec.size(); ++i) {
    ASSERT_EQ(vec[i], expected[i]);
  }
}