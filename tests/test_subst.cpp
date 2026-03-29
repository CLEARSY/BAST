#include "subst.h"

#include <gtest/gtest.h>

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static TypedVar tv(const std::string &name, BType type) {
  return TypedVar{VarName::makeVarWithoutSuffix(name), std::move(type)};
}

static Expr ident(const std::string &name, BType type) {
  return Expr::makeIdent(VarName::makeVarWithoutSuffix(name), std::move(type));
}

// Build a simple assignment: vars := exprs
static Subst simpleAssign(const std::string &var, BType ty,
                           const std::string &rhs) {
  std::vector<TypedVar> vars;
  vars.push_back(tv(var, ty));
  std::vector<Expr> exprs;
  exprs.push_back(ident(rhs, ty));
  return Subst::makeSimpleAssignment(vars, std::move(exprs));
}

// ===========================================================================
// Default constructor
// ===========================================================================

TEST(SubstDefault, DefaultIsSkip) {
  Subst s;
  EXPECT_EQ(s.getTag(), Subst::SKind::Skip);
}

// ===========================================================================
// Construction & getTag
// ===========================================================================

TEST(SubstConstruction, Skip) {
  auto s = Subst::makeSkip();
  EXPECT_EQ(s.getTag(), Subst::SKind::Skip);
}

TEST(SubstConstruction, Block) {
  auto s = Subst::makeBlock(Subst::makeSkip());
  EXPECT_EQ(s.getTag(), Subst::SKind::Block);
  EXPECT_EQ(s.toBlock().getTag(), Subst::SKind::Skip);
}

TEST(SubstConstruction, Assert) {
  auto s = Subst::makeAssert(Pred::makeTrue(), Subst::makeSkip());
  EXPECT_EQ(s.getTag(), Subst::SKind::Assert);
  EXPECT_EQ(s.toAssert().content.getTag(), Subst::SKind::Skip);
}

TEST(SubstConstruction, IfThen) {
  auto s = Subst::makeIfThen(Pred::makeTrue(), Subst::makeSkip());
  EXPECT_EQ(s.getTag(), Subst::SKind::IfThen);
  EXPECT_EQ(s.toIfThen().s_if.getTag(), Subst::SKind::Skip);
}

TEST(SubstConstruction, IfThenElse) {
  auto s = Subst::makeIfThenElse(Pred::makeTrue(), Subst::makeSkip(),
                                  Subst::makeSkip());
  EXPECT_EQ(s.getTag(), Subst::SKind::IfThenElse);
  EXPECT_EQ(s.toIfThenElse().s_if.getTag(), Subst::SKind::Skip);
  EXPECT_EQ(s.toIfThenElse().s_else.getTag(), Subst::SKind::Skip);
}

TEST(SubstConstruction, SimpleAssignment) {
  std::vector<TypedVar> vars;
  vars.push_back(tv("x", BType::INT));
  std::vector<Expr> exprs;
  exprs.push_back(Expr::makeInteger("1"));
  auto s = Subst::makeSimpleAssignment(vars, std::move(exprs));
  EXPECT_EQ(s.getTag(), Subst::SKind::SimpleAssignment);
  EXPECT_EQ(s.toSimpleAssignment().vars.size(), 1u);
  EXPECT_EQ(s.toSimpleAssignment().exprs.size(), 1u);
}

TEST(SubstConstruction, Select) {
  std::vector<std::pair<Pred, Subst>> clauses;
  clauses.push_back({Pred::makeTrue(), Subst::makeSkip()});
  auto s = Subst::makeSelect(std::move(clauses));
  EXPECT_EQ(s.getTag(), Subst::SKind::Select);
  EXPECT_EQ(s.toSelect().clauses.size(), 1u);
}

TEST(SubstConstruction, SelectElse) {
  std::vector<std::pair<Pred, Subst>> clauses;
  clauses.push_back({Pred::makeTrue(), Subst::makeSkip()});
  auto s = Subst::makeSelectElse(std::move(clauses), Subst::makeSkip());
  EXPECT_EQ(s.getTag(), Subst::SKind::SelectElse);
  EXPECT_EQ(s.toSelectElse().clauses.size(), 1u);
  EXPECT_EQ(s.toSelectElse().s_else.getTag(), Subst::SKind::Skip);
}

