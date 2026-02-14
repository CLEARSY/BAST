#include <gtest/gtest.h>

#include "predReader.h"
#include "predDesc.h"
#include "tinyxml2.h"

// typeInfos: [0]=INT, [1]=POW_INT
static const std::vector<BType> TI = {
    BType::INT,      // 0
    BType::POW_INT,  // 1
};

// --- Exp_Comparison ---

TEST(ReadPred, ComparisonEquality) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Exp_Comparison op="=">
    <Id value="x" typref="0"/>
    <Integer_Literal value="0" typref="0"/>
  </Exp_Comparison>)");
  auto p = Xml::readPredicate(doc.RootElement(), TI);
  EXPECT_EQ(p.getTag(), Pred::PKind::ExprComparison);
  EXPECT_EQ(p.toExprComparison().op, Pred::ComparisonOp::Equality);
}

TEST(ReadPred, ComparisonMembership) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Exp_Comparison op=":">
    <Id value="x" typref="0"/>
    <Id value="INTEGER" typref="1"/>
  </Exp_Comparison>)");
  auto p = Xml::readPredicate(doc.RootElement(), TI);
  EXPECT_EQ(p.toExprComparison().op, Pred::ComparisonOp::Membership);
}

TEST(ReadPred, ComparisonSubset) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Exp_Comparison op="&lt;:">
    <Id value="s" typref="1"/>
    <Id value="INTEGER" typref="1"/>
  </Exp_Comparison>)");
  auto p = Xml::readPredicate(doc.RootElement(), TI);
  EXPECT_EQ(p.toExprComparison().op, Pred::ComparisonOp::Subset);
}

TEST(ReadPred, ComparisonStrictSubset) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Exp_Comparison op="&lt;&lt;:">
    <Id value="s" typref="1"/>
    <Id value="INTEGER" typref="1"/>
  </Exp_Comparison>)");
  auto p = Xml::readPredicate(doc.RootElement(), TI);
  EXPECT_EQ(p.toExprComparison().op, Pred::ComparisonOp::Strict_Subset);
}

TEST(ReadPred, ComparisonIgt) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Exp_Comparison op="&gt;i">
    <Id value="x" typref="0"/>
    <Integer_Literal value="0" typref="0"/>
  </Exp_Comparison>)");
  auto p = Xml::readPredicate(doc.RootElement(), TI);
  EXPECT_EQ(p.toExprComparison().op, Pred::ComparisonOp::Igt);
}

TEST(ReadPred, ComparisonIlt) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Exp_Comparison op="&lt;i">
    <Id value="x" typref="0"/>
    <Integer_Literal value="0" typref="0"/>
  </Exp_Comparison>)");
  auto p = Xml::readPredicate(doc.RootElement(), TI);
  EXPECT_EQ(p.toExprComparison().op, Pred::ComparisonOp::Ilt);
}

TEST(ReadPred, ComparisonIge) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Exp_Comparison op="&gt;=i">
    <Id value="x" typref="0"/>
    <Integer_Literal value="0" typref="0"/>
  </Exp_Comparison>)");
  auto p = Xml::readPredicate(doc.RootElement(), TI);
  EXPECT_EQ(p.toExprComparison().op, Pred::ComparisonOp::Ige);
}

TEST(ReadPred, ComparisonIle) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Exp_Comparison op="&lt;=i">
    <Id value="x" typref="0"/>
    <Integer_Literal value="0" typref="0"/>
  </Exp_Comparison>)");
  auto p = Xml::readPredicate(doc.RootElement(), TI);
  EXPECT_EQ(p.toExprComparison().op, Pred::ComparisonOp::Ile);
}

// --- Negated comparisons ---

TEST(ReadPred, NegatedMembership) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Exp_Comparison op="/:">
    <Id value="x" typref="0"/>
    <Id value="INTEGER" typref="1"/>
  </Exp_Comparison>)");
  auto p = Xml::readPredicate(doc.RootElement(), TI);
  EXPECT_EQ(p.getTag(), Pred::PKind::Negation);
  auto &inner = p.toNegation().operand;
  EXPECT_EQ(inner.getTag(), Pred::PKind::ExprComparison);
  EXPECT_EQ(inner.toExprComparison().op, Pred::ComparisonOp::Membership);
}

TEST(ReadPred, NegatedSubset) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Exp_Comparison op="/&lt;:">
    <Id value="s" typref="1"/>
    <Id value="INTEGER" typref="1"/>
  </Exp_Comparison>)");
  auto p = Xml::readPredicate(doc.RootElement(), TI);
  EXPECT_EQ(p.getTag(), Pred::PKind::Negation);
  EXPECT_EQ(p.toNegation().operand.toExprComparison().op,
            Pred::ComparisonOp::Subset);
}

