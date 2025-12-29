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
#include <map>
#include <string>

namespace Xml {

using dict_t = std::map<const std::string, const tinyxml2::XMLElement *>;

static dict_t readSets(const tinyxml2::XMLElement *root) {
  dict_t result;
  auto build_map = [&result](const tinyxml2::XMLElement *parent) {
    if (parent == nullptr) {
      return;
    }
    for (const tinyxml2::XMLElement *set = parent->FirstChildElement("Set");
         set != nullptr; set = set->NextSiblingElement("Set")) {
      const tinyxml2::XMLElement *id = set->FirstChildElement("Id");
      const std::string key = std::string(id->Attribute("value"));
      result.insert({key, set});
    }
  };
  if (root->FirstChildElement("Sets") != nullptr) {
    // for .bxml, .ibxml
    build_map(root->FirstChildElement("Sets"));
  } else if (root->FirstChildElement("Define") != nullptr) {
    // for .poxml, .pog
    for (const tinyxml2::XMLElement *define = root->FirstChildElement("Define");
         define != nullptr; define = define->NextSiblingElement("Define")) {
      if (strcmp(define->Attribute("name"), "sets") == 0) {
        build_map(define);
        break;
      }
    }
  }
  return result;
}

static BType readType(const tinyxml2::XMLElement *dom, const dict_t &dict) {
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
      const std::string key = std::string(value);
      const auto it = dict.find(key);
      if (it == dict.end()) {
        throw BTypeReaderException("Type declaration \"" + key + "\" not found",
                                   dom->GetLineNum());
      }
      const tinyxml2::XMLElement *set = it->second;
      if (set->FirstChildElement("Id") == nullptr) {
        throw BTypeReaderException("Ill-formed type declaration \"" + key +
                                       "\": missing 'Id' child element",
                                   set->GetLineNum());
      }
      const tinyxml2::XMLElement *id = set->FirstChildElement("Id");
      if (id->Attribute("value") == nullptr) {
        throw BTypeReaderException("Ill-formed type declaration \"" + key +
                                       "\": missing 'value' attribute",
                                   id->GetLineNum());
      }
      std::string name = id->Attribute("value");
      const tinyxml2::XMLElement *enumeratedValues =
          set->FirstChildElement("Enumerated_Values");
      if (enumeratedValues == nullptr) {
        return BType::ABSTRACT_SET(name);
      } else {
        std::vector<std::string> values;
        for (const tinyxml2::XMLElement *ide =
                 enumeratedValues->FirstChildElement("Id");
             ide != nullptr; ide = ide->NextSiblingElement("Id")) {
          if (ide->Attribute("value") == nullptr) {
            throw BTypeReaderException("Ill-formed type declaration \"" + key +
                                           "\": missing 'value' attribute",
                                       ide->GetLineNum());
          }
          values.push_back(std::string(ide->Attribute("value")));
        }
        return BType::ENUMERATED_SET({name, values});
      }
    }
  } else if (0 == strcmp(tag, "Unary_Exp")) {
    assert(0 == strcmp(dom->Attribute("op"), "POW"));
    return BType::POW(readType(dom->FirstChildElement(), dict));
  } else if (0 == strcmp(tag, "Binary_Exp")) {
    assert(0 == strcmp(dom->Attribute("op"), "*"));
    const tinyxml2::XMLElement *fst{dom->FirstChildElement()};
    return BType::PROD(readType(fst, dict),
                       readType(fst->NextSiblingElement(), dict));
  } else if (0 == strcmp(tag, "Struct")) {
    std::vector<std::pair<std::string, BType>> fields;
    for (const tinyxml2::XMLElement *item =
             dom->FirstChildElement("Record_Item");
         nullptr != item; item = item->NextSiblingElement("Record_Item")) {
      fields.push_back({std::string(item->Attribute("label")),
                        readType(item->FirstChildElement(), dict)});
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
  const tinyxml2::XMLDocument *doc = dom->GetDocument();
  const tinyxml2::XMLElement *root = doc->RootElement();
  const dict_t setDict = readSets(root);

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
    typeInfos.push_back(readType(typ->FirstChildElement(), setDict));
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
