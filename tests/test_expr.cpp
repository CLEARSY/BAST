#include "expr.h"
#include "exprDesc.h"

#include <gtest/gtest.h>

#include "pred.h"

// --- Constants ---

TEST(Expr, MaxInt) {
  auto e = Expr::makeMaxInt();
  EXPECT_EQ(e.getTag(), Expr::EKind::MaxInt);
}

TEST(Expr, MinInt) {
  auto e = Expr::makeMinInt();
  EXPECT_EQ(e.getTag(), Expr::EKind::MinInt);
}

TEST(Expr, ConstantINTEGER) {
  auto e = Expr::makeINTEGER();
  EXPECT_EQ(e.getTag(), Expr::EKind::INTEGER);
  EXPECT_TRUE(e.isTypeExpression());
}

TEST(Expr, ConstantNATURAL) {
  auto e = Expr::makeNATURAL();
  EXPECT_EQ(e.getTag(), Expr::EKind::NATURAL);
}

TEST(Expr, ConstantBOOL) {
  auto e = Expr::makeBOOL();
  EXPECT_EQ(e.getTag(), Expr::EKind::BOOL);
}

TEST(Expr, ConstantTRUE) {
  auto e = Expr::makeTRUE();
  EXPECT_EQ(e.getTag(), Expr::EKind::TRUE);
}

TEST(Expr, ConstantFALSE) {
  auto e = Expr::makeFALSE();
  EXPECT_EQ(e.getTag(), Expr::EKind::FALSE);
}

TEST(Expr, Successor) {
  auto e = Expr::makeSuccessor();
  EXPECT_EQ(e.getTag(), Expr::EKind::Successor);
}

TEST(Expr, Predecessor) {
  auto e = Expr::makePredecessor();
  EXPECT_EQ(e.getTag(), Expr::EKind::Predecessor);
}

// --- Literals ---

TEST(Expr, IntegerLiteral) {
  auto e = Expr::makeInteger("42");
  EXPECT_EQ(e.getTag(), Expr::EKind::IntegerLiteral);
  EXPECT_EQ(e.getIntegerLiteral(), "42");
}

TEST(Expr, StringLiteral) {
  auto e = Expr::makeString("hello");
  EXPECT_EQ(e.getTag(), Expr::EKind::StringLiteral);
  EXPECT_EQ(e.getStringLiteral(), "hello");
}

TEST(Expr, RealLiteral) {
  Expr::Decimal d("3", "14");
  auto e = Expr::makeReal(d);
  EXPECT_EQ(e.getTag(), Expr::EKind::RealLiteral);
  EXPECT_EQ(e.getRealLiteral().integerPart, "3");
  EXPECT_EQ(e.getRealLiteral().fractionalPart, "14");
}

// --- Decimal ---

TEST(Decimal, Compare) {
  Expr::Decimal d1("3", "14");
  Expr::Decimal d2("3", "14");
  EXPECT_EQ(d1.compare(d2), 0);
}

TEST(Decimal, CompareDifferent) {
  Expr::Decimal d1("3", "14");
  Expr::Decimal d2("4", "0");
  EXPECT_NE(d1.compare(d2), 0);
}

TEST(Decimal, IntegerOnly) {
  Expr::Decimal d("42");
  EXPECT_EQ(d.integerPart, "42");
  EXPECT_EQ(d.fractionalPart, "0");
}

// --- Identifiers ---

TEST(Expr, Ident) {
  auto x = VarName::makeVarWithoutSuffix("x");
  auto e = Expr::makeIdent(x, BType::INT);
  EXPECT_EQ(e.getTag(), Expr::EKind::Id);
  EXPECT_EQ(e.getId(), x);
  EXPECT_EQ(e.getType(), BType::INT);
}

// --- EmptySet ---

TEST(Expr, EmptySet) {
  auto e = Expr::makeEmptySet(BType::POW_INT);
  EXPECT_EQ(e.getTag(), Expr::EKind::EmptySet);
  EXPECT_EQ(e.getType(), BType::POW_INT);
}

