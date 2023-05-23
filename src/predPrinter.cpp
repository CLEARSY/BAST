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

#include "predPrinter.h"

#include "exprPrinter.h"

using std::ostream;

class PredPrinterVisitor : public Pred::Visitor {
        public:
            void visitImplication(const Pred &lhs, const Pred &rhs){
                stream << "(";
                lhs.accept(*this);
                stream << " => ";
                rhs.accept(*this);
                stream << ")";
            };
            void visitEquivalence(const Pred &lhs, const Pred &rhs){
                stream << "(";
                lhs.accept(*this);
                stream << " <=> ";
                rhs.accept(*this);
                stream << ")";
            };
            void visitExprComparison(Pred::ComparisonOp op, const Expr &lhs, const Expr &rhs){
                printExpression(stream,lhs);
                stream << " " << Pred::to_string(op) << " ";
                printExpression(stream, rhs);
            };
            void visitNegation(const Pred &p){
                stream << "not( ";
                p.accept(*this);
                stream << " )";
            };
            void visitConjunction(const std::vector<Pred> &vec){
                stream << "( ";
                bool first {true};
                for(auto &p : vec) {
                    if (first) {
                        first = false;
                    } else {
                        stream << tab << "& ";
                    }
                    p.accept(*this);
                    stream << std::endl;
                }
                stream << ")";
            };
            void visitDisjunction(const std::vector<Pred> &vec){
                stream << "( ";
                bool first {true};
                for(auto &p : vec) {
                    if (first) {
                        first = false;
                    } else {
                        stream << tab << "or ";
                    }
                    p.accept(*this);
                    stream << std::endl;
                }
                stream << ")";
            };
            void visitForall(const std::vector<TypedVar> &vars, const Pred &p){
                stream << "!(";
                bool first {true};
                for(auto &v : vars) {
                    if (first) {
                        first = false;
                    } else {
                        stream << ", ";
                    }
                    printTypedVar(stream, v);
                }
                stream << ").(";
                p.accept(*this);
                stream << ")";
            };
            void visitExists(const std::vector<TypedVar> &vars, const Pred &p){
                stream << "#(";
                bool first {true};
                for(auto &v : vars) {
                    if (first) {
                        first = false;
                    } else {
                        stream << ", ";
                    }
                    printTypedVar(stream, v);
                }
                stream << ").(";
                p.accept(*this);
                stream << ")";
            };
            void visitTrue(){
                stream << "btrue";
            };
            void visitFalse(){
                stream << "bfalse";
            };
            PredPrinterVisitor(ostream &s)
            : stream{s},
              margin{""}
            {};
        private:
            ostream &stream;
            std::string margin;
            static const std::string tab;
    };
const std::string PredPrinterVisitor::tab =  std::string("  ");

void printPredicate(ostream &stream, const Pred &p) {
    PredPrinterVisitor v(stream);
    p.accept(v);
}
