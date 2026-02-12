#include "btype.h"

#include <gtest/gtest.h>

// --- Primitive types ---

TEST(BType, PrimitiveKinds) {
  EXPECT_EQ(BType::INT.getKind(), BType::Kind::INTEGER);
  EXPECT_EQ(BType::BOOL.getKind(), BType::Kind::BOOLEAN);
  EXPECT_EQ(BType::FLOAT.getKind(), BType::Kind::FLOAT);
  EXPECT_EQ(BType::REAL.getKind(), BType::Kind::REAL);
  EXPECT_EQ(BType::STRING.getKind(), BType::Kind::STRING);
}

TEST(BType, PrimitiveEquality) {
  EXPECT_EQ(BType::INT, BType::INT);
  EXPECT_EQ(BType::BOOL, BType::BOOL);
  EXPECT_NE(BType::INT, BType::BOOL);
  EXPECT_NE(BType::FLOAT, BType::REAL);
  EXPECT_NE(BType::STRING, BType::INT);
}

TEST(BType, PrimitiveToString) {
  EXPECT_EQ(BType::INT.to_string(), "INT");
  EXPECT_EQ(BType::BOOL.to_string(), "BOOL");
  EXPECT_EQ(BType::FLOAT.to_string(), "FLOAT");
  EXPECT_EQ(BType::REAL.to_string(), "REAL");
  EXPECT_EQ(BType::STRING.to_string(), "STRING");
}

TEST(BType, DefaultConstructorIsInteger) {
  BType t;
  EXPECT_EQ(t.getKind(), BType::Kind::INTEGER);
  EXPECT_EQ(t, BType::INT);
}

// --- Power types ---

TEST(BType, PowerTypeKind) {
  auto t = BType::POW(BType::INT);
  EXPECT_EQ(t.getKind(), BType::Kind::PowerType);
}

TEST(BType, PowerTypeContent) {
  auto t = BType::POW(BType::INT);
  EXPECT_EQ(t.toPowerType().content, BType::INT);
}

TEST(BType, PowerTypeEquality) {
  EXPECT_EQ(BType::POW(BType::INT), BType::POW(BType::INT));
  EXPECT_NE(BType::POW(BType::INT), BType::POW(BType::BOOL));
}

TEST(BType, PowerTypeNested) {
  auto t = BType::POW(BType::POW(BType::INT));
  EXPECT_EQ(t.getKind(), BType::Kind::PowerType);
  auto inner = t.toPowerType().content;
  EXPECT_EQ(inner.getKind(), BType::Kind::PowerType);
  EXPECT_EQ(inner.toPowerType().content, BType::INT);
}

TEST(BType, PowerTypeToString) {
  EXPECT_EQ(BType::POW(BType::INT).to_string(), "POW(INT)");
  EXPECT_EQ(BType::POW(BType::POW(BType::BOOL)).to_string(),
            "POW(POW(BOOL))");
}

TEST(BType, PrebuiltPowerTypes) {
  EXPECT_EQ(BType::POW_INT, BType::POW(BType::INT));
  EXPECT_EQ(BType::POW_BOOL, BType::POW(BType::BOOL));
  EXPECT_EQ(BType::POW_FLOAT, BType::POW(BType::FLOAT));
  EXPECT_EQ(BType::POW_STRING, BType::POW(BType::STRING));
  EXPECT_EQ(BType::POW_REAL, BType::POW(BType::REAL));
}

// --- Product types ---

TEST(BType, ProductTypeKind) {
  auto t = BType::PROD(BType::INT, BType::BOOL);
  EXPECT_EQ(t.getKind(), BType::Kind::ProductType);
}

TEST(BType, ProductTypeComponents) {
  auto t = BType::PROD(BType::INT, BType::BOOL);
  EXPECT_EQ(t.toProductType().lhs, BType::INT);
  EXPECT_EQ(t.toProductType().rhs, BType::BOOL);
}