// --- Binary expressions ---

TEST(Expr, BinaryExpr) {
  auto lhs = Expr::makeInteger("1");
  auto rhs = Expr::makeInteger("2");
  auto e = Expr::makeBinaryExpr(Expr::BinaryOp::IAddition, std::move(lhs),
                                std::move(rhs), BType::INT);
  EXPECT_EQ(e.getTag(), Expr::EKind::BinaryExpr);
  EXPECT_EQ(e.getType(), BType::INT);
  auto &bin = e.toBinaryExpr();
  EXPECT_EQ(bin.op, Expr::BinaryOp::IAddition);
}

// --- Unary expressions ---

TEST(Expr, UnaryExpr) {
  auto inner = Expr::makeIdent(VarName::makeVarWithoutSuffix("s"), BType::POW_INT);
  auto e =
      Expr::makeUnaryExpr(Expr::UnaryOp::Cardinality, std::move(inner), BType::INT);
  EXPECT_EQ(e.getTag(), Expr::EKind::UnaryExpr);
  EXPECT_EQ(e.toUnaryExpr().op, Expr::UnaryOp::Cardinality);
}

// --- Nary expressions ---

TEST(Expr, NaryExprSet) {
  std::vector<Expr> elems;
  elems.push_back(Expr::makeInteger("1"));
  elems.push_back(Expr::makeInteger("2"));
  auto e =
      Expr::makeNaryExpr(Expr::NaryOp::Set, std::move(elems), BType::POW_INT);
  EXPECT_EQ(e.getTag(), Expr::EKind::NaryExpr);
  EXPECT_EQ(e.toNaryExpr().op, Expr::NaryOp::Set);
}

TEST(Expr, NaryExprSequence) {
  std::vector<Expr> elems;
  elems.push_back(Expr::makeInteger("1"));
  auto seqType = BType::POW(BType::PROD(BType::INT, BType::INT));
  auto e =
      Expr::makeNaryExpr(Expr::NaryOp::Sequence, std::move(elems), seqType);
  EXPECT_EQ(e.getTag(), Expr::EKind::NaryExpr);
  EXPECT_EQ(e.toNaryExpr().op, Expr::NaryOp::Sequence);
}

// --- Ternary expressions ---

TEST(Expr, TernaryExpr) {
  auto fst = Expr::makeInteger("1");
  auto snd = Expr::makeInteger("2");
  auto thd = Expr::makeInteger("3");
  auto e = Expr::makeTernaryExpr(Expr::TernaryOp::Son, std::move(fst),
                                 std::move(snd), std::move(thd), BType::INT);
  EXPECT_EQ(e.getTag(), Expr::EKind::TernaryExpr);
  EXPECT_EQ(e.toTernaryExpr().op, Expr::TernaryOp::Son);
}

// --- Quantified expressions ---

TEST(Expr, QuantifiedExpr) {
  auto x = VarName::makeVarWithoutSuffix("x");
  std::vector<TypedVar> vars = {TypedVar(x, BType::INT)};
  auto cond = Pred::makeTrue();
  auto body = Expr::makeIdent(x, BType::INT);
  auto e = Expr::makeQuantifiedExpr(Expr::QuantifiedOp::Lambda, vars,
                                    std::move(cond), std::move(body),
                                    BType::POW(BType::PROD(BType::INT, BType::INT)));
  EXPECT_EQ(e.getTag(), Expr::EKind::QuantifiedExpr);
}

// --- Quantified set ---

TEST(Expr, QuantifiedSet) {
  auto x = VarName::makeVarWithoutSuffix("x");
  std::vector<TypedVar> vars = {TypedVar(x, BType::INT)};
  auto xExpr = Expr::makeIdent(x, BType::INT);
  auto cond = Pred::makeExprComparison(Pred::ComparisonOp::Igt,
                                       std::move(xExpr), Expr::makeInteger("0"));
  auto e = Expr::makeQuantifiedSet(vars, std::move(cond), BType::POW_INT);
  EXPECT_EQ(e.getTag(), Expr::EKind::QuantifiedSet);
}