TEST(SubstConstruction, Case) {
  std::vector<Subst::CaseChoice> cases;
  Subst::CaseChoice ch;
  ch.values.push_back(Expr::makeInteger("1"));
  ch.body = Subst::makeSkip();
  cases.push_back(std::move(ch));
  auto s = Subst::makeCase(ident("x", BType::INT), std::move(cases));
  EXPECT_EQ(s.getTag(), Subst::SKind::Case);
  EXPECT_EQ(s.toCase().m_cases.size(), 1u);
}

TEST(SubstConstruction, CaseElse) {
  std::vector<Subst::CaseChoice> cases;
  Subst::CaseChoice ch;
  ch.values.push_back(Expr::makeInteger("1"));
  ch.body = Subst::makeSkip();
  cases.push_back(std::move(ch));
  auto s = Subst::makeCaseElse(ident("x", BType::INT), std::move(cases),
                                Subst::makeSkip());
  EXPECT_EQ(s.getTag(), Subst::SKind::CaseElse);
  EXPECT_EQ(s.toCaseElse().m_cases.size(), 1u);
  EXPECT_EQ(s.toCaseElse().m_else.getTag(), Subst::SKind::Skip);
}

TEST(SubstConstruction, Any) {
  std::vector<TypedVar> vars;
  vars.push_back(tv("x", BType::INT));
  auto s = Subst::makeAny(vars, Pred::makeTrue(), Subst::makeSkip());
  EXPECT_EQ(s.getTag(), Subst::SKind::Any);
  EXPECT_EQ(s.toAny().vars.size(), 1u);
}

TEST(SubstConstruction, OpCall) {
  std::vector<Expr> input;
  input.push_back(Expr::makeInteger("1"));
  std::vector<TypedVar> output;
  output.push_back(tv("r", BType::INT));
  std::vector<TypedVar> op_input;
  op_input.push_back(tv("p", BType::INT));
  std::vector<TypedVar> op_output;
  op_output.push_back(tv("q", BType::INT));
  auto s = Subst::makeOpCall("myOp", std::move(input), output, op_input,
                              op_output, Pred::makeTrue(), Subst::makeSkip());
  EXPECT_EQ(s.getTag(), Subst::SKind::OperationCall);
  EXPECT_EQ(s.toOpCall().name, "myOp");
  EXPECT_EQ(s.toOpCall().input.size(), 1u);
  EXPECT_EQ(s.toOpCall().output.size(), 1u);
}

TEST(SubstConstruction, While) {
  auto s = Subst::makeWhile(Pred::makeTrue(), Subst::makeSkip(),
                             Pred::makeTrue(), Expr::makeInteger("0"));
  EXPECT_EQ(s.getTag(), Subst::SKind::While);
}

TEST(SubstConstruction, Witness) {
  std::map<std::string, Expr> witnesses;
  witnesses.emplace("w", Expr::makeInteger("42"));
  auto s = Subst::makeWitness(std::move(witnesses), Subst::makeSkip());
  EXPECT_EQ(s.getTag(), Subst::SKind::Witness);
  EXPECT_EQ(s.toWitness().witnesses.size(), 1u);
}

TEST(SubstConstruction, Sequence) {
  std::vector<Subst> vec;
  vec.push_back(Subst::makeSkip());
  vec.push_back(Subst::makeSkip());
  auto s = Subst::makeSequence(std::move(vec));
  EXPECT_EQ(s.getTag(), Subst::SKind::Sequence);
  EXPECT_EQ(s.toSequence().size(), 2u);
}

TEST(SubstConstruction, Parallel) {
  std::vector<Subst> vec;
  vec.push_back(Subst::makeSkip());
  auto s = Subst::makeParallel(std::move(vec));
  EXPECT_EQ(s.getTag(), Subst::SKind::Parallel);
  EXPECT_EQ(s.toParallel().size(), 1u);
}