TEST(BType, ProductTypeEquality) {
  EXPECT_EQ(BType::PROD(BType::INT, BType::BOOL),
            BType::PROD(BType::INT, BType::BOOL));
  EXPECT_NE(BType::PROD(BType::INT, BType::BOOL),
            BType::PROD(BType::BOOL, BType::INT));
}

TEST(BType, ProductTypeToString) {
  EXPECT_EQ(BType::PROD(BType::INT, BType::BOOL).to_string(),
            "PROD(INT, BOOL)");
}

TEST(BType, RelationInteger) {
  EXPECT_EQ(BType::RELATION_INTEGER, BType::POW(BType::PROD(BType::INT, BType::INT)));
}

// --- Abstract sets ---

TEST(BType, AbstractSetKind) {
  auto t = BType::SET("COLORS");
  EXPECT_EQ(t.getKind(), BType::Kind::AbstractSet);
}

TEST(BType, AbstractSetName) {
  auto t = BType::ABSTRACT_SET("COLORS");
  EXPECT_EQ(t.toAbstractSetType().getName(), "COLORS");
}

TEST(BType, AbstractSetEquality) {
  EXPECT_EQ(BType::SET("S"), BType::SET("S"));
  EXPECT_NE(BType::SET("S"), BType::SET("T"));
}

TEST(BType, AbstractSetToString) {
  EXPECT_EQ(BType::SET("COLORS").to_string(), "ASET(COLORS)");
}

TEST(BType, SetAndAbstractSetEqual) {
  EXPECT_EQ(BType::SET("X"), BType::ABSTRACT_SET("X"));
}

// --- Enumerated sets ---

TEST(BType, EnumeratedSetKind) {
  auto t = BType::ENUMERATED_SET({"COLORS", {"red", "green", "blue"}});
  EXPECT_EQ(t.getKind(), BType::Kind::EnumeratedSet);
}

TEST(BType, EnumeratedSetAccessors) {
  auto t = BType::ENUMERATED_SET({"COLORS", {"red", "green", "blue"}});
  EXPECT_EQ(t.toEnumeratedSetType().getName(), "COLORS");
  auto &content = t.toEnumeratedSetType().getContent();
  ASSERT_EQ(content.size(), 3u);
  EXPECT_EQ(content[0], "red");
  EXPECT_EQ(content[1], "green");
  EXPECT_EQ(content[2], "blue");
}

TEST(BType, EnumeratedSetToString) {
  auto t = BType::ENUMERATED_SET({"COLORS", {"red", "green"}});
  EXPECT_EQ(t.to_string(), "ESET(COLORS)");
}

// --- Struct types ---

TEST(BType, StructKind) {
  auto t = BType::STRUCT({{"x", BType::INT}, {"y", BType::BOOL}});
  EXPECT_EQ(t.getKind(), BType::Kind::Struct);
}

TEST(BType, StructFieldsSorted) {
  auto t = BType::STRUCT({{"z", BType::INT}, {"a", BType::BOOL}});
  auto &fields = t.toRecordType().m_fields;
  ASSERT_EQ(fields.size(), 2u);
  EXPECT_EQ(fields[0].first, "a");
  EXPECT_EQ(fields[1].first, "z");
}

TEST(BType, StructEquality) {
  auto t1 = BType::STRUCT({{"x", BType::INT}, {"y", BType::BOOL}});
  auto t2 = BType::STRUCT({{"y", BType::BOOL}, {"x", BType::INT}});
  EXPECT_EQ(t1, t2);
}

TEST(BType, StructInequality) {
  auto t1 = BType::STRUCT({{"x", BType::INT}});
  auto t2 = BType::STRUCT({{"x", BType::BOOL}});
  EXPECT_NE(t1, t2);
}

TEST(BType, StructToString) {
  auto t = BType::STRUCT({{"x", BType::INT}, {"y", BType::BOOL}});
  EXPECT_EQ(t.to_string(), "STRUCT(x : INT, y : BOOL)");
}

// --- Comparison ---

