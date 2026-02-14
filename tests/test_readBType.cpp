#include <gtest/gtest.h>

#include "btypeReader.h"
#include "tinyxml2.h"

// ============================================================
// readTypeInfos tests
// ============================================================

TEST(ReadTypeInfos, NullDomReturnsEmpty) {
  std::vector<BType> ti;
  Xml::readTypeInfos(nullptr, ti);
  EXPECT_TRUE(ti.empty());
}

TEST(ReadTypeInfos, PrimitiveINTEGER) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Root>
    <TypeInfos>
      <Type id="0"><Id value="INTEGER"/></Type>
    </TypeInfos>
  </Root>)");
  auto *tiElem = doc.RootElement()->FirstChildElement("TypeInfos");
  std::vector<BType> ti;
  Xml::readTypeInfos(tiElem, ti);
  ASSERT_EQ(ti.size(), 1u);
  EXPECT_EQ(ti[0], BType::INT);
}

TEST(ReadTypeInfos, PrimitiveFLOAT) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Root>
    <TypeInfos>
      <Type id="0"><Id value="FLOAT"/></Type>
    </TypeInfos>
  </Root>)");
  auto *tiElem = doc.RootElement()->FirstChildElement("TypeInfos");
  std::vector<BType> ti;
  Xml::readTypeInfos(tiElem, ti);
  ASSERT_EQ(ti.size(), 1u);
  EXPECT_EQ(ti[0], BType::FLOAT);
}

TEST(ReadTypeInfos, PrimitiveREAL) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Root>
    <TypeInfos>
      <Type id="0"><Id value="REAL"/></Type>
    </TypeInfos>
  </Root>)");
  auto *tiElem = doc.RootElement()->FirstChildElement("TypeInfos");
  std::vector<BType> ti;
  Xml::readTypeInfos(tiElem, ti);
  ASSERT_EQ(ti.size(), 1u);
  EXPECT_EQ(ti[0], BType::REAL);
}

TEST(ReadTypeInfos, PrimitiveSTRING) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Root>
    <TypeInfos>
      <Type id="0"><Id value="STRING"/></Type>
    </TypeInfos>
  </Root>)");
  auto *tiElem = doc.RootElement()->FirstChildElement("TypeInfos");
  std::vector<BType> ti;
  Xml::readTypeInfos(tiElem, ti);
  ASSERT_EQ(ti.size(), 1u);
  EXPECT_EQ(ti[0], BType::STRING);
}

TEST(ReadTypeInfos, PrimitiveBOOL) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Root>
    <TypeInfos>
      <Type id="0"><Id value="BOOL"/></Type>
    </TypeInfos>
  </Root>)");
  auto *tiElem = doc.RootElement()->FirstChildElement("TypeInfos");
  std::vector<BType> ti;
  Xml::readTypeInfos(tiElem, ti);
  ASSERT_EQ(ti.size(), 1u);
  EXPECT_EQ(ti[0], BType::BOOL);
}

TEST(ReadTypeInfos, MultiplePrimitives) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Root>
    <TypeInfos>
      <Type id="0"><Id value="INTEGER"/></Type>
      <Type id="1"><Id value="BOOL"/></Type>
      <Type id="2"><Id value="STRING"/></Type>
    </TypeInfos>
  </Root>)");
  auto *tiElem = doc.RootElement()->FirstChildElement("TypeInfos");
  std::vector<BType> ti;
  Xml::readTypeInfos(tiElem, ti);
  ASSERT_EQ(ti.size(), 3u);
  EXPECT_EQ(ti[0], BType::INT);
  EXPECT_EQ(ti[1], BType::BOOL);
  EXPECT_EQ(ti[2], BType::STRING);
}

TEST(ReadTypeInfos, PowerType) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Root>
    <TypeInfos>
      <Type id="0"><Unary_Exp op="POW"><Id value="INTEGER"/></Unary_Exp></Type>
    </TypeInfos>
  </Root>)");
  auto *tiElem = doc.RootElement()->FirstChildElement("TypeInfos");
  std::vector<BType> ti;
  Xml::readTypeInfos(tiElem, ti);
  ASSERT_EQ(ti.size(), 1u);
  EXPECT_EQ(ti[0], BType::POW(BType::INT));
}

