/*
   This file is part of BAST.
   Copyright © CLEARSY 2023
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

#include "btypePrinter.h"

using std::ostream;

class BTypePrinterVisitor : public BType::Visitor
{
public:
  BTypePrinterVisitor(ostream& s)
    : stream{ s } {};
  void visitINTEGER() { stream << "INTEGER"; }
  void visitBOOLEAN() { stream << "BOOLEAN"; }
  void visitFLOAT() { stream << "FLOAT"; }
  void visitREAL() { stream << "REAL"; }
  void visitSTRING() { stream << "STRING"; }
  void visitProductType(const BType& lhs, const BType& rhs)
  {
    stream << "(";
    lhs.accept(*this);
    stream << " * ";
    rhs.accept(*this);
    stream << ")";
  }
  void visitPowerType(const BType& ty)
  {
    stream << "POW(";
    ty.accept(*this);
    stream << ")";
  }
  void visitRecordType(const std::vector<std::pair<std::string, BType>>& fields)
  {
    bool first{ true };
    stream << "rec(";
    for (const auto& f : fields) {
      if (first) {
        first = false;
      } else {
        stream << ", ";
      }
      stream << f.first << " : ";
      f.second.accept(*this);
    }
    stream << ")";
  }

private:
  ostream& stream;
};

void
printType(ostream& stream, const BType& p)
{
  BTypePrinterVisitor visitor(stream);
  p.accept(visitor);
}
