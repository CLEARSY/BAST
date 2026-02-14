#include <gtest/gtest.h>

#include "exprReader.h"
#include "exprDesc.h"
#include "tinyxml2.h"

// typeInfos: [0]=INT, [1]=POW_INT, [2]=BOOL, [3]=POW_BOOL, [4]=STRING,
//            [5]=REAL, [6]=POW(PROD(INT,INT)), [7]=STRUCT({a:INT,b:INT})
static const auto structAB =
    BType::STRUCT({{"a", BType::INT}, {"b", BType::INT}});
static const std::vector<BType> TI = {
    BType::INT,                                // 0
    BType::POW_INT,                            // 1
    BType::BOOL,                               // 2
    BType::POW_BOOL,                           // 3
    BType::STRING,                             // 4
    BType::REAL,                               // 5
    BType::POW(BType::PROD(BType::INT, BType::INT)),  // 6
    structAB,                                  // 7
};

// --- Literals ---

TEST(ReadExpr, IntegerLiteral) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Integer_Literal value=\"42\" typref=\"0\"/>");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::IntegerLiteral);
  EXPECT_EQ(e.getIntegerLiteral(), "42");
}

TEST(ReadExpr, StringLiteral) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<String_Literal value=\"hello\" typref=\"4\"/>");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::StringLiteral);
  EXPECT_EQ(e.getStringLiteral(), "hello");
}

TEST(ReadExpr, STRING_Literal) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<STRING_Literal value=\"world\" typref=\"4\"/>");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::StringLiteral);
  EXPECT_EQ(e.getStringLiteral(), "world");
}

TEST(ReadExpr, RealLiteralWithDecimal) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Real_Literal value=\"3.14\" typref=\"5\"/>");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::RealLiteral);
  EXPECT_EQ(e.getRealLiteral().integerPart, "3");
  EXPECT_EQ(e.getRealLiteral().fractionalPart, "14");
}

TEST(ReadExpr, RealLiteralIntegerOnly) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Real_Literal value=\"5\" typref=\"5\"/>");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::RealLiteral);
  EXPECT_EQ(e.getRealLiteral().integerPart, "5");
}

TEST(ReadExpr, BooleanLiteralTrue) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Boolean_Literal value=\"TRUE\" typref=\"2\"/>");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::TRUE);
}

TEST(ReadExpr, BooleanLiteralFalse) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Boolean_Literal value=\"FALSE\" typref=\"2\"/>");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::FALSE);
}

TEST(ReadExpr, BooleanLiteralCaseInsensitive) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Boolean_Literal value=\"true\" typref=\"2\"/>");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::TRUE);
}

// --- Identifiers ---

TEST(ReadExpr, IdPlain) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Id value=\"x\" typref=\"0\"/>");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::Id);
  EXPECT_EQ(e.getId(), VarName::makeVarWithoutSuffix("x"));
  EXPECT_EQ(e.getType(), BType::INT);
}

TEST(ReadExpr, IdWithSuffix) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Id value=\"x\" suffix=\"1\" typref=\"0\"/>");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::Id);
  EXPECT_EQ(e.getId(), VarName::makeVar("x", 1));
}

TEST(ReadExpr, IdWithSuffix0) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Id value=\"x\" suffix=\"0\" typref=\"0\"/>");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::Id);
  EXPECT_EQ(e.getId(), VarName::makeVarWithoutSuffix("x"));
}

TEST(ReadExpr, FreshId) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Fresh_Id ref=\"f1\" typref=\"0\"/>");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::Id);
  EXPECT_EQ(e.getId(), VarName::makeFreshId("f1"));
}

TEST(ReadExpr, IdWithRichtypref) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Id value=\"y\" richtypref=\"0\"/>");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::Id);
  EXPECT_EQ(e.getType(), BType::INT);
}

// --- Constants ---

TEST(ReadExpr, ConstMAXINT) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Id value=\"MAXINT\" typref=\"0\"/>");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::MaxInt);
}

TEST(ReadExpr, ConstMININT) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Id value=\"MININT\" typref=\"0\"/>");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::MinInt);
}

TEST(ReadExpr, ConstINTEGER) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Id value=\"INTEGER\" typref=\"1\"/>");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::INTEGER);
}

TEST(ReadExpr, ConstNATURAL) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Id value=\"NATURAL\" typref=\"1\"/>");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::NATURAL);
}

TEST(ReadExpr, ConstNATURAL1) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Id value=\"NATURAL1\" typref=\"1\"/>");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::NATURAL1);
}

TEST(ReadExpr, ConstINT) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Id value=\"INT\" typref=\"1\"/>");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::INT);
}

