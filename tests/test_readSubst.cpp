#include <gtest/gtest.h>

#include "substReader.h"
#include "tinyxml2.h"

// typeInfos: [0]=INT, [1]=POW_INT
static const std::vector<BType> TI = {BType::INT, BType::POW_INT};

// ============================================================
// Skip / Block
// ============================================================

TEST(ReadSubst, Skip) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Skip/>");
  auto s = Xml::readSubstitution(doc.RootElement(), TI);
  EXPECT_EQ(s.getTag(), Subst::SKind::Skip);
}

TEST(ReadSubst, Block) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Bloc_Sub><Skip/></Bloc_Sub>");
  auto s = Xml::readSubstitution(doc.RootElement(), TI);
  EXPECT_EQ(s.getTag(), Subst::SKind::Block);
  EXPECT_EQ(s.toBlock().getTag(), Subst::SKind::Skip);
}

// ============================================================
// Assert / PRE
// ============================================================

TEST(ReadSubst, AssertSub) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Assert_Sub>
    <Guard>
      <Exp_Comparison op="=">
        <Id value="x" typref="0"/>
        <Integer_Literal value="0" typref="0"/>
      </Exp_Comparison>
    </Guard>
    <Body><Skip/></Body>
  </Assert_Sub>)");
  auto s = Xml::readSubstitution(doc.RootElement(), TI);
  EXPECT_EQ(s.getTag(), Subst::SKind::Assert);
  EXPECT_EQ(s.toAssert().condition.getTag(), Pred::PKind::ExprComparison);
  EXPECT_EQ(s.toAssert().content.getTag(), Subst::SKind::Skip);
}

TEST(ReadSubst, PreSub) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<PRE_Sub>
    <Precondition>
      <Exp_Comparison op="=">
        <Id value="x" typref="0"/>
        <Integer_Literal value="0" typref="0"/>
      </Exp_Comparison>
    </Precondition>
    <Body><Skip/></Body>
  </PRE_Sub>)");
  auto s = Xml::readSubstitution(doc.RootElement(), TI);
  EXPECT_EQ(s.getTag(), Subst::SKind::Assert);
}

// ============================================================
// IfThen / IfThenElse
// ============================================================

TEST(ReadSubst, IfThen) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<If_Sub>
    <Condition>
      <Exp_Comparison op="=">
        <Id value="x" typref="0"/>
        <Integer_Literal value="0" typref="0"/>
      </Exp_Comparison>
    </Condition>
    <Then><Skip/></Then>
  </If_Sub>)");
  auto s = Xml::readSubstitution(doc.RootElement(), TI);
  EXPECT_EQ(s.getTag(), Subst::SKind::IfThen);
}

TEST(ReadSubst, IfThenElse) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<If_Sub>
    <Condition>
      <Exp_Comparison op="=">
        <Id value="x" typref="0"/>
        <Integer_Literal value="0" typref="0"/>
      </Exp_Comparison>
    </Condition>
    <Then><Skip/></Then>
    <Else><Skip/></Else>
  </If_Sub>)");
  auto s = Xml::readSubstitution(doc.RootElement(), TI);
  EXPECT_EQ(s.getTag(), Subst::SKind::IfThenElse);
  EXPECT_EQ(s.toIfThenElse().s_if.getTag(), Subst::SKind::Skip);
  EXPECT_EQ(s.toIfThenElse().s_else.getTag(), Subst::SKind::Skip);
}

// ============================================================
// SimpleAssignment
// ============================================================

TEST(ReadSubst, SimpleAssignment) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Simple_Assignement_Sub>
    <Variables>
      <Id value="x" typref="0"/>
    </Variables>
    <Values>
      <Integer_Literal value="42" typref="0"/>
    </Values>
  </Simple_Assignement_Sub>)");
  auto s = Xml::readSubstitution(doc.RootElement(), TI);
  EXPECT_EQ(s.getTag(), Subst::SKind::SimpleAssignment);
  auto &sa = s.toSimpleAssignment();
  ASSERT_EQ(sa.vars.size(), 1u);
  EXPECT_EQ(sa.vars[0].name, VarName::makeVarWithoutSuffix("x"));
  ASSERT_EQ(sa.exprs.size(), 1u);
  EXPECT_EQ(sa.exprs[0].getTag(), Expr::EKind::IntegerLiteral);
}