TEST(SubstConstruction, Choice) {
  std::vector<Subst> vec;
  vec.push_back(Subst::makeSkip());
  vec.push_back(Subst::makeSkip());
  vec.push_back(Subst::makeSkip());
  auto s = Subst::makeChoice(std::move(vec));
  EXPECT_EQ(s.getTag(), Subst::SKind::Choice);
  EXPECT_EQ(s.toChoice().size(), 3u);
}

// ===========================================================================
// Copy
// ===========================================================================

TEST(SubstCopy, SkipCopy) {
  auto s = Subst::makeSkip();
  auto c = s.copy();
  EXPECT_EQ(c.getTag(), Subst::SKind::Skip);
}

TEST(SubstCopy, AssignmentCopy) {
  auto s = simpleAssign("x", BType::INT, "y");
  auto c = s.copy();
  EXPECT_EQ(c.getTag(), Subst::SKind::SimpleAssignment);
  EXPECT_EQ(c.toSimpleAssignment().vars.size(), 1u);
  EXPECT_EQ(c.toSimpleAssignment().exprs.size(), 1u);
}

TEST(SubstCopy, NestedCopy) {
  auto s = Subst::makeIfThenElse(
      Pred::makeTrue(), simpleAssign("x", BType::INT, "y"),
      simpleAssign("x", BType::INT, "z"));
  auto c = s.copy();
  EXPECT_EQ(c.getTag(), Subst::SKind::IfThenElse);
  EXPECT_EQ(c.toIfThenElse().s_if.getTag(), Subst::SKind::SimpleAssignment);
  EXPECT_EQ(c.toIfThenElse().s_else.getTag(), Subst::SKind::SimpleAssignment);
}

// ===========================================================================
// getFreeVars
// ===========================================================================

TEST(SubstFreeVars, SkipEmpty) {
  auto fv = Subst::makeSkip().getFreeVars();
  EXPECT_TRUE(fv.empty());
}

TEST(SubstFreeVars, AssignmentFreeVars) {
  // x := y  — both x and y are free (no bound context)
  // But SimpleAssignmentSubst::getFreeVars adds LHS only if in boundVars,
  // so with empty boundVars, only RHS contributes.
  auto s = simpleAssign("x", BType::INT, "y");
  auto fv = s.getFreeVars();
  EXPECT_EQ(fv.count(VarName::makeVarWithoutSuffix("y")), 1u);
  // x is NOT free when called with empty boundVars
  EXPECT_EQ(fv.count(VarName::makeVarWithoutSuffix("x")), 0u);
}

TEST(SubstFreeVars, AssignmentLhsFreeWhenBound) {
  // When x is in boundVars, SimpleAssignment's getFreeVars adds it
  auto s = simpleAssign("x", BType::INT, "y");
  std::set<VarName> bound;
  bound.insert(VarName::makeVarWithoutSuffix("x"));
  std::set<VarName> accu;
  s.getFreeVars(bound, accu);
  EXPECT_EQ(accu.count(VarName::makeVarWithoutSuffix("x")), 1u);
  EXPECT_EQ(accu.count(VarName::makeVarWithoutSuffix("y")), 1u);
}

TEST(SubstFreeVars, IfThenUnion) {
  // IF p(a) THEN x := b END
  auto cond = Pred::makeExprComparison(Pred::ComparisonOp::Equality,
                                        ident("a", BType::INT),
                                        Expr::makeInteger("0"));
  auto body = simpleAssign("x", BType::INT, "b");
  auto s = Subst::makeIfThen(std::move(cond), std::move(body));
  auto fv = s.getFreeVars();
  EXPECT_EQ(fv.count(VarName::makeVarWithoutSuffix("a")), 1u);
  EXPECT_EQ(fv.count(VarName::makeVarWithoutSuffix("b")), 1u);
}

TEST(SubstFreeVars, AnyBindsVars) {
  // ANY x WHERE x=1 THEN skip END
  // x is bound, not free
  std::vector<TypedVar> vars;
  vars.push_back(tv("x", BType::INT));
  auto pred = Pred::makeExprComparison(Pred::ComparisonOp::Equality,
                                        ident("x", BType::INT),
                                        Expr::makeInteger("1"));
  auto s = Subst::makeAny(vars, std::move(pred), Subst::makeSkip());
  auto fv = s.getFreeVars();
  EXPECT_EQ(fv.count(VarName::makeVarWithoutSuffix("x")), 0u);
}

