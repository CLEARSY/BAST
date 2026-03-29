#include "pred.h"
#include "predDesc.h"

#include <gtest/gtest.h>

// --- Constants ---

TEST(Pred, True) {
  auto p = Pred::makeTrue();
  EXPECT_EQ(p.getTag(), Pred::PKind::True);
}

TEST(Pred, False) {
  auto p = Pred::makeFalse();
  EXPECT_EQ(p.getTag(), Pred::PKind::False);
}

// --- Implication ---

TEST(Pred, Implication) {
  auto p = Pred::makeImplication(Pred::makeTrue(), Pred::makeFalse());
  EXPECT_EQ(p.getTag(), Pred::PKind::Implication);
  auto &impl = p.toImplication();
  EXPECT_EQ(impl.lhs.getTag(), Pred::PKind::True);
  EXPECT_EQ(impl.rhs.getTag(), Pred::PKind::False);
}

// --- Equivalence ---

TEST(Pred, Equivalence) {
  auto p = Pred::makeEquivalence(Pred::makeTrue(), Pred::makeTrue());
  EXPECT_EQ(p.getTag(), Pred::PKind::Equivalence);
}

// --- Negation ---

TEST(Pred, Negation) {
  auto p = Pred::makeNegation(Pred::makeTrue());
  EXPECT_EQ(p.getTag(), Pred::PKind::Negation);
  auto &neg = p.toNegation();
  EXPECT_EQ(neg.operand.getTag(), Pred::PKind::True);
}

// --- Conjunction ---

TEST(Pred, Conjunction) {
  std::vector<Pred> vec;
  vec.push_back(Pred::makeTrue());
  vec.push_back(Pred::makeFalse());
  auto p = Pred::makeConjunction(std::move(vec));
  EXPECT_EQ(p.getTag(), Pred::PKind::Conjunction);
  auto &conj = p.toConjunction();
  EXPECT_EQ(conj.operands.size(), 2u);
}

// --- Disjunction ---

TEST(Pred, Disjunction) {
  std::vector<Pred> vec;
  vec.push_back(Pred::makeTrue());
  vec.push_back(Pred::makeFalse());
  auto p = Pred::makeDisjunction(std::move(vec));
  EXPECT_EQ(p.getTag(), Pred::PKind::Disjunction);
  auto &disj = p.toDisjunction();
  EXPECT_EQ(disj.operands.size(), 2u);
}

// --- ExprComparison ---

TEST(Pred, ExprComparison) {
  auto lhs = Expr::makeIdent(VarName::makeVarWithoutSuffix("x"), BType::INT);
  auto rhs = Expr::makeInteger("0");
  auto p = Pred::makeExprComparison(Pred::ComparisonOp::Equality,
                                    std::move(lhs), std::move(rhs));
  EXPECT_EQ(p.getTag(), Pred::PKind::ExprComparison);
  auto &cmp = p.toExprComparison();
  EXPECT_EQ(cmp.op, Pred::ComparisonOp::Equality);
}

TEST(Pred, ExprComparisonMembership) {
  auto x = Expr::makeIdent(VarName::makeVarWithoutSuffix("x"), BType::INT);
  auto s = Expr::makeINTEGER();
  auto p = Pred::makeExprComparison(Pred::ComparisonOp::Membership,
                                    std::move(x), std::move(s));
  EXPECT_EQ(p.getTag(), Pred::PKind::ExprComparison);
  EXPECT_EQ(p.toExprComparison().op, Pred::ComparisonOp::Membership);
}

// --- Forall ---

TEST(Pred, Forall) {
  auto x = VarName::makeVarWithoutSuffix("x");
  std::vector<TypedVar> vars = {TypedVar(x, BType::INT)};
  auto body = Pred::makeTrue();
  auto p = Pred::makeForall(vars, std::move(body));
  EXPECT_EQ(p.getTag(), Pred::PKind::Forall);
  auto &fa = p.toForall();
  EXPECT_EQ(fa.vars.size(), 1u);
}

// --- Exists ---

TEST(Pred, Exists) {
  auto x = VarName::makeVarWithoutSuffix("x");
  std::vector<TypedVar> vars = {TypedVar(x, BType::INT)};
  auto body = Pred::makeTrue();
  auto p = Pred::makeExists(vars, std::move(body));
  EXPECT_EQ(p.getTag(), Pred::PKind::Exists);
  auto &ex = p.toExists();
  EXPECT_EQ(ex.vars.size(), 1u);
}

// --- GoalTag ---

TEST(Pred, GoalTag) {
  auto p = Pred::makeTrue("myGoal");
  EXPECT_EQ(p.getGoalTag(), "myGoal");
}

TEST(Pred, SetGoalTag) {
  auto p = Pred::makeTrue();
  p.setGoalTag("updated");
  EXPECT_EQ(p.getGoalTag(), "updated");
}

