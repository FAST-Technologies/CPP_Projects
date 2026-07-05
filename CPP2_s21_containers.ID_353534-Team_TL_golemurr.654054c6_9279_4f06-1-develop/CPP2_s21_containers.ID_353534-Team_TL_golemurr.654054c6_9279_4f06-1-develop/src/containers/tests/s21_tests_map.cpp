#include <gtest/gtest.h>

#include <map>
#include <string>

#include "../includes/s21_map.h"

using s21::map;

TEST(Map, DefaultConstructor) {
  map<int, int> a;
  std::map<int, int> b;
  EXPECT_EQ(a.size(), b.size());
  EXPECT_TRUE(a.empty());
}

TEST(Map, InitializerConstructor) {
  map<int, int> a{{1, 10}, {2, 20}, {3, 30}, {4, 40}, {5, 50}};
  std::map<int, int> b{{1, 10}, {2, 20}, {3, 30}, {4, 40}, {5, 50}};
  EXPECT_EQ(a.size(), b.size());
  auto it1 = a.begin();
  auto it2 = b.begin();
  for (; it1 != a.end() && it2 != b.end(); ++it1, ++it2) {
    EXPECT_EQ(it1->first, it2->first);
    EXPECT_EQ(it1->second, it2->second);
  }
}

TEST(Map, CopyConstructor) {
  map<int, int> a{{1, 10}, {3, 30}, {5, 50}};
  map<int, int> b = a;
  EXPECT_EQ(a.size(), b.size());
  auto it1 = a.begin();
  auto it2 = b.begin();
  for (; it1 != a.end(); ++it1, ++it2) {
    EXPECT_EQ(it1->first, it2->first);
    EXPECT_EQ(it1->second, it2->second);
  }
}

TEST(Map, MoveConstructor) {
  map<int, int> a{{1, 10}, {2, 20}, {3, 30}};
  map<int, int> b = std::move(a);
  std::map<int, int> ref{{1, 10}, {2, 20}, {3, 30}};
  EXPECT_EQ(b.size(), ref.size());
  EXPECT_TRUE(a.empty());
  auto it1 = b.begin();
  auto it2 = ref.begin();
  for (; it1 != b.end() && it2 != ref.end(); ++it1, ++it2) {
    EXPECT_EQ(it1->first, it2->first);
    EXPECT_EQ(it1->second, it2->second);
  }
}

TEST(Map, CopyAssignment) {
  map<int, int> a{{1, 10}, {3, 30}, {5, 50}};
  map<int, int> b;
  b = a;
  EXPECT_EQ(a.size(), b.size());
  auto it1 = a.begin();
  auto it2 = b.begin();
  for (; it1 != a.end(); ++it1, ++it2) {
    EXPECT_EQ(it1->first, it2->first);
    EXPECT_EQ(it1->second, it2->second);
  }
}

TEST(Map, MoveAssignment) {
  map<int, std::string> a{{1, "as"}, {2, "bu"}, {3, "ki"}};
  map<int, std::string> b{{4, "wu"}, {5, "er"}};
  b = std::move(a);
  EXPECT_EQ(b.size(), 3);
  EXPECT_TRUE(a.empty());
  auto it = b.begin();
  std::map<int, std::string> ref{{1, "as"}, {2, "bu"}, {3, "ki"}};
  auto it_ref = ref.begin();
  for (; it != b.end() && it_ref != ref.end(); ++it, ++it_ref) {
    EXPECT_EQ(it->first, it_ref->first);
    EXPECT_EQ(it->second, it_ref->second);
  }
}

TEST(Map, Insertt) {
  map<int, float> a;
  auto [it1, inserted1] = a.insert({7, 70.1});
  auto [it2, inserted2] = a.insert({8, 80.1});
  EXPECT_EQ(a.size(), 2);
  EXPECT_EQ(it1->first, 7);
  EXPECT_EQ(it2->first, 8);
  EXPECT_NEAR(it1->second, 70.1f, 1e-5);
  EXPECT_NEAR(it2->second, 80.1f, 1e-5);
  EXPECT_TRUE(it2 == a.find(8));
  a.insert({3, 30.6});
  a.insert({8, 80.2});
  EXPECT_EQ(a.size(), 3);
}

