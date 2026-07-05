#include <gtest/gtest.h>

#include "../includes/s21_list.h"
#include "../includes/s21_queue.h"

using size_type = s21::queue<int>::size_type;

// Проверяет push и size
TEST(QueueTest, PushAndSize) {
  s21::queue<int> q;
  q.push(10);
  q.push(20);
  q.push(30);
  EXPECT_EQ(q.size(), static_cast<size_type>(3));
}

// Проверяет front и back
TEST(QueueTest, FrontBack) {
  s21::queue<int> q = {1, 2, 3};
  EXPECT_EQ(q.front(), 1);
  EXPECT_EQ(q.back(), 3);
}

// Проверяет pop
TEST(QueueTest, Pop) {
  s21::queue<int> q = {5, 6, 7};
  q.pop();
  EXPECT_EQ(q.front(), 6);
  q.pop();
  EXPECT_EQ(q.front(), 7);
  q.pop();
  EXPECT_TRUE(q.empty());
}

// Проверяет swap
TEST(QueueTest, Swap) {
  s21::queue<int> q1 = {1, 2, 3};
  s21::queue<int> q2 = {4, 5};
  q1.swap(q2);
  EXPECT_EQ(q1.size(), static_cast<size_type>(2));
  EXPECT_EQ(q2.size(), static_cast<size_type>(3));
  EXPECT_EQ(q1.front(), 4);
  EXPECT_EQ(q2.front(), 1);
}

// Проверяет пустую очередь
TEST(QueueTest, EmptyCheck) {
  s21::queue<int> q;
  EXPECT_TRUE(q.empty());
  q.push(1);
  EXPECT_FALSE(q.empty());
}

TEST(QueueInsertManyTest, InsertManyBack) {
  s21::queue<int> q;
  q.push(1);
  q.push(2);

  q.insert_many_back(3, 4);

  std::vector<int> expected = {1, 2, 3, 4};

  for (int val : expected) {
    ASSERT_EQ(q.front(), val);
    q.pop();
  }
  ASSERT_TRUE(q.empty());
}