// --- BxmlTag ---

TEST(Pred, BxmlTag) {
  Pred::BXmlTags tags = {"t1", "t2"};
  auto p = Pred::makeTrue("", tags);
  EXPECT_EQ(p.getBxmlTag().size(), 2u);
  EXPECT_TRUE(p.getBxmlTag().count("t1"));
  EXPECT_TRUE(p.getBxmlTag().count("t2"));
}

// --- Copy ---

TEST(Pred, CopyTrue) {
  auto p = Pred::makeTrue("goal1");
  auto c = p.copy();
  EXPECT_EQ(c.getTag(), Pred::PKind::True);
  EXPECT_EQ(c.getGoalTag(), "goal1");
}

TEST(Pred, CopyImplication) {
  auto p = Pred::makeImplication(Pred::makeTrue(), Pred::makeFalse());
  auto c = p.copy();
  EXPECT_EQ(c.getTag(), Pred::PKind::Implication);
  EXPECT_EQ(Pred::compare(p, c), 0);
}

// --- Comparison ---

TEST(Pred, CompareEqual) {
  auto p1 = Pred::makeTrue();
  auto p2 = Pred::makeTrue();
  EXPECT_EQ(Pred::compare(p1, p2), 0);
}

TEST(Pred, CompareDifferent) {
  auto p1 = Pred::makeTrue();
  auto p2 = Pred::makeFalse();
  EXPECT_NE(Pred::compare(p1, p2), 0);
}

TEST(Pred, CompareImplication) {
  auto p1 = Pred::makeImplication(Pred::makeTrue(), Pred::makeFalse());
  auto p2 = Pred::makeImplication(Pred::makeTrue(), Pred::makeFalse());
  EXPECT_EQ(Pred::compare(p1, p2), 0);
}

TEST(Pred, VecCompareEqual) {
  std::vector<Pred> v1;
  v1.push_back(Pred::makeTrue());
  v1.push_back(Pred::makeFalse());
  std::vector<Pred> v2;
  v2.push_back(Pred::makeTrue());
  v2.push_back(Pred::makeFalse());
  EXPECT_EQ(Pred::vec_compare(v1, v2), 0);
}

TEST(Pred, VecCompareDifferentSize) {
  std::vector<Pred> v1;
  v1.push_back(Pred::makeTrue());
  std::vector<Pred> v2;
  v2.push_back(Pred::makeTrue());
  v2.push_back(Pred::makeFalse());
  EXPECT_LT(Pred::vec_compare(v1, v2), 0);
}

// --- Hashing ---

TEST(Pred, HashConsistency) {
  auto p1 = Pred::makeTrue();
  auto p2 = Pred::makeTrue();
  EXPECT_EQ(p1.hash_combine(0), p2.hash_combine(0));
}

TEST(Pred, HashEqualImpliesSameHash) {
  auto p1 = Pred::makeImplication(Pred::makeTrue(), Pred::makeFalse());
  auto p2 = Pred::makeImplication(Pred::makeTrue(), Pred::makeFalse());
  EXPECT_EQ(Pred::compare(p1, p2), 0);
  EXPECT_EQ(p1.hash_combine(0), p2.hash_combine(0));
}

TEST(Pred, StdHash) {
  auto p1 = Pred::makeTrue();
  auto p2 = Pred::makeTrue();
  std::hash<Pred> h;
  EXPECT_EQ(h(p1), h(p2));
}

// --- Free variables ---

TEST(Pred, FreeVarsTrue) {
  auto p = Pred::makeTrue();
  EXPECT_TRUE(p.getFreeVars().empty());
}

TEST(Pred, FreeVarsComparison) {
  auto x = VarName::makeVarWithoutSuffix("x");
  auto p = Pred::makeExprComparison(
      Pred::ComparisonOp::Equality, Expr::makeIdent(x, BType::INT),
      Expr::makeInteger("0"));
  auto fv = p.getFreeVars();
  EXPECT_EQ(fv.size(), 1u);
  EXPECT_TRUE(fv.count(x));
}

TEST(Pred, FreeVarsForallBound) {
  auto x = VarName::makeVarWithoutSuffix("x");
  std::vector<TypedVar> vars = {TypedVar(x, BType::INT)};
  auto body = Pred::makeExprComparison(
      Pred::ComparisonOp::Igt, Expr::makeIdent(x, BType::INT),
      Expr::makeInteger("0"));
  auto p = Pred::makeForall(vars, std::move(body));
  auto fv = p.getFreeVars();
  EXPECT_TRUE(fv.empty());
}

