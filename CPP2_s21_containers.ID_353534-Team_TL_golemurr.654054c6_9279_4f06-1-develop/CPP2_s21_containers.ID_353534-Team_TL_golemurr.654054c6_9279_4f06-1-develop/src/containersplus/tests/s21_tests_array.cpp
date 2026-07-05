#include <gtest/gtest.h>

#include "../../s21_containersplus.h"
using s21::array;

TEST(ArrayTest, DefaultConstructor) {
  array<int, 5> a;
  std::array<int, 5> b{};
  EXPECT_EQ(a.size(), b.size());
  for (size_t i = 0; i < a.size(); ++i) {
    EXPECT_EQ(a[i], b[i]);
  }
}

TEST(ArrayTest, InitializerListConstructor) {
  array<int, 3> a{1, 2, 3};
  std::array<int, 3> b{1, 2, 3};
  EXPECT_EQ(a.size(), b.size());
  for (size_t i = 0; i < 3; ++i) {
    EXPECT_EQ(a[i], b[i]);
  }
}

TEST(ArrayTest, InitializerListConstructorTooLong) {
  EXPECT_THROW((array<int, 2>{1, 2, 3}), std::length_error);
}

TEST(ArrayTest, CopyConstructor) {
  array<char, 4> a{'a', 'b', 'c', 'd'};
  array<char, 4> b(a);
  std::array<char, 4> ref{'a', 'b', 'c', 'd'};
  EXPECT_EQ(b.size(), ref.size());
  for (size_t i = 0; i < 4; ++i) {
    EXPECT_EQ(b[i], ref[i]);
  }
}

TEST(ArrayTest, MoveConstructor) {
  array<std::string, 2> a{"hello", "world"};
  array<std::string, 2> b(std::move(a));
  std::array<std::string, 2> ref{"hello", "world"};
  EXPECT_EQ(b[0], ref[0]);
  EXPECT_EQ(b[1], ref[1]);
}

TEST(ArrayTest, MoveAssignment) {
  array<double, 3> a{1.1, 2.2, 3.3};
  array<double, 3> b;
  b = std::move(a);
  std::array<double, 3> ref{1.1, 2.2, 3.3};
  for (size_t i = 0; i < 3; ++i) {
    EXPECT_EQ(b[i], ref[i]);
  }
}

TEST(ArrayTest, AtValid) {
  array<int, 2> a{7, 8};
  std::array<int, 2> b{7, 8};
  EXPECT_EQ(a.at(0), b.at(0));
  EXPECT_EQ(a.at(1), b.at(1));
}

TEST(ArrayTest, AtInvalid) {
  array<int, 2> a{10, 20};
  EXPECT_THROW(a.at(2), std::out_of_range);
}

TEST(ArrayTest, OperatorSquareBrackets) {
  array<int, 2> a;
  std::array<int, 2> b;
  a[0] = b[0] = 100;
  a[1] = b[1] = 200;
  EXPECT_EQ(a[0], b[0]);
  EXPECT_EQ(a[1], b[1]);
}

TEST(ArrayTest, FrontBack) {
  array<int, 3> a{5, 6, 7};
  std::array<int, 3> b{5, 6, 7};
  EXPECT_EQ(a.front(), b.front());
  EXPECT_EQ(a.back(), b.back());
}

TEST(ArrayTest, DataPointer) {
  array<int, 2> a{1, 2};
  int* data = a.data();
  EXPECT_EQ(data[0], 1);
  EXPECT_EQ(data[1], 2);
}

TEST(ArrayTest, Iterators) {
  array<int, 3> a{1, 2, 3};
  std::array<int, 3> b{1, 2, 3};
  auto it1 = a.begin();
  auto it2 = b.begin();
  for (; it1 != a.end(); ++it1, ++it2) {
    EXPECT_EQ(*it1, *it2);
  }
}

TEST(ArrayTest, EmptyFalse) {
  array<int, 1> a;
  std::array<int, 1> b;
  EXPECT_FALSE(a.empty());
  EXPECT_FALSE(b.empty());
}

TEST(ArrayTest, SizeAndMaxSize) {
  array<int, 10> a;
  std::array<int, 10> b;
  EXPECT_EQ(a.size(), b.size());
  EXPECT_EQ(a.max_size(), b.max_size());
}

TEST(ArrayTest, SwapEqualSize) {
  array<int, 3> a{1, 2, 3};
  array<int, 3> b{4, 5, 6};
  std::array<int, 3> a_ref{1, 2, 3};
  std::array<int, 3> b_ref{4, 5, 6};
  a.swap(b);
  a_ref.swap(b_ref);
  for (int i = 0; i < 3; ++i) {
    EXPECT_EQ(a[i], a_ref[i]);
    EXPECT_EQ(b[i], b_ref[i]);
  }
}

TEST(ArrayTest, FillAll) {
  array<char, 4> a;
  std::array<char, 4> b;
  a.fill('z');
  b.fill('z');
  for (int i = 0; i < 4; ++i) {
    EXPECT_EQ(a[i], b[i]);
  }
}