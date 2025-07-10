/*
   This file is part of BAST.
   Copyright © CLEARSY 2022-2023
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

#include "exprWriter.h"
#include "predWriter.h"

namespace Xml {

    class PredWriterVisitor : public Pred::Visitor {
        public:
            void visitImplication(const Pred &lhs, const Pred &rhs){
                stream.OpenElement("Binary_Pred");
                stream.PushAttribute("op","=>");
                lhs.accept(*this);
                rhs.accept(*this);
                stream.CloseElement(); // Binary_Pred
            };
            void visitEquivalence(const Pred &lhs, const Pred &rhs){
                stream.OpenElement("Binary_Pred");
                stream.PushAttribute("op","<=>");
                lhs.accept(*this);
                rhs.accept(*this);
                stream.CloseElement(); // Binary_Pred
            };
            void visitExprComparison(Pred::ComparisonOp op, const Expr &lhs, const Expr &rhs){
                stream.OpenElement("Exp_Comparison");
                stream.PushAttribute("op", Pred::to_string(op).c_str());
                writeExpression(stream,typeInfos,lhs);
                writeExpression(stream,typeInfos,rhs);
                stream.CloseElement(); // Exp_Comparison
            };
            void visitNegation(const Pred &p){
                stream.OpenElement("Unary_Pred");
                stream.PushAttribute("op","not");
                p.accept(*this);
                stream.CloseElement(); // Unary_Pred
            };
            void visitConjunction(const std::vector<Pred> &vec){
                stream.OpenElement("Nary_Pred");
                stream.PushAttribute("op","&");
                for(auto &p : vec)
                    p.accept(*this);
                stream.CloseElement(); // Nary_Pred
            };
            void visitDisjunction(const std::vector<Pred> &vec){
                stream.OpenElement("Nary_Pred");
                stream.PushAttribute("op","or");
                for(auto &p : vec)
                    p.accept(*this);
                stream.CloseElement(); // Nary_Pred
            };
            void visitForall(const std::vector<TypedVar> &vars, const Pred &p){
                stream.OpenElement("Quantified_Pred");
                stream.PushAttribute("type","!");
                stream.OpenElement("Variables");
                for(auto &v : vars)
                    writeTypedVar(stream,typeInfos,v);
                stream.CloseElement(); // Variables
                stream.OpenElement("Body");
                p.accept(*this);
                stream.CloseElement(); // Body
                stream.CloseElement(); // Quantified_Pred
            };
            void visitExists(const std::vector<TypedVar> &vars, const Pred &p){
                stream.OpenElement("Quantified_Pred");
                stream.PushAttribute("type","#");
                stream.OpenElement("Variables");
                for(auto &v : vars)
                    writeTypedVar(stream,typeInfos,v);
                stream.CloseElement(); // Variables
                stream.OpenElement("Body");
                p.accept(*this);
                stream.CloseElement(); // Body
                stream.CloseElement(); // Quantified_Pred
            };
            void visitTrue(){
                stream.OpenElement("Nary_Pred");
                stream.PushAttribute("op","&");
                stream.CloseElement(); // Nary_Pred
            };
            void visitFalse(){
                stream.OpenElement("Nary_Pred");
                stream.PushAttribute("op","or");
                stream.CloseElement(); // Nary_Pred
            };
            PredWriterVisitor(tinyxml2::XMLPrinter &s,std::map<BType,unsigned int> &typeInfos):
                stream{s},
                typeInfos{typeInfos}
            {};
        private:
            tinyxml2::XMLPrinter &stream;
            std::map<BType,unsigned int> &typeInfos;
    };

    void writePredicate(tinyxml2::XMLPrinter &stream, std::map<BType,unsigned int> &typeInfos, const Pred &p){
        PredWriterVisitor v(stream, typeInfos);
        p.accept(v);
    }
}