TEST(Pred, FreeVarsForallMixed) {
  auto x = VarName::makeVarWithoutSuffix("x");
  auto y = VarName::makeVarWithoutSuffix("y");
  std::vector<TypedVar> vars = {TypedVar(x, BType::INT)};
  auto body = Pred::makeExprComparison(
      Pred::ComparisonOp::Equality, Expr::makeIdent(x, BType::INT),
      Expr::makeIdent(y, BType::INT));
  auto p = Pred::makeForall(vars, std::move(body));
  auto fv = p.getFreeVars();
  EXPECT_EQ(fv.size(), 1u);
  EXPECT_TRUE(fv.count(y));
}

// --- getAllVars ---

TEST(Pred, GetAllVarsForall) {
  auto x = VarName::makeVarWithoutSuffix("x");
  auto y = VarName::makeVarWithoutSuffix("y");
  std::vector<TypedVar> vars = {TypedVar(x, BType::INT)};
  auto body = Pred::makeExprComparison(
      Pred::ComparisonOp::Equality, Expr::makeIdent(x, BType::INT),
      Expr::makeIdent(y, BType::INT));
  auto p = Pred::makeForall(vars, std::move(body));
  auto av = p.getAllVars();
  EXPECT_EQ(av.size(), 2u);
  EXPECT_TRUE(av.count(x));
  EXPECT_TRUE(av.count(y));
}

// --- getFreeTVars ---

TEST(Pred, GetFreeTVars) {
  auto x = VarName::makeVarWithoutSuffix("x");
  auto p = Pred::makeExprComparison(
      Pred::ComparisonOp::Equality, Expr::makeIdent(x, BType::INT),
      Expr::makeInteger("0"));
  auto ftv = p.getFreeTVars();
  EXPECT_EQ(ftv.size(), 1u);
  auto it = ftv.begin();
  EXPECT_EQ(it->name, x);
  EXPECT_EQ(it->type, BType::INT);
}

// --- Substitution ---

TEST(Pred, SubstComparison) {
  auto x = VarName::makeVarWithoutSuffix("x");
  auto p = Pred::makeExprComparison(
      Pred::ComparisonOp::Equality, Expr::makeIdent(x, BType::INT),
      Expr::makeInteger("0"));
  std::map<VarName, Expr> map;
  map.insert({x, Expr::makeInteger("5")});
  p.subst(map);
  auto &cmp = p.toExprComparison();
  EXPECT_EQ(cmp.lhs.getTag(), Expr::EKind::IntegerLiteral);
  EXPECT_EQ(cmp.lhs.getIntegerLiteral(), "5");
}

// --- Alpha renaming ---

TEST(Pred, AlphaRename) {
  auto x = VarName::makeVarWithoutSuffix("x");
  auto y = VarName::makeVarWithoutSuffix("y");
  auto p = Pred::makeExprComparison(
      Pred::ComparisonOp::Equality, Expr::makeIdent(x, BType::INT),
      Expr::makeInteger("0"));
  std::map<VarName, VarName> map;
  map.insert({x, y});
  p.alpha(map);
  auto &cmp = p.toExprComparison();
  EXPECT_EQ(cmp.lhs.getId(), y);
}

// --- Alpha equality ---

TEST(Pred, AlphaEqualsTrue) {
  EXPECT_TRUE(Pred::alpha_equals(Pred::makeTrue(), Pred::makeTrue()));
}

TEST(Pred, AlphaEqualsDifferent) {
  EXPECT_FALSE(Pred::alpha_equals(Pred::makeTrue(), Pred::makeFalse()));
}

TEST(Pred, AlphaEqualsForall) {
  // forall x . x > 0  vs  forall x$1 . x$1 > 0  (same prefix, diff suffix)
  auto x = VarName::makeVarWithoutSuffix("x");
  auto x1 = VarName::makeVar("x", 1);
  std::vector<TypedVar> v1 = {TypedVar(x, BType::INT)};
  std::vector<TypedVar> v2 = {TypedVar(x1, BType::INT)};
  auto p1 = Pred::makeForall(
      v1, Pred::makeExprComparison(Pred::ComparisonOp::Igt,
                                   Expr::makeIdent(x, BType::INT),
                                   Expr::makeInteger("0")));
  auto p2 = Pred::makeForall(
      v2, Pred::makeExprComparison(Pred::ComparisonOp::Igt,
                                   Expr::makeIdent(x1, BType::INT),
                                   Expr::makeInteger("0")));
  EXPECT_TRUE(Pred::alpha_equals(p1, p2));
}

