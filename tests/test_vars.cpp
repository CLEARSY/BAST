#include "vars.h"

#include <gtest/gtest.h>

// --- VarName construction & kinds ---

TEST(VarName, NoSuffix) {
  auto v = VarName::makeVarWithoutSuffix("x");
  EXPECT_EQ(v.kind(), VarName::Kind::NoSuffix);
  EXPECT_EQ(v.prefix(), "x");
  EXPECT_EQ(v.suffix(), -1);
}

TEST(VarName, WithSuffix) {
  auto v = VarName::makeVar("x", 3);
  EXPECT_EQ(v.kind(), VarName::Kind::WithSuffix);
  EXPECT_EQ(v.prefix(), "x");
  EXPECT_EQ(v.suffix(), 3);
}

TEST(VarName, FreshId) {
  auto v = VarName::makeFreshId("x");
  EXPECT_EQ(v.kind(), VarName::Kind::FreshId);
  EXPECT_EQ(v.prefix(), "x");
  EXPECT_EQ(v.suffix(), -2);
}

TEST(VarName, Tmp) {
  auto v = VarName::makeTmp("x");
  EXPECT_EQ(v.kind(), VarName::Kind::Tmp);
  EXPECT_EQ(v.prefix(), "x");
  EXPECT_LT(v.suffix(), -2);
}

TEST(VarName, TmpUnique) {
  auto v1 = VarName::makeTmp("x");
  auto v2 = VarName::makeTmp("x");
  EXPECT_NE(v1, v2);
}

// --- VarName::show ---

TEST(VarName, ShowNoSuffix) {
  auto v = VarName::makeVarWithoutSuffix("toto");
  EXPECT_EQ(v.show(), "toto");
}

TEST(VarName, ShowWithSuffix) {
  auto v = VarName::makeVar("toto", 1);
  EXPECT_EQ(v.show(), "toto$1");
}

TEST(VarName, ShowFreshId) {
  auto v = VarName::makeFreshId("toto");
  EXPECT_EQ(v.show(), "_freshId_toto");
}

TEST(VarName, ShowTmp) {
  auto v = VarName::makeTmp("toto");
  auto s = v.show();
  EXPECT_TRUE(s.find("_tmpId_toto$") == 0);
}

// --- VarName comparison ---

TEST(VarName, EqualSame) {
  auto v1 = VarName::makeVarWithoutSuffix("x");
  auto v2 = VarName::makeVarWithoutSuffix("x");
  EXPECT_EQ(v1, v2);
}

TEST(VarName, NotEqualDifferentPrefix) {
  auto v1 = VarName::makeVarWithoutSuffix("x");
  auto v2 = VarName::makeVarWithoutSuffix("y");
  EXPECT_NE(v1, v2);
}

TEST(VarName, NotEqualDifferentSuffix) {
  auto v1 = VarName::makeVarWithoutSuffix("x");
  auto v2 = VarName::makeVar("x", 1);
  EXPECT_NE(v1, v2);
}

TEST(VarName, CompareConsistent) {
  auto v1 = VarName::makeVarWithoutSuffix("x");
  auto v2 = VarName::makeVar("x", 1);
  int c1 = VarName::compare(v1, v2);
  int c2 = VarName::compare(v2, v1);
  EXPECT_NE(c1, 0);
  // opposite signs
  EXPECT_TRUE((c1 < 0 && c2 > 0) || (c1 > 0 && c2 < 0));
}

TEST(VarName, OrderingOperators) {
  auto v1 = VarName::makeVarWithoutSuffix("a");
  auto v2 = VarName::makeVarWithoutSuffix("a");
  EXPECT_TRUE(v1 <= v2);
  EXPECT_TRUE(v1 >= v2);
  EXPECT_FALSE(v1 < v2);
  EXPECT_FALSE(v1 > v2);
}

// --- VarName vec_compare ---

TEST(VarName, VecCompareEqual) {
  std::vector<VarName> v1 = {VarName::makeVarWithoutSuffix("x"),
                             VarName::makeVarWithoutSuffix("y")};
  std::vector<VarName> v2 = {VarName::makeVarWithoutSuffix("x"),
                             VarName::makeVarWithoutSuffix("y")};
  EXPECT_EQ(VarName::vec_compare(v1, v2), 0);
}

TEST(VarName, VecCompareDifferentSize) {
  std::vector<VarName> v1 = {VarName::makeVarWithoutSuffix("x")};
  std::vector<VarName> v2 = {VarName::makeVarWithoutSuffix("x"),
                             VarName::makeVarWithoutSuffix("y")};
  EXPECT_LT(VarName::vec_compare(v1, v2), 0);
}