// --- BooleanExpr ---

TEST(Expr, BooleanExpr) {
  auto p = Pred::makeTrue();
  auto e = Expr::makeBooleanExpr(std::move(p));
  EXPECT_EQ(e.getTag(), Expr::EKind::BooleanExpr);
}

// --- Record / Struct ---

TEST(Expr, RecordExpr) {
  std::vector<std::pair<std::string, Expr>> fields;
  fields.push_back({"a", Expr::makeInteger("1")});
  fields.push_back({"b", Expr::makeInteger("2")});
  auto recType = BType::STRUCT({{"a", BType::INT}, {"b", BType::INT}});
  auto e = Expr::makeRecord(std::move(fields), recType);
  EXPECT_EQ(e.getTag(), Expr::EKind::Record);
}

TEST(Expr, StructExpr) {
  std::vector<std::pair<std::string, Expr>> fields;
  fields.push_back({"a", Expr::makeINTEGER()});
  auto structType = BType::STRUCT({{"a", BType::POW_INT}});
  auto e = Expr::makeStruct(std::move(fields), structType);
  EXPECT_EQ(e.getTag(), Expr::EKind::Struct);
}

// --- Record field access ---

TEST(Expr, RecordFieldAccess) {
  auto x = Expr::makeIdent(
      VarName::makeVarWithoutSuffix("r"),
      BType::STRUCT({{"a", BType::INT}, {"b", BType::BOOL}}));
  auto e = Expr::makeRecordFieldAccess(std::move(x), "a", BType::INT);
  EXPECT_EQ(e.getTag(), Expr::EKind::Record_Field_Access);
}

// --- Record field update ---

TEST(Expr, RecordFieldUpdate) {
  auto recType = BType::STRUCT({{"a", BType::INT}});
  auto rec = Expr::makeIdent(VarName::makeVarWithoutSuffix("r"), recType);
  auto val = Expr::makeInteger("42");
  auto e = Expr::makeRecordFieldUpdate(std::move(rec), "a", std::move(val),
                                       recType);
  EXPECT_EQ(e.getTag(), Expr::EKind::Record_Field_Update);
}

// --- Copy ---

TEST(Expr, CopyPreservesTag) {
  auto e = Expr::makeInteger("42");
  auto c = e.copy();
  EXPECT_EQ(c.getTag(), Expr::EKind::IntegerLiteral);
  EXPECT_EQ(c.getIntegerLiteral(), "42");
}

TEST(Expr, CopyIndependent) {
  auto x = VarName::makeVarWithoutSuffix("x");
  auto e = Expr::makeIdent(x, BType::INT);
  auto c = e.copy();
  EXPECT_EQ(Expr::compare(e, c), 0);
}

// --- Comparison ---

TEST(Expr, CompareEqual) {
  auto e1 = Expr::makeInteger("42");
  auto e2 = Expr::makeInteger("42");
  EXPECT_EQ(Expr::compare(e1, e2), 0);
}

TEST(Expr, CompareDifferentKind) {
  auto e1 = Expr::makeInteger("42");
  auto e2 = Expr::makeString("hello");
  EXPECT_NE(Expr::compare(e1, e2), 0);
}

TEST(Expr, CompareBinary) {
  auto e1 = Expr::makeBinaryExpr(Expr::BinaryOp::IAddition,
                                 Expr::makeInteger("1"), Expr::makeInteger("2"),
                                 BType::INT);
  auto e2 = Expr::makeBinaryExpr(Expr::BinaryOp::IAddition,
                                 Expr::makeInteger("1"), Expr::makeInteger("2"),
                                 BType::INT);
  EXPECT_EQ(Expr::compare(e1, e2), 0);
}

