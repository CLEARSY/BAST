/*
   This file is part of BAST.
   Copyright © CLEARSY 2022-2025
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

#include <numeric>
#include <string>
#include <vector>

using std::string;
using std::vector;

#include "exprWriter.h"
#include "predWriter.h"

using std::string;
using std::vector;

namespace Xml {

class PredWriterVisitor : public Pred::VisitorWithTags {
 public:
  void visitImplication(const Pred &lhs, const Pred &rhs,
                        const Pred::BXmlTags &bxmlTags) {
    stream.OpenElement("Binary_Pred");
    stream.PushAttribute("op", "=>");
    writeAttributes(bxmlTags);
    lhs.accept(*this);
    rhs.accept(*this);
    stream.CloseElement();  // Binary_Pred
  };
  void visitEquivalence(const Pred &lhs, const Pred &rhs,
                        const Pred::BXmlTags &bxmlTags) {
    stream.OpenElement("Binary_Pred");
    stream.PushAttribute("op", "<=>");
    writeAttributes(bxmlTags);
    lhs.accept(*this);
    rhs.accept(*this);
    stream.CloseElement();  // Binary_Pred
  };
  void visitExprComparison(Pred::ComparisonOp op, const Expr &lhs,
                           const Expr &rhs, const Pred::BXmlTags &bxmlTags) {
    stream.OpenElement("Exp_Comparison");
    stream.PushAttribute("op", Pred::to_string(op).c_str());
    writeAttributes(bxmlTags);
    writeExpression(stream, typeInfos, lhs);
    writeExpression(stream, typeInfos, rhs);
    stream.CloseElement();  // Exp_Comparison
  };
  void visitNegation(const Pred &p, const Pred::BXmlTags &bxmlTags) {
    stream.OpenElement("Unary_Pred");
    stream.PushAttribute("op", "not");
    writeAttributes(bxmlTags);
    p.accept(*this);
    stream.CloseElement();  // Unary_Pred
  };
  void visitConjunction(const std::vector<Pred> &vec,
                        const Pred::BXmlTags &bxmlTags) {
    stream.OpenElement("Nary_Pred");
    stream.PushAttribute("op", "&");
    writeAttributes(bxmlTags);
    for (auto &p : vec) p.accept(*this);
    stream.CloseElement();  // Nary_Pred
  };
  void visitDisjunction(const std::vector<Pred> &vec,
                        const Pred::BXmlTags &bxmlTags) {
    stream.OpenElement("Nary_Pred");
    stream.PushAttribute("op", "or");
    writeAttributes(bxmlTags);
    for (auto &p : vec) p.accept(*this);
    stream.CloseElement();  // Nary_Pred
  };
  void visitForall(const std::vector<TypedVar> &vars, const Pred &p,
                   const Pred::BXmlTags &bxmlTags) {
    stream.OpenElement("Quantified_Pred");
    stream.PushAttribute("type", "!");
    writeAttributes(bxmlTags);
    stream.OpenElement("Variables");
    for (auto &v : vars) writeTypedVar(stream, typeInfos, v);
    stream.CloseElement();  // Variables
    stream.OpenElement("Body");
    p.accept(*this);
    stream.CloseElement();  // Body
    stream.CloseElement();  // Quantified_Pred
  };
  void visitExists(const std::vector<TypedVar> &vars, const Pred &p,
                   const Pred::BXmlTags &bxmlTags) {
    stream.OpenElement("Quantified_Pred");
    stream.PushAttribute("type", "#");
    writeAttributes(bxmlTags);
    stream.OpenElement("Variables");
    for (auto &v : vars) writeTypedVar(stream, typeInfos, v);
    stream.CloseElement();  // Variables
    stream.OpenElement("Body");
    p.accept(*this);
    stream.CloseElement();  // Body
    stream.CloseElement();  // Quantified_Pred
  };
  void visitTrue(const Pred::BXmlTags &bxmlTags) {
    stream.OpenElement("Nary_Pred");
    stream.PushAttribute("op", "&");
    writeAttributes(bxmlTags);
    stream.CloseElement();  // Nary_Pred
  };
  void visitFalse(const Pred::BXmlTags &bxmlTags) {
    stream.OpenElement("Nary_Pred");
    stream.PushAttribute("op", "or");
    writeAttributes(bxmlTags);
    stream.CloseElement();  // Nary_Pred
  };
  PredWriterVisitor(tinyxml2::XMLPrinter &s, TypeMap_t &typeInfos)
      : stream{s}, typeInfos{typeInfos} {};

 private:
  tinyxml2::XMLPrinter &stream;
  TypeMap_t &typeInfos;

  void writeAttributes(const Pred::BXmlTags &bxmlTag) {
    if (!bxmlTag.empty()) {
      bool first = true;
      std::string result;
      for (const auto &tag : bxmlTag) {
        if (!first) {
          result += ",";
        }
        result += tag;
        first = false;
      }
      stream.PushAttribute("tag", result.c_str());
    }
  }
};

void writePredicate(tinyxml2::XMLPrinter &stream, TypeMap_t &typeInfos,
                    const Pred &p) {
  PredWriterVisitor v(stream, typeInfos);
  p.accept(v);
}
}  // namespace Xml
