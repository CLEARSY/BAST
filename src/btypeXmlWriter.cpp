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
#include "btypeXmlWriter.h"

Xml::BTypeWriter::BTypeWriter(tinyxml2::XMLPrinter *printer)
    : m_printer(printer) {}

void Xml::BTypeWriter::visitINTEGER() {
  m_printer->OpenElement("Id");
  m_printer->PushAttribute("value", "INTEGER");
  m_printer->CloseElement();  // Id
}

void Xml::BTypeWriter::visitBOOLEAN() {
  m_printer->OpenElement("Id");
  m_printer->PushAttribute("value", "BOOLEAN");
  m_printer->CloseElement();  // Id
}
void Xml::BTypeWriter::visitFLOAT() {
  m_printer->OpenElement("Id");
  m_printer->PushAttribute("value", "FLOAT");
  m_printer->CloseElement();  // Id
}
void Xml::BTypeWriter::visitREAL() {
  m_printer->OpenElement("Id");
  m_printer->PushAttribute("value", "REAL");
  m_printer->CloseElement();  // Id
}
void Xml::BTypeWriter::visitSTRING() {
  m_printer->OpenElement("Id");
  m_printer->PushAttribute("value", "STRING");
  m_printer->CloseElement();  // Id
}
void Xml::BTypeWriter::visitProductType(const BType &lhs, const BType &rhs) {
  m_printer->OpenElement("Binary_Exp");
  m_printer->PushAttribute("op", "*");
  lhs.accept(*this);
  rhs.accept(*this);
  m_printer->CloseElement();  // Product_Type
}
void Xml::BTypeWriter::visitPowerType(const BType &ty) {
  m_printer->OpenElement("Unary_Exp");
  m_printer->PushAttribute("op", "POW");
  ty.accept(*this);
  m_printer->CloseElement();  // Power_Type
}
void Xml::BTypeWriter::visitRecordType(
    const std::vector<std::pair<std::string, BType>> &fields) {
  m_printer->OpenElement("Struct");
  for (const auto &f : fields) {
    m_printer->OpenElement("Record_Item");
    m_printer->PushAttribute("label", f.first.c_str());
    f.second.accept(*this);
    m_printer->CloseElement();  // Record_Item
  }
  m_printer->CloseElement();  // Struct
}
void Xml::BTypeWriter::visitAbstractSet(const BType::AbstractSet &type) {
  m_printer->OpenElement("AbstractSet");
  m_printer->PushAttribute("id", type.getName().c_str());
  m_printer->CloseElement();  // AbstractSet
}
void Xml::BTypeWriter::visitEnumeratedSet(const BType::EnumeratedSet &type) {
  m_printer->OpenElement("EnumeratedSet");
  m_printer->PushAttribute("id", type.getName().c_str());
  for (const auto &v : type.getContent()) {
    m_printer->OpenElement("EnumeratedValue");
    m_printer->PushAttribute("id", v.c_str());
    m_printer->CloseElement();  // EnumeratedValue
  }
  m_printer->CloseElement();  // EnumeratedSet
}