TEST(ReadSubst, SimpleAssignmentMultiple) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Simple_Assignement_Sub>
    <Variables>
      <Id value="x" typref="0"/>
      <Id value="y" typref="0"/>
    </Variables>
    <Values>
      <Integer_Literal value="1" typref="0"/>
      <Integer_Literal value="2" typref="0"/>
    </Values>
  </Simple_Assignement_Sub>)");
  auto s = Xml::readSubstitution(doc.RootElement(), TI);
  EXPECT_EQ(s.toSimpleAssignment().vars.size(), 2u);
  EXPECT_EQ(s.toSimpleAssignment().exprs.size(), 2u);
}

// ============================================================
// Select / SelectElse
// ============================================================

TEST(ReadSubst, Select) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Select>
    <When_Clauses>
      <When>
        <Condition>
          <Exp_Comparison op="=">
            <Id value="x" typref="0"/>
            <Integer_Literal value="0" typref="0"/>
          </Exp_Comparison>
        </Condition>
        <Then><Skip/></Then>
      </When>
    </When_Clauses>
  </Select>)");
  auto s = Xml::readSubstitution(doc.RootElement(), TI);
  EXPECT_EQ(s.getTag(), Subst::SKind::Select);
  EXPECT_EQ(s.toSelect().clauses.size(), 1u);
}

TEST(ReadSubst, SelectElse) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Select>
    <When_Clauses>
      <When>
        <Condition>
          <Exp_Comparison op="=">
            <Id value="x" typref="0"/>
            <Integer_Literal value="0" typref="0"/>
          </Exp_Comparison>
        </Condition>
        <Then><Skip/></Then>
      </When>
    </When_Clauses>
    <Else><Skip/></Else>
  </Select>)");
  auto s = Xml::readSubstitution(doc.RootElement(), TI);
  EXPECT_EQ(s.getTag(), Subst::SKind::SelectElse);
}

// ============================================================
// Case / CaseElse
// ============================================================

TEST(ReadSubst, Case) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Case_Sub>
    <Value><Id value="x" typref="0"/></Value>
    <Choices>
      <Choice>
        <Value><Integer_Literal value="1" typref="0"/></Value>
        <Then><Skip/></Then>
      </Choice>
      <Choice>
        <Value><Integer_Literal value="2" typref="0"/></Value>
        <Then><Skip/></Then>
      </Choice>
    </Choices>
  </Case_Sub>)");
  auto s = Xml::readSubstitution(doc.RootElement(), TI);
  EXPECT_EQ(s.getTag(), Subst::SKind::Case);
  EXPECT_EQ(s.toCase().m_cases.size(), 2u);
}

TEST(ReadSubst, CaseElse) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Case_Sub>
    <Value><Id value="x" typref="0"/></Value>
    <Choices>
      <Choice>
        <Value><Integer_Literal value="1" typref="0"/></Value>
        <Then><Skip/></Then>
      </Choice>
    </Choices>
    <Else><Skip/></Else>
  </Case_Sub>)");
  auto s = Xml::readSubstitution(doc.RootElement(), TI);
  EXPECT_EQ(s.getTag(), Subst::SKind::CaseElse);
}

// ============================================================
// Any
// ============================================================

TEST(ReadSubst, Any) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<ANY_Sub>
    <Variables>
      <Id value="x" typref="0"/>
    </Variables>
    <Pred>
      <Exp_Comparison op=":">
        <Id value="x" typref="0"/>
        <Id value="INTEGER" typref="1"/>
      </Exp_Comparison>
    </Pred>
    <Then><Skip/></Then>
  </ANY_Sub>)");
  auto s = Xml::readSubstitution(doc.RootElement(), TI);
  EXPECT_EQ(s.getTag(), Subst::SKind::Any);
  EXPECT_EQ(s.toAny().vars.size(), 1u);
  EXPECT_EQ(s.toAny().body.getTag(), Subst::SKind::Skip);
}