// ===========================================================================
// getAllVars
// ===========================================================================

TEST(SubstAllVars, SkipEmpty) {
  auto av = Subst::makeSkip().getAllVars();
  EXPECT_TRUE(av.empty());
}

TEST(SubstAllVars, Assignment) {
  auto s = simpleAssign("x", BType::INT, "y");
  auto av = s.getAllVars();
  EXPECT_EQ(av.count(VarName::makeVarWithoutSuffix("x")), 1u);
  EXPECT_EQ(av.count(VarName::makeVarWithoutSuffix("y")), 1u);
}

TEST(SubstAllVars, AnyIncludesBound) {
  std::vector<TypedVar> vars;
  vars.push_back(tv("x", BType::INT));
  auto pred = Pred::makeExprComparison(Pred::ComparisonOp::Equality,
                                        ident("x", BType::INT),
                                        ident("y", BType::INT));
  auto s = Subst::makeAny(vars, std::move(pred), Subst::makeSkip());
  auto av = s.getAllVars();
  EXPECT_EQ(av.count(VarName::makeVarWithoutSuffix("x")), 1u);
  EXPECT_EQ(av.count(VarName::makeVarWithoutSuffix("y")), 1u);
}

// ===========================================================================
// getModifiedVars
// ===========================================================================

TEST(SubstModifiedVars, SkipEmpty) {
  auto mv = Subst::makeSkip().getModifiedVars();
  EXPECT_TRUE(mv.empty());
}

TEST(SubstModifiedVars, Assignment) {
  auto s = simpleAssign("x", BType::INT, "y");
  auto mv = s.getModifiedVars();
  EXPECT_EQ(mv.size(), 1u);
  EXPECT_EQ(mv.begin()->name, VarName::makeVarWithoutSuffix("x"));
}

TEST(SubstModifiedVars, AnyExcludesBound) {
  // ANY x WHERE TRUE THEN x := 1 END
  // x is bound by ANY, so not modified from outer perspective
  std::vector<TypedVar> vars;
  vars.push_back(tv("x", BType::INT));
  auto body = simpleAssign("x", BType::INT, "x");
  auto s = Subst::makeAny(vars, Pred::makeTrue(), std::move(body));
  auto mv = s.getModifiedVars();
  EXPECT_TRUE(mv.empty());
}

// ===========================================================================
// Hashing
// ===========================================================================

TEST(SubstHash, Consistency) {
  auto s1 = Subst::makeSkip();
  auto s2 = Subst::makeSkip();
  EXPECT_EQ(s1.hash_combine(0), s2.hash_combine(0));
}

TEST(SubstHash, SkipDiffersFromBlock) {
  auto skip = Subst::makeSkip();
  auto block = Subst::makeBlock(Subst::makeSkip());
  EXPECT_NE(skip.hash_combine(0), block.hash_combine(0));
}

TEST(SubstHash, StdHash) {
  std::hash<Subst> h;
  auto s1 = Subst::makeSkip();
  auto s2 = Subst::makeSkip();
  EXPECT_EQ(h(s1), h(s2));
}

// ===========================================================================
// Visitor
// ===========================================================================

class TestVisitor : public Subst::Visitor {
 public:
  Subst::SKind visited = Subst::SKind::Skip;
  int count = 0;