TEST(VarName, VecCompareEmpty) {
  std::vector<VarName> v1, v2;
  EXPECT_EQ(VarName::vec_compare(v1, v2), 0);
}

// --- VarName hashing ---

TEST(VarName, HashConsistency) {
  auto v1 = VarName::makeVarWithoutSuffix("x");
  auto v2 = VarName::makeVarWithoutSuffix("x");
  EXPECT_EQ(v1.hash_combine(0), v2.hash_combine(0));
}

TEST(VarName, HashDifferent) {
  auto v1 = VarName::makeVarWithoutSuffix("x");
  auto v2 = VarName::makeVarWithoutSuffix("y");
  EXPECT_NE(v1.hash_combine(0), v2.hash_combine(0));
}

// --- VarName::getFreshVar ---

TEST(VarName, GetFreshVarNotInSet) {
  std::set<VarName> s;
  auto v = VarName::getFreshVar("x", s);
  EXPECT_EQ(v, VarName::makeVarWithoutSuffix("x"));
}

TEST(VarName, GetFreshVarConflict) {
  std::set<VarName> s;
  s.insert(VarName::makeVarWithoutSuffix("x"));
  auto v = VarName::getFreshVar("x", s);
  EXPECT_NE(v, VarName::makeVarWithoutSuffix("x"));
  EXPECT_EQ(v.prefix(), "x");
  EXPECT_EQ(s.find(v), s.end());
}

TEST(VarName, GetFreshVarMultipleConflicts) {
  std::set<VarName> s;
  s.insert(VarName::makeVarWithoutSuffix("x"));
  s.insert(VarName::makeVar("x", 1));
  s.insert(VarName::makeVar("x", 2));
  auto v = VarName::getFreshVar("x", s);
  EXPECT_EQ(v.prefix(), "x");
  EXPECT_EQ(v.suffix(), 3);
}

TEST(VarName, GetFreshVarFromVarName) {
  std::set<VarName> s;
  auto orig = VarName::makeVarWithoutSuffix("x");
  auto v = VarName::getFreshVar(orig, s);
  EXPECT_EQ(v, orig);
}

TEST(VarName, GetFreshVarFromVarNameConflict) {
  std::set<VarName> s;
  s.insert(VarName::makeVarWithoutSuffix("x"));
  auto orig = VarName::makeVarWithoutSuffix("x");
  auto v = VarName::getFreshVar(orig, s);
  EXPECT_NE(v, orig);
  EXPECT_EQ(v.prefix(), "x");
  EXPECT_EQ(s.find(v), s.end());
}

TEST(VarName, GetFreshVarFromSuffixed) {
  std::set<VarName> s;
  s.insert(VarName::makeVar("x", 1));
  auto orig = VarName::makeVar("x", 1);
  auto v = VarName::getFreshVar(orig, s);
  EXPECT_NE(v, orig);
  EXPECT_EQ(v.prefix(), "x");
  EXPECT_GT(v.suffix(), 1);
}

TEST(VarName, GetFreshVarFromTmp) {
  std::set<VarName> s;
  auto orig = VarName::makeTmp("x");
  s.insert(orig);
  auto v = VarName::getFreshVar(orig, s);
  EXPECT_NE(v, orig);
  EXPECT_EQ(v.kind(), VarName::Kind::Tmp);
}

// --- TypedVar ---

TEST(TypedVar, Construction) {
  auto name = VarName::makeVarWithoutSuffix("x");
  TypedVar tv(name, BType::INT);
  EXPECT_EQ(tv.name, name);
  EXPECT_EQ(tv.type, BType::INT);
}

TEST(TypedVar, EqualSame) {
  TypedVar tv1(VarName::makeVarWithoutSuffix("x"), BType::INT);
  TypedVar tv2(VarName::makeVarWithoutSuffix("x"), BType::INT);
  EXPECT_EQ(tv1, tv2);
}

TEST(TypedVar, NotEqualDifferentName) {
  TypedVar tv1(VarName::makeVarWithoutSuffix("x"), BType::INT);
  TypedVar tv2(VarName::makeVarWithoutSuffix("y"), BType::INT);
  EXPECT_NE(tv1, tv2);
}

TEST(TypedVar, NotEqualDifferentType) {
  TypedVar tv1(VarName::makeVarWithoutSuffix("x"), BType::INT);
  TypedVar tv2(VarName::makeVarWithoutSuffix("x"), BType::BOOL);
  EXPECT_NE(tv1, tv2);
}

