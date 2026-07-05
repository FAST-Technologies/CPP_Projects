#include <gtest/gtest.h>

#include <set>

#include "../../s21_containersplus.h"

using s21::multiset;

TEST(Multiset, DefaultConstructor) {
  multiset<int> a;
  std::multiset<int> b;
  EXPECT_EQ(a.size(), b.size());
}

TEST(Multiset, InitializerConstructor) {
  multiset<int> a{1, 2, 3, 3, 4};
  std::multiset<int> b{1, 2, 3, 3, 4};
  EXPECT_EQ(a.size(), b.size());
  auto it1 = a.begin();
  auto it2 = b.begin();
  for (; it1 != a.end() && it2 != b.end(); ++it1, ++it2) {
    EXPECT_EQ(*it1, *it2);
  }
}

TEST(Multiset, CopyConstructor) {
  multiset<int> a{1, 2, 2, 3};
  multiset<int> b = a;
  EXPECT_EQ(a.size(), b.size());
  auto it1 = a.begin();
  auto it2 = b.begin();
  for (; it1 != a.end(); ++it1, ++it2) {
    EXPECT_EQ(*it1, *it2);
  }
}

TEST(Multiset, MoveConstructor) {
  multiset<int> a{1, 2, 3};
  multiset<int> b = std::move(a);
  std::multiset<int> ref{1, 2, 3};
  EXPECT_EQ(b.size(), ref.size());
}

TEST(Multiset, InsertAndCount) {
  multiset<int> a;
  a.insert(5);
  a.insert(5);
  a.insert(3);
  EXPECT_EQ(a.count(5), 2);
  EXPECT_EQ(a.count(3), 1);
  EXPECT_EQ(a.count(0), 0);
}

TEST(Multiset, EraseSingle) {
  multiset<int> a{1, 2, 2, 3};
  auto it = a.begin();
  ++it;
  a.erase(it);
  EXPECT_EQ(a.count(2), 1);
}

TEST(Multiset, Swap) {
  multiset<int> a{1, 2};
  multiset<int> b{3, 4};
  a.swap(b);
  EXPECT_EQ(*a.begin(), 3);
  EXPECT_EQ(*b.begin(), 1);
}

TEST(Multiset, Merge) {
  multiset<int> a{1, 2};
  multiset<int> b{2, 3};
  a.merge(b);
  EXPECT_EQ(a.size(), 4);
  EXPECT_TRUE(b.empty());
}

TEST(Multiset, Contains) {
  multiset<int> a{1, 2, 3};
  EXPECT_TRUE(a.contains(2));
  EXPECT_FALSE(a.contains(4));
}

TEST(Multiset, EqualRange) {
  multiset<int> a{1, 2, 2, 3, 4};
  auto [first, second] = a.equal_range(2);
  EXPECT_EQ(*first, 2);
  ++first;
  EXPECT_EQ(*first, 2);
  ++first;
  EXPECT_NE(*first, 2);
}

TEST(Multiset, LowerBound) {
  multiset<int> a{1, 3, 5};
  auto it = a.lower_bound(2);
  EXPECT_EQ(*it, 3);
}

TEST(Multiset, UpperBound) {
  multiset<int> a{1, 3, 5};
  auto it = a.upper_bound(3);
  EXPECT_EQ(*it, 5);
}

TEST(Multiset, InsertMany) {
  multiset<int> a;
  auto res = a.insert_many(1, 2, 2, 3);
  EXPECT_EQ(a.count(2), 2);
  EXPECT_EQ(res.size(), 4);
  for (const auto& [it, ok] : res) {
    EXPECT_TRUE(ok);
    EXPECT_TRUE(a.contains(*it));
  }
}

TEST(Multiset, Clear) {
  multiset<int> a{1, 2, 3};
  a.clear();
  EXPECT_TRUE(a.empty());
  EXPECT_EQ(a.size(), 0);
}