  void visitSkip() override {
    visited = Subst::SKind::Skip;
    count++;
  }
  void visitBlock(const Subst &) override {
    visited = Subst::SKind::Block;
    count++;
  }
  void visitAssert(const Pred &, const Subst &) override {
    visited = Subst::SKind::Assert;
    count++;
  }
  void visitIfThen(const Pred &, const Subst &) override {
    visited = Subst::SKind::IfThen;
    count++;
  }
  void visitIfThenElse(const Pred &, const Subst &, const Subst &) override {
    visited = Subst::SKind::IfThenElse;
    count++;
  }
  void visitSimpleAssignment(const std::vector<TypedVar> &,
                              const std::vector<Expr> &) override {
    visited = Subst::SKind::SimpleAssignment;
    count++;
  }
  void visitSelect(
      const std::vector<std::pair<Pred, Subst>> &) override {
    visited = Subst::SKind::Select;
    count++;
  }
  void visitSelectElse(const std::vector<std::pair<Pred, Subst>> &,
                        const Subst &) override {
    visited = Subst::SKind::SelectElse;
    count++;
  }
  void visitCase(const Expr &,
                  const std::vector<Subst::CaseChoice> &) override {
    visited = Subst::SKind::Case;
    count++;
  }
  void visitCaseElse(const Expr &, const std::vector<Subst::CaseChoice> &,
                      const Subst &) override {
    visited = Subst::SKind::CaseElse;
    count++;
  }
  void visitAny(const std::vector<TypedVar> &, const Pred &,
                 const Subst &) override {
    visited = Subst::SKind::Any;
    count++;
  }
  void visitOpCall(const std::string &, const std::vector<Expr> &,
                    const std::vector<TypedVar> &,
                    const std::vector<TypedVar> &,
                    const std::vector<TypedVar> &, const Pred &,
                    const Subst &) override {
    visited = Subst::SKind::OperationCall;
    count++;
  }
  void visitWhile(const Pred &, const Subst &, const Pred &,
                   const Expr &) override {
    visited = Subst::SKind::While;
    count++;
  }
  void visitSequence(const std::vector<Subst> &) override {
    visited = Subst::SKind::Sequence;
    count++;
  }
  void visitParallel(const std::vector<Subst> &) override {
    visited = Subst::SKind::Parallel;
    count++;
  }
  void visitChoice(const std::vector<Subst> &) override {
    visited = Subst::SKind::Choice;
    count++;
  }
  void visitWitness(const std::map<std::string, Expr> &,
                     const Subst &) override {
    visited = Subst::SKind::Witness;
    count++;
  }
};

static void checkVisitor(Subst &s, Subst::SKind expected) {
  TestVisitor v;
  s.accept(v);
  EXPECT_EQ(v.visited, expected);
  EXPECT_EQ(v.count, 1);
}