TEST(ReadPred, NegatedStrictSubset) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Exp_Comparison op="/&lt;&lt;:">
    <Id value="s" typref="1"/>
    <Id value="INTEGER" typref="1"/>
  </Exp_Comparison>)");
  auto p = Xml::readPredicate(doc.RootElement(), TI);
  EXPECT_EQ(p.getTag(), Pred::PKind::Negation);
  EXPECT_EQ(p.toNegation().operand.toExprComparison().op,
            Pred::ComparisonOp::Strict_Subset);
}

TEST(ReadPred, NegatedEquality) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Exp_Comparison op="/=">
    <Id value="x" typref="0"/>
    <Integer_Literal value="0" typref="0"/>
  </Exp_Comparison>)");
  auto p = Xml::readPredicate(doc.RootElement(), TI);
  EXPECT_EQ(p.getTag(), Pred::PKind::Negation);
  EXPECT_EQ(p.toNegation().operand.toExprComparison().op,
            Pred::ComparisonOp::Equality);
}

// --- Binary predicates ---

TEST(ReadPred, Implication) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Binary_Pred op="=&gt;">
    <Exp_Comparison op="=">
      <Id value="x" typref="0"/>
      <Integer_Literal value="0" typref="0"/>
    </Exp_Comparison>
    <Exp_Comparison op="=">
      <Id value="x" typref="0"/>
      <Integer_Literal value="0" typref="0"/>
    </Exp_Comparison>
  </Binary_Pred>)");
  auto p = Xml::readPredicate(doc.RootElement(), TI);
  EXPECT_EQ(p.getTag(), Pred::PKind::Implication);
}

TEST(ReadPred, Equivalence) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Binary_Pred op="&lt;=&gt;">
    <Exp_Comparison op="=">
      <Id value="x" typref="0"/>
      <Integer_Literal value="0" typref="0"/>
    </Exp_Comparison>
    <Exp_Comparison op="=">
      <Id value="x" typref="0"/>
      <Integer_Literal value="0" typref="0"/>
    </Exp_Comparison>
  </Binary_Pred>)");
  auto p = Xml::readPredicate(doc.RootElement(), TI);
  EXPECT_EQ(p.getTag(), Pred::PKind::Equivalence);
}

// --- Unary predicate ---

TEST(ReadPred, Negation) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Unary_Pred op="not">
    <Exp_Comparison op="=">
      <Id value="x" typref="0"/>
      <Integer_Literal value="0" typref="0"/>
    </Exp_Comparison>
  </Unary_Pred>)");
  auto p = Xml::readPredicate(doc.RootElement(), TI);
  EXPECT_EQ(p.getTag(), Pred::PKind::Negation);
}

// --- Nary predicates ---

TEST(ReadPred, Conjunction) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Nary_Pred op="&amp;">
    <Exp_Comparison op="=">
      <Id value="x" typref="0"/>
      <Integer_Literal value="0" typref="0"/>
    </Exp_Comparison>
    <Exp_Comparison op="=">
      <Id value="x" typref="0"/>
      <Integer_Literal value="1" typref="0"/>
    </Exp_Comparison>
  </Nary_Pred>)");
  auto p = Xml::readPredicate(doc.RootElement(), TI);
  EXPECT_EQ(p.getTag(), Pred::PKind::Conjunction);
  EXPECT_EQ(p.toConjunction().operands.size(), 2u);
}

TEST(ReadPred, Disjunction) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Nary_Pred op="or">
    <Exp_Comparison op="=">
      <Id value="x" typref="0"/>
      <Integer_Literal value="0" typref="0"/>
    </Exp_Comparison>
    <Exp_Comparison op="=">
      <Id value="x" typref="0"/>
      <Integer_Literal value="1" typref="0"/>
    </Exp_Comparison>
  </Nary_Pred>)");
  auto p = Xml::readPredicate(doc.RootElement(), TI);
  EXPECT_EQ(p.getTag(), Pred::PKind::Disjunction);
  EXPECT_EQ(p.toDisjunction().operands.size(), 2u);
}

// --- Quantified predicates ---

TEST(ReadPred, Forall) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Quantified_Pred type="!">
    <Variables>
      <Id value="x" typref="0"/>
    </Variables>
    <Body>
      <Exp_Comparison op="&gt;i">
        <Id value="x" typref="0"/>
        <Integer_Literal value="0" typref="0"/>
      </Exp_Comparison>
    </Body>
  </Quantified_Pred>)");
  auto p = Xml::readPredicate(doc.RootElement(), TI);
  EXPECT_EQ(p.getTag(), Pred::PKind::Forall);
  EXPECT_EQ(p.toForall().vars.size(), 1u);
}