TEST(ReadTypeInfos, ProductType) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Root>
    <TypeInfos>
      <Type id="0">
        <Binary_Exp op="*">
          <Id value="INTEGER"/>
          <Id value="BOOL"/>
        </Binary_Exp>
      </Type>
    </TypeInfos>
  </Root>)");
  auto *tiElem = doc.RootElement()->FirstChildElement("TypeInfos");
  std::vector<BType> ti;
  Xml::readTypeInfos(tiElem, ti);
  ASSERT_EQ(ti.size(), 1u);
  EXPECT_EQ(ti[0], BType::PROD(BType::INT, BType::BOOL));
}

TEST(ReadTypeInfos, StructType) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Root>
    <TypeInfos>
      <Type id="0">
        <Struct>
          <Record_Item label="a"><Id value="INTEGER"/></Record_Item>
          <Record_Item label="b"><Id value="BOOL"/></Record_Item>
        </Struct>
      </Type>
    </TypeInfos>
  </Root>)");
  auto *tiElem = doc.RootElement()->FirstChildElement("TypeInfos");
  std::vector<BType> ti;
  Xml::readTypeInfos(tiElem, ti);
  ASSERT_EQ(ti.size(), 1u);
  EXPECT_EQ(ti[0], BType::STRUCT({{"a", BType::INT}, {"b", BType::BOOL}}));
}

TEST(ReadTypeInfos, AbstractSetFromSets) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Root>
    <Sets>
      <Set>
        <Id value="COLOR"/>
      </Set>
    </Sets>
    <TypeInfos>
      <Type id="0"><Id value="COLOR"/></Type>
    </TypeInfos>
  </Root>)");
  auto *tiElem = doc.RootElement()->FirstChildElement("TypeInfos");
  std::vector<BType> ti;
  Xml::readTypeInfos(tiElem, ti);
  ASSERT_EQ(ti.size(), 1u);
  EXPECT_EQ(ti[0], BType::ABSTRACT_SET("COLOR"));
}

TEST(ReadTypeInfos, EnumeratedSetFromSets) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Root>
    <Sets>
      <Set>
        <Id value="COLOR"/>
        <Enumerated_Values>
          <Id value="red"/>
          <Id value="green"/>
          <Id value="blue"/>
        </Enumerated_Values>
      </Set>
    </Sets>
    <TypeInfos>
      <Type id="0"><Id value="COLOR"/></Type>
    </TypeInfos>
  </Root>)");
  auto *tiElem = doc.RootElement()->FirstChildElement("TypeInfos");
  std::vector<BType> ti;
  Xml::readTypeInfos(tiElem, ti);
  ASSERT_EQ(ti.size(), 1u);
  EXPECT_EQ(ti[0],
            BType::ENUMERATED_SET({"COLOR", {"red", "green", "blue"}}));
}

TEST(ReadTypeInfos, SetFromDefine) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Root>
    <Define>
      <Set>
        <Id value="STATUS"/>
      </Set>
    </Define>
    <TypeInfos>
      <Type id="0"><Id value="STATUS"/></Type>
    </TypeInfos>
  </Root>)");
  auto *tiElem = doc.RootElement()->FirstChildElement("TypeInfos");
  std::vector<BType> ti;
  Xml::readTypeInfos(tiElem, ti);
  ASSERT_EQ(ti.size(), 1u);
  EXPECT_EQ(ti[0], BType::ABSTRACT_SET("STATUS"));
}

TEST(ReadTypeInfos, NestedPowerProduct) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Root>
    <TypeInfos>
      <Type id="0">
        <Unary_Exp op="POW">
          <Binary_Exp op="*">
            <Id value="INTEGER"/>
            <Id value="INTEGER"/>
          </Binary_Exp>
        </Unary_Exp>
      </Type>
    </TypeInfos>
  </Root>)");
  auto *tiElem = doc.RootElement()->FirstChildElement("TypeInfos");
  std::vector<BType> ti;
  Xml::readTypeInfos(tiElem, ti);
  ASSERT_EQ(ti.size(), 1u);
  EXPECT_EQ(ti[0], BType::POW(BType::PROD(BType::INT, BType::INT)));
}

// --- Error cases ---

TEST(ReadTypeInfos, MissingIdAttributeThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Root>
    <TypeInfos>
      <Type><Id value="INTEGER"/></Type>
    </TypeInfos>
  </Root>)");
  auto *tiElem = doc.RootElement()->FirstChildElement("TypeInfos");
  std::vector<BType> ti;
  EXPECT_THROW(Xml::readTypeInfos(tiElem, ti), Xml::BTypeReaderException);
}

