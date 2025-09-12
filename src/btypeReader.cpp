/*
   This file is part of BAST.
   Copyright © CLEARSY 2025
   BAST is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "btypeReader.h"

#include <cassert>

namespace Xml {

static BType readType(const tinyxml2::XMLElement *dom) {
  if (nullptr == dom) throw BTypeReaderException("Null dom element.", 0);

  auto tag{dom->Name()};
  if (0 == strcmp(tag, "Id")) {
    auto value{dom->Attribute("value")};
    if (0 == strcmp(value, "INTEGER")) {
      return BType::INT;
    } else if (0 == strcmp(value, "FLOAT")) {
      return BType::FLOAT;
    } else if (0 == strcmp(value, "REAL")) {
      return BType::REAL;
    } else if (0 == strcmp(value, "STRING")) {
      return BType::STRING;
    } else if (0 == strcmp(value, "BOOL")) {
      return BType::BOOL;
    } else {
      if (nullptr != dom->Attribute("suffix"))
        throw BTypeReaderException(
            "Abstract or Concrete Set with suffix.",
            dom->GetLineNum());  // this constraint could be removed
      return BType::INT;
    }
  } else if (0 == strcmp(tag, "Unary_Exp")) {
    assert(0 == strcmp(dom->Attribute("op"), "POW"));
    return BType::POW(readType(dom->FirstChildElement()));
  } else if (0 == strcmp(tag, "Binary_Exp")) {
    assert(0 == strcmp(dom->Attribute("op"), "*"));
    const tinyxml2::XMLElement *fst{dom->FirstChildElement()};
    return BType::PROD(readType(fst), readType(fst->NextSiblingElement()));
  } else if (0 == strcmp(tag, "Struct")) {
    std::vector<std::pair<std::string, BType>> fields;
    for (const tinyxml2::XMLElement *item =
             dom->FirstChildElement("Record_Item");
         nullptr != item; item = item->NextSiblingElement("Record_Item")) {
      fields.push_back({std::string(item->Attribute("label")),
                        readType(item->FirstChildElement())});
    }
    return BType::STRUCT(fields);
  } else {
    throw BTypeReaderException("Unexpected Tag", dom->GetLineNum());
  }
  assert(false);  // unreachable
}

static BType readRichType(const tinyxml2::XMLElement *dom,
                          const std::vector<BType> &typeInfos) {
  if (nullptr == dom) throw BTypeReaderException("Null dom element.", 0);

  auto tag{dom->Name()};
  if (0 == strcmp(tag, "INTEGER")) {
    return BType::INT;
  } else if (0 == strcmp(tag, "BOOL")) {
    return BType::BOOL;
  } else if (0 == strcmp(tag, "REAL")) {
    return BType::REAL;
  } else if (0 == strcmp(tag, "FLOAT")) {
    return BType::FLOAT;
  } else if (0 == strcmp(tag, "STRING")) {
    return BType::STRING;
  } else if (0 == strcmp(tag, "AbstractSet")) {
    if (dom->Attribute("id") == nullptr)
      throw BTypeReaderException("Missing id attribute in AbstractSet element",
                                 dom->GetLineNum());
    std::string id = dom->Attribute("id");
    return BType::ABSTRACT_SET(id);
  } else if (0 == strcmp(tag, "EnumeratedSet")) {
    if (dom->Attribute("id") == nullptr)
      throw BTypeReaderException("Missing id attribute in EnumerateSet element",
                                 dom->GetLineNum());
    std::string id = dom->Attribute("id");
    std::vector<std::string> values;
    for (auto dom2 = dom->FirstChildElement("EnumeratedValue"); dom2 != nullptr;
         dom2 = dom2->NextSiblingElement("EnumeratedValue")) {
      if (dom2->Attribute("id") == nullptr)
        throw BTypeReaderException(
            "Missing id attribute in EnumerateValue element",
            dom->GetLineNum());
      values.push_back(dom2->Attribute("id"));
    }
    return BType::ENUMERATED_SET({id, values});
  } else if (0 == strcmp(tag, "PowerSet")) {
    if (dom->Attribute("arg") == nullptr)
      throw BTypeReaderException("Missing arg attribute in PowerSet element",
                                 dom->GetLineNum());
    auto arg = static_cast<size_t>(dom->Unsigned64Attribute("arg"));
    if (typeInfos.size() <= arg)
      throw BTypeReaderException("Unresolved arg reference in PowerSet element",
                                 dom->GetLineNum());
    BType argtype = typeInfos.at(arg);
    return BType::POW(argtype);
  } else {
    throw BTypeReaderException("Unexpected Tag", dom->GetLineNum());
  }
  assert(false);  // unreachable
}

void readTypeInfos(const tinyxml2::XMLElement *dom,
                   std::vector<BType> &typeInfos) {
  assert(typeInfos.size() == 0);
  if (nullptr == dom) return;

  int cpt = 0;
  for (const tinyxml2::XMLElement *typ = dom->FirstChildElement("Type");
       nullptr != typ; typ = typ->NextSiblingElement()) {
    int typref;
    if (tinyxml2::XML_SUCCESS != typ->QueryIntAttribute("id", &typref))
      throw BTypeReaderException("Integer expected", typ->GetLineNum());
    if (typref != cpt)
      throw BTypeReaderException("Unexpected typref. Expecting '" +
                                     std::to_string(cpt) + "'. Found '" +
                                     std::to_string(typref) + "'.",
                                 typ->GetLineNum());
    typeInfos.push_back(readType(typ->FirstChildElement()));
    cpt++;
  }
}

void readRichTypesInfo(const tinyxml2::XMLElement *dom,
                       std::vector<BType> &typeInfos) {
  assert(typeInfos.size() == 0);
  if (nullptr == dom) return;

  int cpt = 0;
  for (const tinyxml2::XMLElement *typ = dom->FirstChildElement("RichType");
       nullptr != typ; typ = typ->NextSiblingElement()) {
    int typref;
    if (tinyxml2::XML_SUCCESS != typ->QueryIntAttribute("id", &typref))
      throw BTypeReaderException("Integer expected", typ->GetLineNum());
    if (typref != cpt)
      throw BTypeReaderException("Unexpected typref. Expecting '" +
                                     std::to_string(cpt) + "'. Found '" +
                                     std::to_string(typref) + "'.",
                                 typ->GetLineNum());
    auto type = readRichType(typ->FirstChildElement(), typeInfos);
    typeInfos.push_back(type);
    cpt++;
  }
}

}  // namespace Xml