TEST(Pred, AlphaNotEqualsForallDifferentPrefix) {
  // forall x . x > 0  vs  forall y . y > 0  (different prefix -> not equal)
  auto x = VarName::makeVarWithoutSuffix("x");
  auto y = VarName::makeVarWithoutSuffix("y");
  std::vector<TypedVar> v1 = {TypedVar(x, BType::INT)};
  std::vector<TypedVar> v2 = {TypedVar(y, BType::INT)};
  auto p1 = Pred::makeForall(
      v1, Pred::makeExprComparison(Pred::ComparisonOp::Igt,
                                   Expr::makeIdent(x, BType::INT),
                                   Expr::makeInteger("0")));
  auto p2 = Pred::makeForall(
      v2, Pred::makeExprComparison(Pred::ComparisonOp::Igt,
                                   Expr::makeIdent(y, BType::INT),
                                   Expr::makeInteger("0")));
  EXPECT_FALSE(Pred::alpha_equals(p1, p2));
}

// --- isPureTypingPredicate ---

TEST(Pred, IsPureTypingMembership) {
  // x : INTEGER is a pure typing predicate
  auto x = Expr::makeIdent(VarName::makeVarWithoutSuffix("x"), BType::INT);
  auto p = Pred::makeExprComparison(Pred::ComparisonOp::Membership,
                                    std::move(x), Expr::makeINTEGER());
  EXPECT_TRUE(p.isPureTypingPredicate());
}

TEST(Pred, IsNotPureTypingEquality) {
  // x = 3 is not a pure typing predicate
  auto x = Expr::makeIdent(VarName::makeVarWithoutSuffix("x"), BType::INT);
  auto p = Pred::makeExprComparison(Pred::ComparisonOp::Equality,
                                    std::move(x), Expr::makeInteger("3"));
  EXPECT_FALSE(p.isPureTypingPredicate());
}

TEST(Pred, IsNotPureTypingTrue) {
  auto p = Pred::makeTrue();
  EXPECT_FALSE(p.isPureTypingPredicate());
}

// --- Visitor ---

class TestPredVisitor : public Pred::Visitor {
 public:
  Pred::PKind visited = Pred::PKind::True;
  void visitImplication(const Pred &, const Pred &) override {
    visited = Pred::PKind::Implication;
  }
  void visitEquivalence(const Pred &, const Pred &) override {
    visited = Pred::PKind::Equivalence;
  }
  void visitExprComparison(Pred::ComparisonOp, const Expr &,
                           const Expr &) override {
    visited = Pred::PKind::ExprComparison;
  }
  void visitNegation(const Pred &) override {
    visited = Pred::PKind::Negation;
  }
  void visitConjunction(const std::vector<Pred> &) override {
    visited = Pred::PKind::Conjunction;
  }
  void visitDisjunction(const std::vector<Pred> &) override {
    visited = Pred::PKind::Disjunction;
  }
  void visitForall(const std::vector<TypedVar> &, const Pred &) override {
    visited = Pred::PKind::Forall;
  }
  void visitExists(const std::vector<TypedVar> &, const Pred &) override {
    visited = Pred::PKind::Exists;
  }
  void visitTrue() override { visited = Pred::PKind::True; }
  void visitFalse() override { visited = Pred::PKind::False; }
};

TEST(Pred, VisitorTrue) {
  TestPredVisitor v;
  Pred::makeTrue().accept(v);
  EXPECT_EQ(v.visited, Pred::PKind::True);
}

TEST(Pred, VisitorImplication) {
  TestPredVisitor v;
  Pred::makeImplication(Pred::makeTrue(), Pred::makeFalse()).accept(v);
  EXPECT_EQ(v.visited, Pred::PKind::Implication);
}

TEST(Pred, VisitorNegation) {
  TestPredVisitor v;
  Pred::makeNegation(Pred::makeTrue()).accept(v);
  EXPECT_EQ(v.visited, Pred::PKind::Negation);
}

TEST(Pred, VisitorConjunction) {
  TestPredVisitor v;
  std::vector<Pred> vec;
  vec.push_back(Pred::makeTrue());
  Pred::makeConjunction(std::move(vec)).accept(v);
  EXPECT_EQ(v.visited, Pred::PKind::Conjunction);
}

TEST(Pred, VisitorExprComparison) {
  TestPredVisitor v;
  Pred::makeExprComparison(Pred::ComparisonOp::Equality,
                           Expr::makeInteger("1"), Expr::makeInteger("1"))
      .accept(v);
  EXPECT_EQ(v.visited, Pred::PKind::ExprComparison);
}

TEST(Pred, VisitorForall) {
  TestPredVisitor v;
  std::vector<TypedVar> vars = {
      TypedVar(VarName::makeVarWithoutSuffix("x"), BType::INT)};
  Pred::makeForall(vars, Pred::makeTrue()).accept(v);
  EXPECT_EQ(v.visited, Pred::PKind::Forall);
}

// --- ComparisonOp to_string ---

TEST(Pred, ComparisonOpToString) {
  EXPECT_FALSE(Pred::to_string(Pred::ComparisonOp::Equality).empty());
  EXPECT_FALSE(Pred::to_string(Pred::ComparisonOp::Membership).empty());
}