TEST(SubstVisitor, AllKinds) {
  auto skip = Subst::makeSkip();
  checkVisitor(skip, Subst::SKind::Skip);

  auto block = Subst::makeBlock(Subst::makeSkip());
  checkVisitor(block, Subst::SKind::Block);

  auto assert_ = Subst::makeAssert(Pred::makeTrue(), Subst::makeSkip());
  checkVisitor(assert_, Subst::SKind::Assert);

  auto ifThen = Subst::makeIfThen(Pred::makeTrue(), Subst::makeSkip());
  checkVisitor(ifThen, Subst::SKind::IfThen);

  auto ifThenElse = Subst::makeIfThenElse(Pred::makeTrue(), Subst::makeSkip(),
                                            Subst::makeSkip());
  checkVisitor(ifThenElse, Subst::SKind::IfThenElse);

  auto assign = simpleAssign("x", BType::INT, "y");
  checkVisitor(assign, Subst::SKind::SimpleAssignment);

  {
    std::vector<std::pair<Pred, Subst>> clauses;
    clauses.push_back({Pred::makeTrue(), Subst::makeSkip()});
    auto sel = Subst::makeSelect(std::move(clauses));
    checkVisitor(sel, Subst::SKind::Select);
  }
  {
    std::vector<std::pair<Pred, Subst>> clauses;
    clauses.push_back({Pred::makeTrue(), Subst::makeSkip()});
    auto selE = Subst::makeSelectElse(std::move(clauses), Subst::makeSkip());
    checkVisitor(selE, Subst::SKind::SelectElse);
  }
  {
    std::vector<Subst::CaseChoice> cases;
    Subst::CaseChoice ch;
    ch.values.push_back(Expr::makeInteger("1"));
    ch.body = Subst::makeSkip();
    cases.push_back(std::move(ch));
    auto cas = Subst::makeCase(ident("x", BType::INT), std::move(cases));
    checkVisitor(cas, Subst::SKind::Case);
  }
  {
    std::vector<Subst::CaseChoice> cases;
    Subst::CaseChoice ch;
    ch.values.push_back(Expr::makeInteger("1"));
    ch.body = Subst::makeSkip();
    cases.push_back(std::move(ch));
    auto casE = Subst::makeCaseElse(ident("x", BType::INT),
                                     std::move(cases), Subst::makeSkip());
    checkVisitor(casE, Subst::SKind::CaseElse);
  }
  {
    std::vector<TypedVar> vars;
    vars.push_back(tv("x", BType::INT));
    auto any = Subst::makeAny(vars, Pred::makeTrue(), Subst::makeSkip());
    checkVisitor(any, Subst::SKind::Any);
  }
  {
    std::vector<Expr> input;
    input.push_back(Expr::makeInteger("1"));
    std::vector<TypedVar> output, op_in, op_out;
    output.push_back(tv("r", BType::INT));
    op_in.push_back(tv("p", BType::INT));
    op_out.push_back(tv("q", BType::INT));
    auto op = Subst::makeOpCall("op", std::move(input), output, op_in, op_out,
                                 Pred::makeTrue(), Subst::makeSkip());
    checkVisitor(op, Subst::SKind::OperationCall);
  }
  {
    auto wh = Subst::makeWhile(Pred::makeTrue(), Subst::makeSkip(),
                                Pred::makeTrue(), Expr::makeInteger("0"));
    checkVisitor(wh, Subst::SKind::While);
  }
  {
    std::vector<Subst> vec;
    vec.push_back(Subst::makeSkip());
    auto seq = Subst::makeSequence(std::move(vec));
    checkVisitor(seq, Subst::SKind::Sequence);
  }
  {
    std::vector<Subst> vec;
    vec.push_back(Subst::makeSkip());
    auto par = Subst::makeParallel(std::move(vec));
    checkVisitor(par, Subst::SKind::Parallel);
  }
  {
    std::vector<Subst> vec;
    vec.push_back(Subst::makeSkip());
    auto cho = Subst::makeChoice(std::move(vec));
    checkVisitor(cho, Subst::SKind::Choice);
  }
  {
    std::map<std::string, Expr> witnesses;
    witnesses.emplace("w", Expr::makeInteger("1"));
    auto wit = Subst::makeWitness(std::move(witnesses), Subst::makeSkip());
    checkVisitor(wit, Subst::SKind::Witness);
  }
}

// ===========================================================================
// Alpha renaming
// ===========================================================================

TEST(SubstAlpha, AssignmentRenames) {
  // x := y, rename x->a, y->b
  auto s = simpleAssign("x", BType::INT, "y");
  std::map<VarName, VarName> map;
  map.emplace(VarName::makeVarWithoutSuffix("x"), VarName::makeVarWithoutSuffix("a"));
  map.emplace(VarName::makeVarWithoutSuffix("y"), VarName::makeVarWithoutSuffix("b"));
  s.alpha(map);
  auto &sa = s.toSimpleAssignment();
  EXPECT_EQ(sa.vars[0].name, VarName::makeVarWithoutSuffix("a"));
  // RHS expr should also be renamed
  auto fv = s.getAllVars();
  EXPECT_EQ(fv.count(VarName::makeVarWithoutSuffix("b")), 1u);
}

TEST(SubstAlpha, AnyShadowsBoundVars) {
  // ANY x WHERE x=y THEN skip END, rename x->a
  // Since x is bound by ANY, the rename should NOT apply to x
  std::vector<TypedVar> vars;
  vars.push_back(tv("x", BType::INT));
  auto pred = Pred::makeExprComparison(Pred::ComparisonOp::Equality,
                                        ident("x", BType::INT),
                                        ident("y", BType::INT));
  auto s = Subst::makeAny(vars, std::move(pred), Subst::makeSkip());
  std::map<VarName, VarName> map;
  map.emplace(VarName::makeVarWithoutSuffix("x"), VarName::makeVarWithoutSuffix("a"));
  map.emplace(VarName::makeVarWithoutSuffix("y"), VarName::makeVarWithoutSuffix("b"));
  s.alpha(map);
  // x should remain x (shadowed), y should become b
  auto av = s.getAllVars();
  EXPECT_EQ(av.count(VarName::makeVarWithoutSuffix("x")), 1u);
  EXPECT_EQ(av.count(VarName::makeVarWithoutSuffix("b")), 1u);
  EXPECT_EQ(av.count(VarName::makeVarWithoutSuffix("a")), 0u);
}