TEST(ReadTypeInfos, NonSequentialIdThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Root>
    <TypeInfos>
      <Type id="1"><Id value="INTEGER"/></Type>
    </TypeInfos>
  </Root>)");
  auto *tiElem = doc.RootElement()->FirstChildElement("TypeInfos");
  std::vector<BType> ti;
  EXPECT_THROW(Xml::readTypeInfos(tiElem, ti), Xml::BTypeReaderException);
}

TEST(ReadTypeInfos, UnknownSetNameThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Root>
    <TypeInfos>
      <Type id="0"><Id value="UNKNOWN_SET"/></Type>
    </TypeInfos>
  </Root>)");
  auto *tiElem = doc.RootElement()->FirstChildElement("TypeInfos");
  std::vector<BType> ti;
  EXPECT_THROW(Xml::readTypeInfos(tiElem, ti), Xml::BTypeReaderException);
}

TEST(ReadTypeInfos, UnexpectedTagThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<Root>
    <TypeInfos>
      <Type id="0"><Bogus/></Type>
    </TypeInfos>
  </Root>)");
  auto *tiElem = doc.RootElement()->FirstChildElement("TypeInfos");
  std::vector<BType> ti;
  EXPECT_THROW(Xml::readTypeInfos(tiElem, ti), Xml::BTypeReaderException);
}

// ============================================================
// readRichTypesInfo tests
// ============================================================

TEST(ReadRichTypesInfo, NullDomReturnsEmpty) {
  std::vector<BType> ti;
  Xml::readRichTypesInfo(nullptr, ti);
  EXPECT_TRUE(ti.empty());
}

TEST(ReadRichTypesInfo, PrimitiveINTEGER) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<RichTypesInfo>
    <RichType id="0"><INTEGER/></RichType>
  </RichTypesInfo>)");
  std::vector<BType> ti;
  Xml::readRichTypesInfo(doc.RootElement(), ti);
  ASSERT_EQ(ti.size(), 1u);
  EXPECT_EQ(ti[0], BType::INT);
}

TEST(ReadRichTypesInfo, PrimitiveBOOL) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<RichTypesInfo>
    <RichType id="0"><BOOL/></RichType>
  </RichTypesInfo>)");
  std::vector<BType> ti;
  Xml::readRichTypesInfo(doc.RootElement(), ti);
  ASSERT_EQ(ti.size(), 1u);
  EXPECT_EQ(ti[0], BType::BOOL);
}

TEST(ReadRichTypesInfo, PrimitiveREAL) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<RichTypesInfo>
    <RichType id="0"><REAL/></RichType>
  </RichTypesInfo>)");
  std::vector<BType> ti;
  Xml::readRichTypesInfo(doc.RootElement(), ti);
  ASSERT_EQ(ti.size(), 1u);
  EXPECT_EQ(ti[0], BType::REAL);
}

TEST(ReadRichTypesInfo, PrimitiveFLOAT) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<RichTypesInfo>
    <RichType id="0"><FLOAT/></RichType>
  </RichTypesInfo>)");
  std::vector<BType> ti;
  Xml::readRichTypesInfo(doc.RootElement(), ti);
  ASSERT_EQ(ti.size(), 1u);
  EXPECT_EQ(ti[0], BType::FLOAT);
}

TEST(ReadRichTypesInfo, PrimitiveSTRING) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<RichTypesInfo>
    <RichType id="0"><STRING/></RichType>
  </RichTypesInfo>)");
  std::vector<BType> ti;
  Xml::readRichTypesInfo(doc.RootElement(), ti);
  ASSERT_EQ(ti.size(), 1u);
  EXPECT_EQ(ti[0], BType::STRING);
}

TEST(ReadRichTypesInfo, AbstractSet) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<RichTypesInfo>
    <RichType id="0"><AbstractSet id="COLOR"/></RichType>
  </RichTypesInfo>)");
  std::vector<BType> ti;
  Xml::readRichTypesInfo(doc.RootElement(), ti);
  ASSERT_EQ(ti.size(), 1u);
  EXPECT_EQ(ti[0], BType::ABSTRACT_SET("COLOR"));
}

TEST(ReadRichTypesInfo, EnumeratedSet) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<RichTypesInfo>
    <RichType id="0">
      <EnumeratedSet id="COLOR">
        <EnumeratedValue id="red"/>
        <EnumeratedValue id="green"/>
        <EnumeratedValue id="blue"/>
      </EnumeratedSet>
    </RichType>
  </RichTypesInfo>)");
  std::vector<BType> ti;
  Xml::readRichTypesInfo(doc.RootElement(), ti);
  ASSERT_EQ(ti.size(), 1u);
  EXPECT_EQ(ti[0],
            BType::ENUMERATED_SET({"COLOR", {"red", "green", "blue"}}));
}