TEST(ReadExpr, ConstNAT) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Id value=\"NAT\" typref=\"1\"/>");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::NAT);
}

TEST(ReadExpr, ConstNAT1) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Id value=\"NAT1\" typref=\"1\"/>");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::NAT1);
}

TEST(ReadExpr, ConstSTRING) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Id value=\"STRING\" typref=\"1\"/>");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::STRING);
}

TEST(ReadExpr, ConstBOOL) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Id value=\"BOOL\" typref=\"1\"/>");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::BOOL);
}

TEST(ReadExpr, ConstREAL) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Id value=\"REAL\" typref=\"1\"/>");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::REAL);
}

TEST(ReadExpr, ConstFLOAT) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Id value=\"FLOAT\" typref=\"1\"/>");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::FLOAT);
}

TEST(ReadExpr, ConstTRUE) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Id value=\"TRUE\" typref=\"2\"/>");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::TRUE);
}

TEST(ReadExpr, ConstFALSE) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Id value=\"FALSE\" typref=\"2\"/>");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::FALSE);
}

TEST(ReadExpr, ConstSuccessor) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Id value=\"succ\" typref=\"0\"/>");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::Successor);
}

TEST(ReadExpr, ConstPredecessor) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Id value=\"pred\" typref=\"0\"/>");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::Predecessor);
}

// --- EmptySet / EmptySeq ---

TEST(ReadExpr, EmptySet) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<EmptySet typref=\"1\"/>");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::EmptySet);
  EXPECT_EQ(e.getType(), TI[1]);
}

TEST(ReadExpr, EmptySeq) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<EmptySeq typref=\"1\"/>");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::EmptySet);
}

// --- Binary expressions ---

TEST(ReadExpr, BinaryExpAddition) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Binary_Exp op="+i" typref="0">
    <Integer_Literal value="1" typref="0"/>
    <Integer_Literal value="2" typref="0"/>
  </Binary_Exp>)");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::BinaryExpr);
  EXPECT_EQ(e.getType(), BType::INT);
  auto &bin = e.toBinaryExpr();
  EXPECT_EQ(bin.op, Expr::BinaryOp::IAddition);
  EXPECT_EQ(bin.lhs.getIntegerLiteral(), "1");
  EXPECT_EQ(bin.rhs.getIntegerLiteral(), "2");
}

TEST(ReadExpr, BinaryExpMapplet) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Binary_Exp op="|->" typref="6">
    <Integer_Literal value="1" typref="0"/>
    <Integer_Literal value="2" typref="0"/>
  </Binary_Exp>)");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.toBinaryExpr().op, Expr::BinaryOp::Mapplet);
}

TEST(ReadExpr, BinaryExpInterval) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Binary_Exp op=".." typref="1">
    <Integer_Literal value="1" typref="0"/>
    <Integer_Literal value="10" typref="0"/>
  </Binary_Exp>)");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.toBinaryExpr().op, Expr::BinaryOp::Interval);
}

// --- Unary expressions ---

TEST(ReadExpr, UnaryExpCard) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Unary_Exp op="card" typref="0">
    <Id value="s" typref="1"/>
  </Unary_Exp>)");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::UnaryExpr);
  EXPECT_EQ(e.toUnaryExpr().op, Expr::UnaryOp::Cardinality);
}

TEST(ReadExpr, UnaryExpDom) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Unary_Exp op="dom" typref="1">
    <Id value="f" typref="6"/>
  </Unary_Exp>)");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.toUnaryExpr().op, Expr::UnaryOp::Domain);
}

TEST(ReadExpr, UnaryExpSucc) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Unary_Exp op="succ" typref="0">
    <Integer_Literal value="5" typref="0"/>
  </Unary_Exp>)");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  // succ is rewritten as Application(successor, arg)
  EXPECT_EQ(e.getTag(), Expr::EKind::BinaryExpr);
  EXPECT_EQ(e.toBinaryExpr().op, Expr::BinaryOp::Application);
  EXPECT_EQ(e.toBinaryExpr().lhs.getTag(), Expr::EKind::Successor);
}

TEST(ReadExpr, UnaryExpPred) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Unary_Exp op="pred" typref="0">
    <Integer_Literal value="5" typref="0"/>
  </Unary_Exp>)");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::BinaryExpr);
  EXPECT_EQ(e.toBinaryExpr().op, Expr::BinaryOp::Application);
  EXPECT_EQ(e.toBinaryExpr().lhs.getTag(), Expr::EKind::Predecessor);
}

// --- Nary expressions ---