// ============================================================
// While
// ============================================================

TEST(ReadSubst, While) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<While>
    <Condition>
      <Exp_Comparison op="&gt;i">
        <Id value="x" typref="0"/>
        <Integer_Literal value="0" typref="0"/>
      </Exp_Comparison>
    </Condition>
    <Body>
      <Simple_Assignement_Sub>
        <Variables><Id value="x" typref="0"/></Variables>
        <Values>
          <Binary_Exp op="-i" typref="0">
            <Id value="x" typref="0"/>
            <Integer_Literal value="1" typref="0"/>
          </Binary_Exp>
        </Values>
      </Simple_Assignement_Sub>
    </Body>
    <Invariant>
      <Exp_Comparison op="&gt;=i">
        <Id value="x" typref="0"/>
        <Integer_Literal value="0" typref="0"/>
      </Exp_Comparison>
    </Invariant>
    <Variant>
      <Id value="x" typref="0"/>
    </Variant>
  </While>)");
  auto s = Xml::readSubstitution(doc.RootElement(), TI);
  EXPECT_EQ(s.getTag(), Subst::SKind::While);
  auto &w = s.toWhile();
  EXPECT_EQ(w.body.getTag(), Subst::SKind::SimpleAssignment);
}

// ============================================================
// Nary_Sub: Sequence, Parallel, Choice
// ============================================================

TEST(ReadSubst, Sequence) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Nary_Sub op=";">
    <Skip/>
    <Skip/>
  </Nary_Sub>)");
  auto s = Xml::readSubstitution(doc.RootElement(), TI);
  EXPECT_EQ(s.getTag(), Subst::SKind::Sequence);
  EXPECT_EQ(s.toSequence().size(), 2u);
}

TEST(ReadSubst, Parallel) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Nary_Sub op="||">
    <Skip/>
    <Skip/>
    <Skip/>
  </Nary_Sub>)");
  auto s = Xml::readSubstitution(doc.RootElement(), TI);
  EXPECT_EQ(s.getTag(), Subst::SKind::Parallel);
  EXPECT_EQ(s.toParallel().size(), 3u);
}

TEST(ReadSubst, Choice) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Nary_Sub op="CHOICE">
    <Skip/>
    <Skip/>
  </Nary_Sub>)");
  auto s = Xml::readSubstitution(doc.RootElement(), TI);
  EXPECT_EQ(s.getTag(), Subst::SKind::Choice);
  EXPECT_EQ(s.toChoice().size(), 2u);
}

// ============================================================
// Witness
// ============================================================

TEST(ReadSubst, WitnessSingle) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Witness>
    <Witnesses>
      <Exp_Comparison op="=">
        <Id value="w" typref="0"/>
        <Integer_Literal value="42" typref="0"/>
      </Exp_Comparison>
    </Witnesses>
    <Body><Skip/></Body>
  </Witness>)");
  auto s = Xml::readSubstitution(doc.RootElement(), TI);
  EXPECT_EQ(s.getTag(), Subst::SKind::Witness);
  auto &wt = s.toWitness();
  EXPECT_EQ(wt.witnesses.size(), 1u);
  EXPECT_TRUE(wt.witnesses.count("w"));
  EXPECT_EQ(wt.body.getTag(), Subst::SKind::Skip);
}

TEST(ReadSubst, WitnessMultiple) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Witness>
    <Witnesses>
      <Nary_Pred op="&amp;">
        <Exp_Comparison op="=">
          <Id value="a" typref="0"/>
          <Integer_Literal value="1" typref="0"/>
        </Exp_Comparison>
        <Exp_Comparison op="=">
          <Id value="b" typref="0"/>
          <Integer_Literal value="2" typref="0"/>
        </Exp_Comparison>
      </Nary_Pred>
    </Witnesses>
    <Body><Skip/></Body>
  </Witness>)");
  auto s = Xml::readSubstitution(doc.RootElement(), TI);
  EXPECT_EQ(s.getTag(), Subst::SKind::Witness);
  EXPECT_EQ(s.toWitness().witnesses.size(), 2u);
  EXPECT_TRUE(s.toWitness().witnesses.count("a"));
  EXPECT_TRUE(s.toWitness().witnesses.count("b"));
}