TEST(ReadRichTypesInfo, PowerSetRefPrevious) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<RichTypesInfo>
    <RichType id="0"><INTEGER/></RichType>
    <RichType id="1"><PowerSet arg="0"/></RichType>
  </RichTypesInfo>)");
  std::vector<BType> ti;
  Xml::readRichTypesInfo(doc.RootElement(), ti);
  ASSERT_EQ(ti.size(), 2u);
  EXPECT_EQ(ti[0], BType::INT);
  EXPECT_EQ(ti[1], BType::POW(BType::INT));
}

TEST(ReadRichTypesInfo, MultipleMixed) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<RichTypesInfo>
    <RichType id="0"><INTEGER/></RichType>
    <RichType id="1"><BOOL/></RichType>
    <RichType id="2"><AbstractSet id="S"/></RichType>
    <RichType id="3"><PowerSet arg="0"/></RichType>
  </RichTypesInfo>)");
  std::vector<BType> ti;
  Xml::readRichTypesInfo(doc.RootElement(), ti);
  ASSERT_EQ(ti.size(), 4u);
  EXPECT_EQ(ti[0], BType::INT);
  EXPECT_EQ(ti[1], BType::BOOL);
  EXPECT_EQ(ti[2], BType::ABSTRACT_SET("S"));
  EXPECT_EQ(ti[3], BType::POW(BType::INT));
}

// --- Error cases ---

TEST(ReadRichTypesInfo, MissingIdAttributeThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<RichTypesInfo>
    <RichType><INTEGER/></RichType>
  </RichTypesInfo>)");
  std::vector<BType> ti;
  EXPECT_THROW(Xml::readRichTypesInfo(doc.RootElement(), ti),
               Xml::BTypeReaderException);
}

TEST(ReadRichTypesInfo, NonSequentialIdThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<RichTypesInfo>
    <RichType id="1"><INTEGER/></RichType>
  </RichTypesInfo>)");
  std::vector<BType> ti;
  EXPECT_THROW(Xml::readRichTypesInfo(doc.RootElement(), ti),
               Xml::BTypeReaderException);
}

TEST(ReadRichTypesInfo, UnexpectedTagThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<RichTypesInfo>
    <RichType id="0"><Bogus/></RichType>
  </RichTypesInfo>)");
  std::vector<BType> ti;
  EXPECT_THROW(Xml::readRichTypesInfo(doc.RootElement(), ti),
               Xml::BTypeReaderException);
}

TEST(ReadRichTypesInfo, AbstractSetMissingIdThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<RichTypesInfo>
    <RichType id="0"><AbstractSet/></RichType>
  </RichTypesInfo>)");
  std::vector<BType> ti;
  EXPECT_THROW(Xml::readRichTypesInfo(doc.RootElement(), ti),
               Xml::BTypeReaderException);
}

TEST(ReadRichTypesInfo, EnumeratedSetMissingIdThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<RichTypesInfo>
    <RichType id="0"><EnumeratedSet/></RichType>
  </RichTypesInfo>)");
  std::vector<BType> ti;
  EXPECT_THROW(Xml::readRichTypesInfo(doc.RootElement(), ti),
               Xml::BTypeReaderException);
}

TEST(ReadRichTypesInfo, EnumeratedValueMissingIdThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<RichTypesInfo>
    <RichType id="0">
      <EnumeratedSet id="S">
        <EnumeratedValue/>
      </EnumeratedSet>
    </RichType>
  </RichTypesInfo>)");
  std::vector<BType> ti;
  EXPECT_THROW(Xml::readRichTypesInfo(doc.RootElement(), ti),
               Xml::BTypeReaderException);
}

TEST(ReadRichTypesInfo, PowerSetMissingArgThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<RichTypesInfo>
    <RichType id="0"><PowerSet/></RichType>
  </RichTypesInfo>)");
  std::vector<BType> ti;
  EXPECT_THROW(Xml::readRichTypesInfo(doc.RootElement(), ti),
               Xml::BTypeReaderException);
}

TEST(ReadRichTypesInfo, PowerSetOutOfRangeArgThrows) {
  tinyxml2::XMLDocument doc;
  doc.Parse(R"(<RichTypesInfo>
    <RichType id="0"><PowerSet arg="99"/></RichType>
  </RichTypesInfo>)");
  std::vector<BType> ti;
  EXPECT_THROW(Xml::readRichTypesInfo(doc.RootElement(), ti),
               Xml::BTypeReaderException);
}