TEST(ReadExpr, NaryExpSet) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Nary_Exp op="{" typref="1">
    <Integer_Literal value="1" typref="0"/>
    <Integer_Literal value="2" typref="0"/>
    <Integer_Literal value="3" typref="0"/>
  </Nary_Exp>)");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::NaryExpr);
  EXPECT_EQ(e.toNaryExpr().op, Expr::NaryOp::Set);
  EXPECT_EQ(e.toNaryExpr().vec.size(), 3u);
}

TEST(ReadExpr, NaryExpSequence) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Nary_Exp op="[" typref="6">
    <Integer_Literal value="10" typref="0"/>
  </Nary_Exp>)");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.toNaryExpr().op, Expr::NaryOp::Sequence);
}

// --- Ternary expressions ---

TEST(ReadExpr, TernaryExpSon) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Ternary_Exp op="son" typref="0">
    <Integer_Literal value="1" typref="0"/>
    <Integer_Literal value="2" typref="0"/>
    <Integer_Literal value="3" typref="0"/>
  </Ternary_Exp>)");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::TernaryExpr);
  EXPECT_EQ(e.toTernaryExpr().op, Expr::TernaryOp::Son);
}

TEST(ReadExpr, TernaryExpBin) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Ternary_Exp op="bin" typref="0">
    <Integer_Literal value="1" typref="0"/>
    <Integer_Literal value="2" typref="0"/>
    <Integer_Literal value="3" typref="0"/>
  </Ternary_Exp>)");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.toTernaryExpr().op, Expr::TernaryOp::Bin);
}

// --- Quantified expression ---

TEST(ReadExpr, QuantifiedExprLambda) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Quantified_Exp type="%" typref="6">
    <Variables>
      <Id value="x" typref="0"/>
    </Variables>
    <Pred>
      <Exp_Comparison op=":">
        <Id value="x" typref="0"/>
        <Id value="INTEGER" typref="1"/>
      </Exp_Comparison>
    </Pred>
    <Body>
      <Id value="x" typref="0"/>
    </Body>
  </Quantified_Exp>)");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::QuantifiedExpr);
}

TEST(ReadExpr, QuantifiedExprISigma) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Quantified_Exp type="iSIGMA" typref="0">
    <Variables>
      <Id value="x" typref="0"/>
    </Variables>
    <Pred>
      <Exp_Comparison op=":">
        <Id value="x" typref="0"/>
        <Id value="NAT" typref="1"/>
      </Exp_Comparison>
    </Pred>
    <Body>
      <Id value="x" typref="0"/>
    </Body>
  </Quantified_Exp>)");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::QuantifiedExpr);
}

// --- Quantified set ---

TEST(ReadExpr, QuantifiedSet) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Quantified_Set typref="1">
    <Variables>
      <Id value="x" typref="0"/>
    </Variables>
    <Body>
      <Exp_Comparison op="&gt;i">
        <Id value="x" typref="0"/>
        <Integer_Literal value="0" typref="0"/>
      </Exp_Comparison>
    </Body>
  </Quantified_Set>)");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::QuantifiedSet);
}

// --- Boolean expression ---

TEST(ReadExpr, BooleanExp) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Boolean_Exp typref="2">
    <Exp_Comparison op="=">
      <Id value="x" typref="0"/>
      <Integer_Literal value="0" typref="0"/>
    </Exp_Comparison>
  </Boolean_Exp>)");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::BooleanExpr);
}

// --- Struct / Record ---

TEST(ReadExpr, Struct) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Struct typref="7">
    <Record_Item label="a">
      <Id value="INTEGER" typref="1"/>
    </Record_Item>
    <Record_Item label="b">
      <Id value="INTEGER" typref="1"/>
    </Record_Item>
  </Struct>)");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::Struct);
}

TEST(ReadExpr, Record) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Record typref="7">
    <Record_Item label="a">
      <Integer_Literal value="1" typref="0"/>
    </Record_Item>
    <Record_Item label="b">
      <Integer_Literal value="2" typref="0"/>
    </Record_Item>
  </Record>)");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::Record);
}

// --- Record_Field_Access ---

TEST(ReadExpr, RecordFieldAccess) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Record_Field_Access label="a" typref="0">
    <Id value="r" typref="7"/>
  </Record_Field_Access>)");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::Record_Field_Access);
}

// --- Record_Update ---

TEST(ReadExpr, RecordFieldUpdate) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Record_Update label="a" typref="7">
    <Id value="r" typref="7"/>
    <Integer_Literal value="99" typref="0"/>
  </Record_Update>)");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_EQ(e.getTag(), Expr::EKind::Record_Field_Update);
}