TEST(TypedVar, VecCompare) {
  std::vector<TypedVar> v1 = {
      TypedVar(VarName::makeVarWithoutSuffix("x"), BType::INT)};
  std::vector<TypedVar> v2 = {
      TypedVar(VarName::makeVarWithoutSuffix("x"), BType::INT)};
  EXPECT_EQ(TypedVar::vec_compare(v1, v2), 0);
}

TEST(TypedVar, VecCompareDifferentSize) {
  std::vector<TypedVar> v1;
  std::vector<TypedVar> v2 = {
      TypedVar(VarName::makeVarWithoutSuffix("x"), BType::INT)};
  EXPECT_LT(TypedVar::vec_compare(v1, v2), 0);
}

TEST(TypedVar, HashConsistency) {
  TypedVar tv1(VarName::makeVarWithoutSuffix("x"), BType::INT);
  TypedVar tv2(VarName::makeVarWithoutSuffix("x"), BType::INT);
  EXPECT_EQ(tv1.hash_combine(0), tv2.hash_combine(0));
}

// --- Context ---

TEST(Context, PushMatchingVars) {
  Context ctx;
  std::vector<TypedVar> v1 = {
      TypedVar(VarName::makeVarWithoutSuffix("x"), BType::INT)};
  std::vector<TypedVar> v2 = {
      TypedVar(VarName::makeVar("x", 1), BType::INT)};
  EXPECT_TRUE(ctx.push(v1, v2));
}

TEST(Context, PushDifferentSizes) {
  Context ctx;
  std::vector<TypedVar> v1 = {
      TypedVar(VarName::makeVarWithoutSuffix("x"), BType::INT)};
  std::vector<TypedVar> v2;
  EXPECT_FALSE(ctx.push(v1, v2));
}

TEST(Context, PushDifferentPrefixes) {
  Context ctx;
  std::vector<TypedVar> v1 = {
      TypedVar(VarName::makeVarWithoutSuffix("x"), BType::INT)};
  std::vector<TypedVar> v2 = {
      TypedVar(VarName::makeVarWithoutSuffix("y"), BType::INT)};
  EXPECT_FALSE(ctx.push(v1, v2));
}

TEST(Context, PushDifferentTypes) {
  Context ctx;
  std::vector<TypedVar> v1 = {
      TypedVar(VarName::makeVarWithoutSuffix("x"), BType::INT)};
  std::vector<TypedVar> v2 = {
      TypedVar(VarName::makeVarWithoutSuffix("x"), BType::BOOL)};
  EXPECT_FALSE(ctx.push(v1, v2));
}

TEST(Context, EqualsBoundVars) {
  Context ctx;
  auto x1 = VarName::makeVarWithoutSuffix("x");
  auto x2 = VarName::makeVar("x", 1);
  std::vector<TypedVar> v1 = {TypedVar(x1, BType::INT)};
  std::vector<TypedVar> v2 = {TypedVar(x2, BType::INT)};
  ctx.push(v1, v2);
  EXPECT_TRUE(ctx.equals(x1, x2));
}

TEST(Context, EqualsFreeVars) {
  Context ctx;
  auto y1 = VarName::makeVarWithoutSuffix("y");
  auto y2 = VarName::makeVarWithoutSuffix("y");
  EXPECT_TRUE(ctx.equals(y1, y2));
}

TEST(Context, NotEqualsMixed) {
  Context ctx;
  auto x1 = VarName::makeVarWithoutSuffix("x");
  auto x2 = VarName::makeVar("x", 1);
  auto y = VarName::makeVarWithoutSuffix("y");
  std::vector<TypedVar> v1 = {TypedVar(x1, BType::INT)};
  std::vector<TypedVar> v2 = {TypedVar(x2, BType::INT)};
  ctx.push(v1, v2);
  // x1 is bound, y is free - should not be equal
  EXPECT_FALSE(ctx.equals(x1, y));
}

TEST(Context, PopRestores) {
  Context ctx;
  auto x1 = VarName::makeVarWithoutSuffix("x");
  auto x2 = VarName::makeVar("x", 1);
  std::vector<TypedVar> v1 = {TypedVar(x1, BType::INT)};
  std::vector<TypedVar> v2 = {TypedVar(x2, BType::INT)};
  ctx.push(v1, v2);
  ctx.pop();
  // After pop, x1 and x2 are treated as free vars again
  EXPECT_FALSE(ctx.equals(x1, x2));
}
