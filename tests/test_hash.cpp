#include "hash.h"

#include <gtest/gtest.h>

TEST(HashUtil, CombineIntDeterministic) {
  size_t a = hashUtil::hash_combine_int(42, 0);
  size_t b = hashUtil::hash_combine_int(42, 0);
  EXPECT_EQ(a, b);
}

TEST(HashUtil, CombineIntDifferentValues) {
  size_t a = hashUtil::hash_combine_int(1, 0);
  size_t b = hashUtil::hash_combine_int(2, 0);
  EXPECT_NE(a, b);
}

TEST(HashUtil, CombineIntDifferentSeeds) {
  size_t a = hashUtil::hash_combine_int(42, 0);
  size_t b = hashUtil::hash_combine_int(42, 1);
  EXPECT_NE(a, b);
}

TEST(HashUtil, CombineStringDeterministic) {
  size_t a = hashUtil::hash_combine_string("hello", 0);
  size_t b = hashUtil::hash_combine_string("hello", 0);
  EXPECT_EQ(a, b);
}

TEST(HashUtil, CombineStringDifferentValues) {
  size_t a = hashUtil::hash_combine_string("hello", 0);
  size_t b = hashUtil::hash_combine_string("world", 0);
  EXPECT_NE(a, b);
}

TEST(HashUtil, CombineStringDifferentSeeds) {
  size_t a = hashUtil::hash_combine_string("hello", 0);
  size_t b = hashUtil::hash_combine_string("hello", 1);
  EXPECT_NE(a, b);
}

TEST(HashUtil, CombineStringEmpty) {
  size_t a = hashUtil::hash_combine_string("", 0);
  size_t b = hashUtil::hash_combine_string("x", 0);
  EXPECT_NE(a, b);
}

TEST(HashUtil, CombineChaining) {
  size_t h1 = hashUtil::hash_combine_int(1, 0);
  h1 = hashUtil::hash_combine_int(2, h1);

  size_t h2 = hashUtil::hash_combine_int(1, 0);
  h2 = hashUtil::hash_combine_int(2, h2);

  EXPECT_EQ(h1, h2);
}

TEST(HashUtil, CombineOrderMatters) {
  size_t h1 = hashUtil::hash_combine_int(1, 0);
  h1 = hashUtil::hash_combine_int(2, h1);

  size_t h2 = hashUtil::hash_combine_int(2, 0);
  h2 = hashUtil::hash_combine_int(1, h2);

  EXPECT_NE(h1, h2);
}