// --- Tag attribute ---

TEST(ReadExpr, TagAttribute) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Integer_Literal value=\"1\" typref=\"0\" tag=\"foo\"/>");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  ASSERT_EQ(e.getBxmlTag().size(), 1u);
  EXPECT_EQ(e.getBxmlTag()[0], "foo");
}

TEST(ReadExpr, EmptyTagIgnored) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Integer_Literal value=\"1\" typref=\"0\" tag=\"\"/>");
  auto e = Xml::readExpression(doc.RootElement(), TI);
  EXPECT_TRUE(e.getBxmlTag().empty());
}

// --- Error cases ---

TEST(ReadExpr, NullDomThrows) {
  EXPECT_THROW(Xml::readExpression(nullptr, TI), Xml::ExprReaderException);
}

TEST(ReadExpr, UnknownTagThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Bogus typref=\"0\"/>");
  EXPECT_THROW(Xml::readExpression(doc.RootElement(), TI),
               Xml::ExprReaderException);
}

TEST(ReadExpr, MissingTyprefThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Integer_Literal value=\"1\"/>");
  EXPECT_THROW(Xml::readExpression(doc.RootElement(), TI),
               Xml::ExprReaderException);
}

TEST(ReadExpr, OutOfRangeTyprefThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Integer_Literal value=\"1\" typref=\"999\"/>");
  EXPECT_THROW(Xml::readExpression(doc.RootElement(), TI),
               Xml::ExprReaderException);
}

TEST(ReadExpr, UnknownBinaryOpThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Binary_Exp op="???" typref="0">
    <Integer_Literal value="1" typref="0"/>
    <Integer_Literal value="2" typref="0"/>
  </Binary_Exp>)");
  EXPECT_THROW(Xml::readExpression(doc.RootElement(), TI),
               Xml::ExprReaderException);
}

TEST(ReadExpr, UnknownUnaryOpThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Unary_Exp op="???" typref="0">
    <Integer_Literal value="1" typref="0"/>
  </Unary_Exp>)");
  EXPECT_THROW(Xml::readExpression(doc.RootElement(), TI),
               Xml::ExprReaderException);
}

TEST(ReadExpr, UnknownNaryOpThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Nary_Exp op="???" typref="0">
    <Integer_Literal value="1" typref="0"/>
  </Nary_Exp>)");
  EXPECT_THROW(Xml::readExpression(doc.RootElement(), TI),
               Xml::ExprReaderException);
}

TEST(ReadExpr, UnknownTernaryOpThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Ternary_Exp op="???" typref="0">
    <Integer_Literal value="1" typref="0"/>
    <Integer_Literal value="2" typref="0"/>
    <Integer_Literal value="3" typref="0"/>
  </Ternary_Exp>)");
  EXPECT_THROW(Xml::readExpression(doc.RootElement(), TI),
               Xml::ExprReaderException);
}

TEST(ReadExpr, UnknownQuantifiedExprTypeThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Quantified_Exp type="???" typref="0">
    <Variables><Id value="x" typref="0"/></Variables>
    <Pred><Exp_Comparison op=":"><Id value="x" typref="0"/><Id value="INTEGER" typref="1"/></Exp_Comparison></Pred>
    <Body><Id value="x" typref="0"/></Body>
  </Quantified_Exp>)");
  EXPECT_THROW(Xml::readExpression(doc.RootElement(), TI),
               Xml::ExprReaderException);
}

TEST(ReadExpr, UnknownBooleanLiteralThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Boolean_Literal value=\"MAYBE\" typref=\"2\"/>");
  EXPECT_THROW(Xml::readExpression(doc.RootElement(), TI),
               Xml::ExprReaderException);
}

// --- VarNameFromId ---

TEST(ReadExpr, VarNameFromIdPlain) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Id value=\"x\" typref=\"0\"/>");
  auto tv = Xml::VarNameFromId(doc.RootElement(), TI);
  EXPECT_EQ(tv.name, VarName::makeVarWithoutSuffix("x"));
  EXPECT_EQ(tv.type, BType::INT);
}

TEST(ReadExpr, VarNameFromIdFresh) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Fresh_Id ref=\"f1\" typref=\"0\"/>");
  auto tv = Xml::VarNameFromId(doc.RootElement(), TI);
  EXPECT_EQ(tv.name, VarName::makeFreshId("f1"));
}

TEST(ReadExpr, VarNameFromIdBadTagThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse("<Bogus typref=\"0\"/>");
  EXPECT_THROW(Xml::VarNameFromId(doc.RootElement(), TI),
               Xml::ExprReaderException);
}