// ============================================================
// OperationCall
// ============================================================

TEST(ReadSubst, OperationCallNoParams) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Operation_Call>
    <Name><Id value="op1" typref="0"/></Name>
    <Operation name="op1">
      <Body><Skip/></Body>
    </Operation>
  </Operation_Call>)");
  auto s = Xml::readSubstitution(doc.RootElement(), TI);
  EXPECT_EQ(s.getTag(), Subst::SKind::OperationCall);
  EXPECT_EQ(s.toOpCall().name, "op1");
}

TEST(ReadSubst, OperationCallWithParams) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Operation_Call>
    <Name><Id value="op2" typref="0"/></Name>
    <Input_Parameters>
      <Integer_Literal value="5" typref="0"/>
    </Input_Parameters>
    <Output_Parameters>
      <Id value="r" typref="0"/>
    </Output_Parameters>
    <Operation name="op2">
      <Input_Parameters>
        <Id value="p" typref="0"/>
      </Input_Parameters>
      <Output_Parameters>
        <Id value="q" typref="0"/>
      </Output_Parameters>
      <Body><Skip/></Body>
    </Operation>
  </Operation_Call>)");
  auto s = Xml::readSubstitution(doc.RootElement(), TI);
  EXPECT_EQ(s.getTag(), Subst::SKind::OperationCall);
  auto &oc = s.toOpCall();
  EXPECT_EQ(oc.name, "op2");
  EXPECT_EQ(oc.input.size(), 1u);
  EXPECT_EQ(oc.output.size(), 1u);
  EXPECT_EQ(oc.op_input.size(), 1u);
  EXPECT_EQ(oc.op_output.size(), 1u);
}

TEST(ReadSubst, OperationCallWithPrecondition) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Operation_Call>
    <Name><Id value="op3" typref="0"/></Name>
    <Operation name="op3">
      <Precondition>
        <Exp_Comparison op="=">
          <Id value="x" typref="0"/>
          <Integer_Literal value="0" typref="0"/>
        </Exp_Comparison>
      </Precondition>
      <Body><Skip/></Body>
    </Operation>
  </Operation_Call>)");
  auto s = Xml::readSubstitution(doc.RootElement(), TI);
  EXPECT_EQ(s.toOpCall().op_precondition.getTag(), Pred::PKind::ExprComparison);
}

// ============================================================
// Error cases
// ============================================================

TEST(ReadSubst, NullDomThrows) {
  EXPECT_THROW(Xml::readSubstitution(nullptr, TI),
               Xml::SubstReaderException);
}

TEST(ReadSubst, UnknownTagThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Bogus/>");
  EXPECT_THROW(Xml::readSubstitution(doc.RootElement(), TI),
               Xml::SubstReaderException);
}

TEST(ReadSubst, UnknownNaryOpThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Nary_Sub op="???"><Skip/></Nary_Sub>)");
  EXPECT_THROW(Xml::readSubstitution(doc.RootElement(), TI),
               Xml::SubstReaderException);
}

TEST(ReadSubst, AssertMissingGuardThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Assert_Sub><Body><Skip/></Body></Assert_Sub>)");
  EXPECT_THROW(Xml::readSubstitution(doc.RootElement(), TI),
               Xml::SubstReaderException);
}

TEST(ReadSubst, AssertMissingBodyThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Assert_Sub>
    <Guard>
      <Exp_Comparison op="=">
        <Id value="x" typref="0"/>
        <Integer_Literal value="0" typref="0"/>
      </Exp_Comparison>
    </Guard>
  </Assert_Sub>)");
  EXPECT_THROW(Xml::readSubstitution(doc.RootElement(), TI),
               Xml::SubstReaderException);
}

TEST(ReadSubst, PreSubMissingPreconditionThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<PRE_Sub><Body><Skip/></Body></PRE_Sub>)");
  EXPECT_THROW(Xml::readSubstitution(doc.RootElement(), TI),
               Xml::SubstReaderException);
}