TEST(SubstAlpha, NestedPropagates) {
  // IF TRUE THEN x := y END, rename y->z
  auto body = simpleAssign("x", BType::INT, "y");
  auto s = Subst::makeIfThen(Pred::makeTrue(), std::move(body));
  std::map<VarName, VarName> map;
  map.emplace(VarName::makeVarWithoutSuffix("y"), VarName::makeVarWithoutSuffix("z"));
  s.alpha(map);
  auto av = s.getAllVars();
  EXPECT_EQ(av.count(VarName::makeVarWithoutSuffix("z")), 1u);
  EXPECT_EQ(av.count(VarName::makeVarWithoutSuffix("y")), 0u);
}

// ===========================================================================
// substFreshId
// ===========================================================================

TEST(SubstSubstFreshId, AssignmentFreshId) {
  // Assignment with FreshId variable
  std::vector<TypedVar> vars;
  vars.push_back(TypedVar{VarName::makeFreshId("tmp"), BType::INT});
  std::vector<Expr> exprs;
  exprs.push_back(Expr::makeInteger("1"));
  auto s = Subst::makeSimpleAssignment(vars, std::move(exprs));
  s.substFreshId("tmp", VarName::makeVarWithoutSuffix("real_var"));
  EXPECT_EQ(s.toSimpleAssignment().vars[0].name,
            VarName::makeVarWithoutSuffix("real_var"));
}

TEST(SubstSubstFreshId, AnyFreshIdInBoundVars) {
  std::vector<TypedVar> vars;
  vars.push_back(TypedVar{VarName::makeFreshId("tmp"), BType::INT});
  auto s = Subst::makeAny(vars, Pred::makeTrue(), Subst::makeSkip());
  s.substFreshId("tmp", VarName::makeVarWithoutSuffix("real_var"));
  EXPECT_EQ(s.toAny().vars[0].name, VarName::makeVarWithoutSuffix("real_var"));
}

// ===========================================================================
// getInnerFreeVars
// ===========================================================================

TEST(SubstInnerFreeVars, SkipEmpty) {
  std::set<VarName> accu;
  Subst::makeSkip().getInnerFreeVars(accu);
  EXPECT_TRUE(accu.empty());
}

TEST(SubstInnerFreeVars, AssignmentEmpty) {
  auto s = simpleAssign("x", BType::INT, "y");
  std::set<VarName> accu;
  s.getInnerFreeVars(accu);
  EXPECT_TRUE(accu.empty());
}

TEST(SubstInnerFreeVars, OpCallExposesInnerFreeVars) {
  // OpCall with op_body referencing a free var "z" (not in op_input/op_output)
  std::vector<Expr> input;
  input.push_back(Expr::makeInteger("1"));
  std::vector<TypedVar> output;
  output.push_back(tv("r", BType::INT));
  std::vector<TypedVar> op_input;
  op_input.push_back(tv("p", BType::INT));
  std::vector<TypedVar> op_output;
  op_output.push_back(tv("q", BType::INT));
  // op_body: q := z  (z is free in op body, not bound by op_input/op_output)
  auto op_body = simpleAssign("q", BType::INT, "z");
  auto s = Subst::makeOpCall("myOp", std::move(input), output, op_input,
                              op_output, Pred::makeTrue(), std::move(op_body));
  std::set<VarName> accu;
  s.getInnerFreeVars(accu);
  EXPECT_EQ(accu.count(VarName::makeVarWithoutSuffix("z")), 1u);
  // q appears because SimpleAssignment::getFreeVars adds LHS vars that ARE
  // in boundVars (referencing a bound op_output variable)
  EXPECT_EQ(accu.count(VarName::makeVarWithoutSuffix("q")), 1u);
  // p is not referenced in op_body
  EXPECT_EQ(accu.count(VarName::makeVarWithoutSuffix("p")), 0u);
}
