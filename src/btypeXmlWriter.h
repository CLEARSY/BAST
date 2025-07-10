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
#ifndef BTYPE_XML_WRITER_H
#define BTYPE_XML_WRITER_H

#include <string>
#include <vector>

#include "btype.h"
#include "tinyxml2.h"

namespace Xml {

class BTypeWriter : public BType::Visitor {
 public:
  explicit BTypeWriter(tinyxml2::XMLPrinter *printer);

  void visitINTEGER() override;
  void visitBOOLEAN() override;
  void visitFLOAT() override;
  void visitREAL() override;
  void visitSTRING() override;
  void visitProductType(const BType &lhs, const BType &rhs) override;
  void visitPowerType(const BType &ty) override;
  void visitRecordType(
      const std::vector<std::pair<std::string, BType>> &fields) override;
  void visitAbstractSet(const BType::AbstractSet &type) override;
  void visitEnumeratedSet(const BType::EnumeratedSet &type) override;

 private:
  tinyxml2::XMLPrinter *m_printer;
};

}  // namespace Xml

#endif  // BTYPE_XML_WRITER_H