TEST(Expr, CompareBinaryDifferentOp) {
  auto e1 = Expr::makeBinaryExpr(Expr::BinaryOp::IAddition,
                                 Expr::makeInteger("1"), Expr::makeInteger("2"),
                                 BType::INT);
  auto e2 = Expr::makeBinaryExpr(Expr::BinaryOp::ISubtraction,
                                 Expr::makeInteger("1"), Expr::makeInteger("2"),
                                 BType::INT);
  EXPECT_NE(Expr::compare(e1, e2), 0);
}

// --- Alpha equality ---

TEST(Expr, AlphaEqualsIdentical) {
  auto e1 = Expr::makeInteger("42");
  auto e2 = Expr::makeInteger("42");
  EXPECT_TRUE(Expr::alpha_equals(e1, e2));
}

TEST(Expr, AlphaEqualsDifferent) {
  auto e1 = Expr::makeInteger("1");
  auto e2 = Expr::makeInteger("2");
  EXPECT_FALSE(Expr::alpha_equals(e1, e2));
}

TEST(Expr, AlphaEqualsQuantified) {
  // lambda x . x  vs  lambda x$1 . x$1  (same prefix, different suffix)
  auto x = VarName::makeVarWithoutSuffix("x");
  auto x1 = VarName::makeVar("x", 1);
  std::vector<TypedVar> vars1 = {TypedVar(x, BType::INT)};
  std::vector<TypedVar> vars2 = {TypedVar(x1, BType::INT)};
  auto lambdaType = BType::POW(BType::PROD(BType::INT, BType::INT));

  auto e1 = Expr::makeQuantifiedExpr(Expr::QuantifiedOp::Lambda, vars1,
                                     Pred::makeTrue(),
                                     Expr::makeIdent(x, BType::INT), lambdaType);
  auto e2 = Expr::makeQuantifiedExpr(Expr::QuantifiedOp::Lambda, vars2,
                                     Pred::makeTrue(),
                                     Expr::makeIdent(x1, BType::INT), lambdaType);
  EXPECT_TRUE(Expr::alpha_equals(e1, e2));
}

TEST(Expr, AlphaNotEqualsDifferentPrefix) {
  // lambda x . x  vs  lambda y . y  (different prefix -> not alpha-equal)
  auto x = VarName::makeVarWithoutSuffix("x");
  auto y = VarName::makeVarWithoutSuffix("y");
  std::vector<TypedVar> vars1 = {TypedVar(x, BType::INT)};
  std::vector<TypedVar> vars2 = {TypedVar(y, BType::INT)};
  auto lambdaType = BType::POW(BType::PROD(BType::INT, BType::INT));

  auto e1 = Expr::makeQuantifiedExpr(Expr::QuantifiedOp::Lambda, vars1,
                                     Pred::makeTrue(),
                                     Expr::makeIdent(x, BType::INT), lambdaType);
  auto e2 = Expr::makeQuantifiedExpr(Expr::QuantifiedOp::Lambda, vars2,
                                     Pred::makeTrue(),
                                     Expr::makeIdent(y, BType::INT), lambdaType);
  EXPECT_FALSE(Expr::alpha_equals(e1, e2));
}

// --- Hashing ---

TEST(Expr, HashConsistency) {
  auto e1 = Expr::makeInteger("42");
  auto e2 = Expr::makeInteger("42");
  EXPECT_EQ(e1.hash_combine(0), e2.hash_combine(0));
}

TEST(Expr, HashEqualImpliesSameHash) {
  auto e1 = Expr::makeBinaryExpr(Expr::BinaryOp::IAddition,
                                 Expr::makeInteger("1"), Expr::makeInteger("2"),
                                 BType::INT);
  auto e2 = Expr::makeBinaryExpr(Expr::BinaryOp::IAddition,
                                 Expr::makeInteger("1"), Expr::makeInteger("2"),
                                 BType::INT);
  EXPECT_EQ(Expr::compare(e1, e2), 0);
  EXPECT_EQ(e1.hash_combine(0), e2.hash_combine(0));
}