TEST(ReadPred, Exists) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Quantified_Pred type="#">
    <Variables>
      <Id value="x" typref="0"/>
    </Variables>
    <Body>
      <Exp_Comparison op="=">
        <Id value="x" typref="0"/>
        <Integer_Literal value="42" typref="0"/>
      </Exp_Comparison>
    </Body>
  </Quantified_Pred>)");
  auto p = Xml::readPredicate(doc.RootElement(), TI);
  EXPECT_EQ(p.getTag(), Pred::PKind::Exists);
  EXPECT_EQ(p.toExists().vars.size(), 1u);
}

TEST(ReadPred, ForallMultipleVars) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Quantified_Pred type="!">
    <Variables>
      <Id value="x" typref="0"/>
      <Id value="y" typref="0"/>
    </Variables>
    <Body>
      <Exp_Comparison op="=">
        <Id value="x" typref="0"/>
        <Id value="y" typref="0"/>
      </Exp_Comparison>
    </Body>
  </Quantified_Pred>)");
  auto p = Xml::readPredicate(doc.RootElement(), TI);
  EXPECT_EQ(p.getTag(), Pred::PKind::Forall);
  EXPECT_EQ(p.toForall().vars.size(), 2u);
}

// --- Tag wrapper ---

TEST(ReadPred, TagWrapper) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Tag>
    <Exp_Comparison op="=">
      <Id value="x" typref="0"/>
      <Integer_Literal value="0" typref="0"/>
    </Exp_Comparison>
  </Tag>)");
  auto p = Xml::readPredicate(doc.RootElement(), TI);
  EXPECT_EQ(p.getTag(), Pred::PKind::ExprComparison);
  EXPECT_EQ(p.toExprComparison().op, Pred::ComparisonOp::Equality);
}

// --- Error cases ---

TEST(ReadPred, NullDomThrows) {
  EXPECT_THROW(Xml::readPredicate(nullptr, TI), Xml::PredReaderException);
}

TEST(ReadPred, UnknownTagThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Bogus/>");
  EXPECT_THROW(Xml::readPredicate(doc.RootElement(), TI),
               Xml::PredReaderException);
}

TEST(ReadPred, UnknownBinaryPredOpThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Binary_Pred op="???">
    <Exp_Comparison op="=">
      <Id value="x" typref="0"/>
      <Integer_Literal value="0" typref="0"/>
    </Exp_Comparison>
    <Exp_Comparison op="=">
      <Id value="x" typref="0"/>
      <Integer_Literal value="0" typref="0"/>
    </Exp_Comparison>
  </Binary_Pred>)");
  EXPECT_THROW(Xml::readPredicate(doc.RootElement(), TI),
               Xml::PredReaderException);
}

TEST(ReadPred, UnknownComparisonOpThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Exp_Comparison op="???">
    <Id value="x" typref="0"/>
    <Integer_Literal value="0" typref="0"/>
  </Exp_Comparison>)");
  EXPECT_THROW(Xml::readPredicate(doc.RootElement(), TI),
               Xml::PredReaderException);
}

TEST(ReadPred, UnknownUnaryPredOpThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Unary_Pred op="???">
    <Exp_Comparison op="=">
      <Id value="x" typref="0"/>
      <Integer_Literal value="0" typref="0"/>
    </Exp_Comparison>
  </Unary_Pred>)");
  EXPECT_THROW(Xml::readPredicate(doc.RootElement(), TI),
               Xml::PredReaderException);
}

TEST(ReadPred, UnknownNaryPredOpThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Nary_Pred op="???">
    <Exp_Comparison op="=">
      <Id value="x" typref="0"/>
      <Integer_Literal value="0" typref="0"/>
    </Exp_Comparison>
  </Nary_Pred>)");
  EXPECT_THROW(Xml::readPredicate(doc.RootElement(), TI),
               Xml::PredReaderException);
}

TEST(ReadPred, UnknownQuantifiedPredTypeThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Quantified_Pred type="???">
    <Variables><Id value="x" typref="0"/></Variables>
    <Body>
      <Exp_Comparison op="=">
        <Id value="x" typref="0"/>
        <Integer_Literal value="0" typref="0"/>
      </Exp_Comparison>
    </Body>
  </Quantified_Pred>)");
  EXPECT_THROW(Xml::readPredicate(doc.RootElement(), TI),
               Xml::PredReaderException);
}