TEST(BType, CompareReflexive) {
  EXPECT_EQ(BType::compare(BType::INT, BType::INT), 0);
}

TEST(BType, CompareTransitive) {
  // Different kinds have an ordering
  auto cmp1 = BType::compare(BType::INT, BType::BOOL);
  auto cmp2 = BType::compare(BType::BOOL, BType::INT);
  EXPECT_NE(cmp1, 0);
  EXPECT_EQ(cmp1, -cmp2);
}

TEST(BType, CompareOperators) {
  EXPECT_TRUE(BType::INT == BType::INT);
  EXPECT_TRUE(BType::INT != BType::BOOL);
  // One of these must be true
  EXPECT_TRUE(BType::INT < BType::BOOL || BType::INT > BType::BOOL);
}

// Note: BType::vec_compare is declared but not implemented in the library

// --- Hashing ---

TEST(BType, HashConsistency) {
  EXPECT_EQ(BType::INT.hash_combine(0), BType::INT.hash_combine(0));
  EXPECT_EQ(BType::POW(BType::INT).hash_combine(0),
            BType::POW(BType::INT).hash_combine(0));
}

TEST(BType, HashEqualObjectsSameHash) {
  auto t1 = BType::PROD(BType::INT, BType::BOOL);
  auto t2 = BType::PROD(BType::INT, BType::BOOL);
  EXPECT_EQ(t1, t2);
  EXPECT_EQ(t1.hash_combine(0), t2.hash_combine(0));
}

// --- Visitor ---

class TestVisitor : public BType::Visitor {
 public:
  BType::Kind visited = BType::Kind::INTEGER;
  void visitINTEGER() override { visited = BType::Kind::INTEGER; }
  void visitBOOLEAN() override { visited = BType::Kind::BOOLEAN; }
  void visitFLOAT() override { visited = BType::Kind::FLOAT; }
  void visitREAL() override { visited = BType::Kind::REAL; }
  void visitSTRING() override { visited = BType::Kind::STRING; }
  void visitProductType(const BType &, const BType &) override {
    visited = BType::Kind::ProductType;
  }
  void visitPowerType(const BType &) override {
    visited = BType::Kind::PowerType;
  }
  void visitRecordType(
      const std::vector<std::pair<std::string, BType>> &) override {
    visited = BType::Kind::Struct;
  }
  void visitAbstractSet(const BType::AbstractSet &) override {
    visited = BType::Kind::AbstractSet;
  }
  void visitEnumeratedSet(const BType::EnumeratedSet &) override {
    visited = BType::Kind::EnumeratedSet;
  }
};

TEST(BType, VisitorPrimitive) {
  TestVisitor v;
  BType::INT.accept(v);
  EXPECT_EQ(v.visited, BType::Kind::INTEGER);
  BType::BOOL.accept(v);
  EXPECT_EQ(v.visited, BType::Kind::BOOLEAN);
}

TEST(BType, VisitorPowerType) {
  TestVisitor v;
  BType::POW(BType::INT).accept(v);
  EXPECT_EQ(v.visited, BType::Kind::PowerType);
}

TEST(BType, VisitorProductType) {
  TestVisitor v;
  BType::PROD(BType::INT, BType::BOOL).accept(v);
  EXPECT_EQ(v.visited, BType::Kind::ProductType);
}

TEST(BType, VisitorStruct) {
  TestVisitor v;
  BType::STRUCT({{"x", BType::INT}}).accept(v);
  EXPECT_EQ(v.visited, BType::Kind::Struct);
}

TEST(BType, VisitorAbstractSet) {
  TestVisitor v;
  BType::SET("S").accept(v);
  EXPECT_EQ(v.visited, BType::Kind::AbstractSet);
}

TEST(BType, VisitorEnumeratedSet) {
  TestVisitor v;
  BType::ENUMERATED_SET({"S", {"a", "b"}}).accept(v);
  EXPECT_EQ(v.visited, BType::Kind::EnumeratedSet);
}