TEST(Map, Insert) {
  map<int, int> a;
  auto [it1, inserted1] = a.insert({7, 70});
  auto [it2, inserted2] = a.insert({8, 80});
  EXPECT_EQ(a.size(), 2);
  EXPECT_EQ(it1->first, 7);
  EXPECT_EQ(it2->first, 8);
  EXPECT_EQ(it1->second, 70);
  EXPECT_EQ(it2->second, 80);
  EXPECT_TRUE(it2 == a.find(8));
  a.insert({3, 30});
  a.insert({8, 80});
  EXPECT_EQ(a.size(), 3);
}

TEST(Map, InsertWithKeyValue) {
  map<int, std::string> a;
  auto [it, inserted] = a.insert(1, "one");
  EXPECT_TRUE(inserted);
  EXPECT_EQ(a.size(), 1);
  EXPECT_EQ(it->first, 1);
  EXPECT_EQ(it->second, "one");
  auto [it2, inserted2] = a.insert(1, "two");
  EXPECT_FALSE(inserted2);
  EXPECT_EQ(a.size(), 1);
  EXPECT_EQ(it2->second, "one");
}

TEST(Map, InsertOrAssign) {
  map<int, int> a;
  auto [it1, inserted1] = a.insert_or_assign(5, 50);
  EXPECT_TRUE(inserted1);
  EXPECT_EQ(a.size(), 1);
  EXPECT_EQ(it1->second, 50);
  auto [it2, inserted2] = a.insert_or_assign(5, 55);
  EXPECT_FALSE(inserted2);
  EXPECT_EQ(a.size(), 1);
  EXPECT_EQ(it2->second, 55);
}

TEST(Map, Erase) {
  map<int, int> a{{1, 10}, {3, 30}, {7, 70}, {25, 250}};
  auto it = a.begin();
  ++it;
  EXPECT_EQ(it->first, 3);
  EXPECT_EQ(it->second, 30);
  a.erase(it);
  EXPECT_EQ(a.size(), 3);
  EXPECT_FALSE(a.contains(3));
}

TEST(Map, EraseRange) {
  map<int, int> a{{1, 10}, {3, 30}, {7, 70}, {25, 250}};
  auto it1 = a.begin();
  auto it2 = a.find(7);
  a.erase(it1, it2);
  EXPECT_EQ(a.size(), 2);
  EXPECT_FALSE(a.contains(1));
  EXPECT_FALSE(a.contains(3));
}

TEST(Map, Swap) {
  map<int, int> a{{9, 90}, {18, 180}};
  map<int, int> b{{4, 40}, {7, 70}};
  a.swap(b);
  EXPECT_EQ(a.size(), 2);
  EXPECT_EQ(b.size(), 2);
  EXPECT_EQ(a.begin()->first, 4);
  EXPECT_EQ(b.begin()->first, 9);
}

TEST(Map, Merge) {
  map<int, int> a{{1, 10}, {2, 20}, {3, 30}};
  map<int, int> b{{2, 25}, {3, 35}};
  a.merge(b);
  EXPECT_EQ(a.size(), 3);
  EXPECT_FALSE(b.empty());
  EXPECT_EQ(a.at(1), 10);
  EXPECT_EQ(a.at(2), 20);
  EXPECT_EQ(a.at(3), 30);
}

TEST(Map, Find) {
  map<int, int> a{{2, 20}, {4, 40}, {6, 60}, {8, 80}};
  auto it = a.find(2);
  EXPECT_NE(it, a.end());
  EXPECT_EQ(it->first, 2);
  EXPECT_EQ(it->second, 20);
  auto it_not_found = a.find(10);
  EXPECT_EQ(it_not_found, a.end());
}

TEST(Map, Contains) {
  map<int, int> a{{9, 27}, {18, 54}, {27, 91}};
  EXPECT_TRUE(a.contains(27));
  EXPECT_FALSE(a.contains(88));
}