// --- Free variables ---

TEST(Expr, FreeVarsIdent) {
  auto x = VarName::makeVarWithoutSuffix("x");
  auto e = Expr::makeIdent(x, BType::INT);
  auto fv = e.getFreeVars();
  EXPECT_EQ(fv.size(), 1u);
  EXPECT_TRUE(fv.count(x));
}

TEST(Expr, FreeVarsConstant) {
  auto e = Expr::makeInteger("42");
  auto fv = e.getFreeVars();
  EXPECT_TRUE(fv.empty());
}

TEST(Expr, FreeVarsBinary) {
  auto x = VarName::makeVarWithoutSuffix("x");
  auto y = VarName::makeVarWithoutSuffix("y");
  auto e = Expr::makeBinaryExpr(Expr::BinaryOp::IAddition,
                                Expr::makeIdent(x, BType::INT),
                                Expr::makeIdent(y, BType::INT), BType::INT);
  auto fv = e.getFreeVars();
  EXPECT_EQ(fv.size(), 2u);
  EXPECT_TRUE(fv.count(x));
  EXPECT_TRUE(fv.count(y));
}

TEST(Expr, FreeVarsQuantifiedBound) {
  auto x = VarName::makeVarWithoutSuffix("x");
  std::vector<TypedVar> vars = {TypedVar(x, BType::INT)};
  auto lambdaType = BType::POW(BType::PROD(BType::INT, BType::INT));
  auto e = Expr::makeQuantifiedExpr(Expr::QuantifiedOp::Lambda, vars,
                                    Pred::makeTrue(),
                                    Expr::makeIdent(x, BType::INT), lambdaType);
  auto fv = e.getFreeVars();
  EXPECT_TRUE(fv.empty());
}

TEST(Expr, FreeVarsQuantifiedMixed) {
  auto x = VarName::makeVarWithoutSuffix("x");
  auto y = VarName::makeVarWithoutSuffix("y");
  std::vector<TypedVar> vars = {TypedVar(x, BType::INT)};
  auto lambdaType = BType::POW(BType::PROD(BType::INT, BType::INT));
  // lambda x . x + y  =>  y is free
  auto body = Expr::makeBinaryExpr(Expr::BinaryOp::IAddition,
                                   Expr::makeIdent(x, BType::INT),
                                   Expr::makeIdent(y, BType::INT), BType::INT);
  auto e = Expr::makeQuantifiedExpr(Expr::QuantifiedOp::Lambda, vars,
                                    Pred::makeTrue(), std::move(body),
                                    lambdaType);
  auto fv = e.getFreeVars();
  EXPECT_EQ(fv.size(), 1u);
  EXPECT_TRUE(fv.count(y));
  EXPECT_FALSE(fv.count(x));
}

// --- getAllVars ---

TEST(Expr, GetAllVarsQuantified) {
  auto x = VarName::makeVarWithoutSuffix("x");
  auto y = VarName::makeVarWithoutSuffix("y");
  std::vector<TypedVar> vars = {TypedVar(x, BType::INT)};
  auto lambdaType = BType::POW(BType::PROD(BType::INT, BType::INT));
  auto body = Expr::makeBinaryExpr(Expr::BinaryOp::IAddition,
                                   Expr::makeIdent(x, BType::INT),
                                   Expr::makeIdent(y, BType::INT), BType::INT);
  auto e = Expr::makeQuantifiedExpr(Expr::QuantifiedOp::Lambda, vars,
                                    Pred::makeTrue(), std::move(body),
                                    lambdaType);
  auto av = e.getAllVars();
  EXPECT_EQ(av.size(), 2u);
  EXPECT_TRUE(av.count(x));
  EXPECT_TRUE(av.count(y));
}

