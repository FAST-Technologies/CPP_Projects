#include <gtest/gtest.h>

#include "../includes/s21_stack.h"

using size_type = s21::stack<int>::size_type;

// Проверяет push и size
TEST(StackTest, PushAndSize) {
  s21::stack<int> s;
  s.push(10);
  s.push(20);
  s.push(30);
  EXPECT_EQ(s.size(), static_cast<size_type>(3));
  EXPECT_EQ(s.top(), 30);
}

// Проверяет pop и top
TEST(StackTest, PopAndTop) {
  s21::stack<int> s = {1, 2, 3};
  s.pop();
  EXPECT_EQ(s.top(), 2);
  s.pop();
  EXPECT_EQ(s.top(), 1);
  s.pop();
  EXPECT_TRUE(s.empty());
}

// Проверяет swap
TEST(StackTest, Swap) {
  s21::stack<int> s1 = {1, 2, 3};
  s21::stack<int> s2 = {4, 5};
  s1.swap(s2);
  EXPECT_EQ(s1.size(), static_cast<size_type>(2));
  EXPECT_EQ(s2.size(), static_cast<size_type>(3));
  EXPECT_EQ(s1.top(), 5);
  EXPECT_EQ(s2.top(), 3);
}

// Проверяет пустую очередь
TEST(StackTest, EmptyCheck) {
  s21::stack<int> s;
  EXPECT_TRUE(s.empty());
  s.push(42);
  EXPECT_FALSE(s.empty());
}

TEST(StackInsertManyTest, InsertManyBack) {
  s21::stack<int> st;
  st.push(1);
  st.push(2);

  st.insert_many_back(3, 4);

  std::vector<int> expected = {4, 3, 2, 1};

  for (int val : expected) {
    ASSERT_EQ(st.top(), val);
    st.pop();
  }
  ASSERT_TRUE(st.empty());
}