TEST(ReadSubst, IfSubMissingConditionThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<If_Sub><Then><Skip/></Then></If_Sub>)");
  EXPECT_THROW(Xml::readSubstitution(doc.RootElement(), TI),
               Xml::SubstReaderException);
}

TEST(ReadSubst, IfSubMissingThenThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<If_Sub>
    <Condition>
      <Exp_Comparison op="=">
        <Id value="x" typref="0"/>
        <Integer_Literal value="0" typref="0"/>
      </Exp_Comparison>
    </Condition>
  </If_Sub>)");
  EXPECT_THROW(Xml::readSubstitution(doc.RootElement(), TI),
               Xml::SubstReaderException);
}

TEST(ReadSubst, SimpleAssignmentMissingVariablesThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Simple_Assignement_Sub>
    <Values><Integer_Literal value="1" typref="0"/></Values>
  </Simple_Assignement_Sub>)");
  EXPECT_THROW(Xml::readSubstitution(doc.RootElement(), TI),
               Xml::SubstReaderException);
}

TEST(ReadSubst, SimpleAssignmentMissingValuesThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Simple_Assignement_Sub>
    <Variables><Id value="x" typref="0"/></Variables>
  </Simple_Assignement_Sub>)");
  EXPECT_THROW(Xml::readSubstitution(doc.RootElement(), TI),
               Xml::SubstReaderException);
}

TEST(ReadSubst, SelectMissingWhenClausesThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Select/>");
  EXPECT_THROW(Xml::readSubstitution(doc.RootElement(), TI),
               Xml::SubstReaderException);
}

TEST(ReadSubst, AnyMissingVariablesThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<ANY_Sub>
    <Pred><Exp_Comparison op=":"><Id value="x" typref="0"/><Id value="INTEGER" typref="1"/></Exp_Comparison></Pred>
    <Then><Skip/></Then>
  </ANY_Sub>)");
  EXPECT_THROW(Xml::readSubstitution(doc.RootElement(), TI),
               Xml::SubstReaderException);
}

TEST(ReadSubst, WhileMissingConditionThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<While>
    <Body><Skip/></Body>
    <Invariant><Exp_Comparison op="="><Id value="x" typref="0"/><Integer_Literal value="0" typref="0"/></Exp_Comparison></Invariant>
    <Variant><Id value="x" typref="0"/></Variant>
  </While>)");
  EXPECT_THROW(Xml::readSubstitution(doc.RootElement(), TI),
               Xml::SubstReaderException);
}

TEST(ReadSubst, OperationCallMissingNameThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Operation_Call>
    <Operation name="op"><Body><Skip/></Body></Operation>
  </Operation_Call>)");
  EXPECT_THROW(Xml::readSubstitution(doc.RootElement(), TI),
               Xml::SubstReaderException);
}

TEST(ReadSubst, OperationCallParamMismatchThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Operation_Call>
    <Name><Id value="op" typref="0"/></Name>
    <Input_Parameters>
      <Integer_Literal value="1" typref="0"/>
      <Integer_Literal value="2" typref="0"/>
    </Input_Parameters>
    <Operation name="op">
      <Input_Parameters>
        <Id value="p" typref="0"/>
      </Input_Parameters>
      <Body><Skip/></Body>
    </Operation>
  </Operation_Call>)");
  EXPECT_THROW(Xml::readSubstitution(doc.RootElement(), TI),
               Xml::SubstReaderException);
}

TEST(ReadSubst, WitnessMissingWitnessesThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Witness><Body><Skip/></Body></Witness>)");
  EXPECT_THROW(Xml::readSubstitution(doc.RootElement(), TI),
               Xml::SubstReaderException);
}

TEST(ReadSubst, WitnessBadChildThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Witness>
    <Witnesses><Bogus/></Witnesses>
    <Body><Skip/></Body>
  </Witness>)");
  EXPECT_THROW(Xml::readSubstitution(doc.RootElement(), TI),
               Xml::SubstReaderException);
}