// --- Substitution ---

TEST(Expr, SubstIdent) {
  auto x = VarName::makeVarWithoutSuffix("x");
  auto e = Expr::makeIdent(x, BType::INT);
  std::map<VarName, Expr> map;
  map.insert({x, Expr::makeInteger("42")});
  e.subst(map);
  EXPECT_EQ(e.getTag(), Expr::EKind::IntegerLiteral);
  EXPECT_EQ(e.getIntegerLiteral(), "42");
}

TEST(Expr, SubstNoMatch) {
  auto x = VarName::makeVarWithoutSuffix("x");
  auto y = VarName::makeVarWithoutSuffix("y");
  auto e = Expr::makeIdent(x, BType::INT);
  std::map<VarName, Expr> map;
  map.insert({y, Expr::makeInteger("42")});
  e.subst(map);
  EXPECT_EQ(e.getTag(), Expr::EKind::Id);
  EXPECT_EQ(e.getId(), x);
}

TEST(Expr, SubstBinary) {
  auto x = VarName::makeVarWithoutSuffix("x");
  auto e = Expr::makeBinaryExpr(Expr::BinaryOp::IAddition,
                                Expr::makeIdent(x, BType::INT),
                                Expr::makeInteger("1"), BType::INT);
  std::map<VarName, Expr> map;
  map.insert({x, Expr::makeInteger("5")});
  e.subst(map);
  // After substitution, lhs should be 5
  auto &bin = e.toBinaryExpr();
  EXPECT_EQ(bin.lhs.getTag(), Expr::EKind::IntegerLiteral);
  EXPECT_EQ(bin.lhs.getIntegerLiteral(), "5");
}

// --- Alpha renaming ---

TEST(Expr, AlphaRename) {
  auto x = VarName::makeVarWithoutSuffix("x");
  auto y = VarName::makeVarWithoutSuffix("y");
  auto e = Expr::makeIdent(x, BType::INT);
  std::map<VarName, VarName> map;
  map.insert({x, y});
  e.alpha(map);
  EXPECT_EQ(e.getId(), y);
}

// --- BxmlTag ---

TEST(Expr, BxmlTag) {
  auto e = Expr::makeInteger("1", {"tag1", "tag2"});
  auto &tags = e.getBxmlTag();
  ASSERT_EQ(tags.size(), 2u);
  EXPECT_EQ(tags[0], "tag1");
  EXPECT_EQ(tags[1], "tag2");
}

// --- Default constructor ---

TEST(Expr, DefaultConstructor) {
  Expr e;
  EXPECT_EQ(e.getTag(), Expr::EKind::MaxInt);
}

// --- Vec compare ---

TEST(Expr, VecCompareEqual) {
  std::vector<Expr> v1;
  v1.push_back(Expr::makeInteger("1"));
  v1.push_back(Expr::makeInteger("2"));
  std::vector<Expr> v2;
  v2.push_back(Expr::makeInteger("1"));
  v2.push_back(Expr::makeInteger("2"));
  EXPECT_EQ(Expr::vec_compare(v1, v2), 0);
}

TEST(Expr, VecCompareDifferentSize) {
  std::vector<Expr> v1;
  v1.push_back(Expr::makeInteger("1"));
  std::vector<Expr> v2;
  v2.push_back(Expr::makeInteger("1"));
  v2.push_back(Expr::makeInteger("2"));
  EXPECT_LT(Expr::vec_compare(v1, v2), 0);
}

// --- getFreeTVars ---

TEST(Expr, GetFreeTVars) {
  auto x = VarName::makeVarWithoutSuffix("x");
  auto e = Expr::makeIdent(x, BType::INT);
  auto ftv = e.getFreeTVars();
  EXPECT_EQ(ftv.size(), 1u);
  auto it = ftv.begin();
  EXPECT_EQ(it->name, x);
  EXPECT_EQ(it->type, BType::INT);
}