TEST(Map, At) {
  map<int, int> a{{1, 10}, {2, 20}, {3, 30}};
  EXPECT_EQ(a.at(2), 20);
  EXPECT_THROW(a.at(4), std::out_of_range);
}

TEST(Map, OperatorBracket) {
  map<int, int> a;
  a[1] = 10;
  a[2] = 20;
  EXPECT_EQ(a.size(), 2);
  EXPECT_EQ(a[1], 10);
  EXPECT_EQ(a[2], 20);
}

TEST(Map, EraseAll) {
  map<int, int> a{{1, 10}, {2, 20}, {3, 30}};
  a.erase(a.find(1));
  a.erase(a.find(2));
  a.erase(a.find(3));
  EXPECT_EQ(a.size(), 0);
  EXPECT_FALSE(a.contains(1));
  EXPECT_FALSE(a.contains(2));
  EXPECT_FALSE(a.contains(3));
}

TEST(Map, EraseNodeWithOneChild) {
  map<int, int> a{{10, 100}, {5, 50}};
  auto it = a.find(10);
  a.erase(it);
  EXPECT_EQ(a.size(), 1);
  EXPECT_FALSE(a.contains(10));
  EXPECT_TRUE(a.contains(5));
}

TEST(Map, EraseLeaf) {
  map<int, int> a{{10, 100}, {5, 50}, {15, 150}};
  auto it = a.find(15);  // лист
  EXPECT_EQ(it->first, 15);
  a.erase(it);
  EXPECT_EQ(a.size(), 2);
  EXPECT_FALSE(a.contains(15));
}

TEST(Map, MaxSize) {
  map<int, int> a;
  EXPECT_GT(a.max_size(), a.size());
}

TEST(Map, Clear) {
  map<int, int> a{{234, 2340}, {567, 5670}, {876, 8760}};
  a.clear();
  EXPECT_TRUE(a.empty());
  EXPECT_EQ(a.size(), 0);
}

TEST(Map, BeginEnd) {
  map<int, int> a{{2, 20}, {4, 40}, {5, 50}, {9, 90}};
  auto it = a.begin();
  EXPECT_EQ(it->first, 2);
  EXPECT_EQ(it->second, 20);
  auto end = a.end();
  --end;
  EXPECT_EQ(end->first, 9);
  EXPECT_EQ(end->second, 90);
}

TEST(Map, Empty) {
  map<int, int> a;
  EXPECT_TRUE(a.empty());
  a.insert({1, 10});
  EXPECT_FALSE(a.empty());
}

TEST(Map, Size) {
  map<float, float> b{{23.6, 236.0}, {54.6, 546.0}, {23.8, 238.0}};
  EXPECT_EQ(b.size(), 3);
  b.erase(b.begin());
  EXPECT_EQ(b.size(), 2);
}

TEST(Map, StringInitializerConstructor) {
  map<std::string, int> a{{"abc", 1}, {"def", 2}, {"def", 3}, {"ghi", 4}};
  std::map<std::string, int> b{{"abc", 1}, {"def", 2}, {"ghi", 4}};
  EXPECT_EQ(a.size(), b.size());
  auto it1 = a.begin();
  auto it2 = b.begin();
  for (; it1 != a.end() && it2 != b.end(); ++it1, ++it2) {
    EXPECT_EQ(it1->first, it2->first);
    EXPECT_EQ(it1->second, it2->second);
  }
}

TEST(Map, InsertMany) {
  map<int, std::string> m;
  auto result =
      m.insert_many(std::make_pair(1, "one"), std::make_pair(2, "two"),
                    std::make_pair(3, "three"), std::make_pair(2, "duplicate"));

  ASSERT_EQ(m.size(), 3);
  ASSERT_EQ(result.size(), 4);

  EXPECT_TRUE(result[0].second);
  EXPECT_TRUE(result[1].second);
  EXPECT_TRUE(result[2].second);
  EXPECT_FALSE(result[3].second);

  EXPECT_EQ(m.at(2), "two");
}
