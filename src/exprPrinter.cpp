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

#include "exprPrinter.h"

#include "expr.h"
#include "predPrinter.h"

using std::ostream;

void printTypedVar(ostream &stream, const TypedVar &v) {
    if(v.name.kind() == VarName::Kind::NoSuffix) {
        stream << v.name.prefix();
    }
    else if (v.name.kind() == VarName::Kind::WithSuffix) {
        stream << v.name.prefix() << '$' << v.name.suffix();
    }
    else {
        stream << "ERROR";
    }
}

class ExprPrinterVisitor : public Expr::Visitor {
  public:
    ExprPrinterVisitor(ostream &s) : stream{s} {};

    virtual void visitConstant(const BType &type,
                               const std::vector<std::string> &bxmlTag,
                               EConstant c) {
        switch (c) {
        case Expr::Visitor::EConstant::MaxInt: {
            stream << "MAXINT";
            break;
        }
        case Expr::Visitor::EConstant::MinInt: {
            stream << "MAXINT";
            break;
        }
        case Expr::Visitor::EConstant::INTEGER: {
            stream << "INTEGER";
            break;
        }
        case Expr::Visitor::EConstant::NATURAL: {
            stream << "NATURAL";
            break;
        }
        case Expr::Visitor::EConstant::NATURAL1: {
            stream << "NATURAL1";
            break;
        }
        case Expr::Visitor::EConstant::INT: {
            stream << "INT";
            break;
        }
        case Expr::Visitor::EConstant::NAT: {
            stream << "NAT";
            break;
        }
        case Expr::Visitor::EConstant::NAT1: {
            stream << "NA1";
            break;
        }
        case Expr::Visitor::EConstant::BOOL: {
            stream << "BOOL";
            break;
        }
        case Expr::Visitor::EConstant::STRING: {
            stream << "STRING";
            break;
        }
        case Expr::Visitor::EConstant::REAL: {
            stream << "REAL";
            break;
        }
        case Expr::Visitor::EConstant::FLOAT: {
            stream << "FLOAT";
            break;
        }
        case Expr::Visitor::EConstant::TRUE: {
            stream << "TRUE";
            break;
        }
        case Expr::Visitor::EConstant::FALSE: {
            stream << "FALSE";
            break;
        }
        case Expr::Visitor::EConstant::EmptySet: {
            stream << "{}";
            break;
        }
        case Expr::Visitor::EConstant::Successor: {
            stream << "succ";
            break;
        }
        case Expr::Visitor::EConstant::Predecessor: {
            stream << "pred";
            break;
        }
        }
    }
    virtual void visitIdent(const BType &type,
                            const std::vector<std::string> &bxmlTag,
                            const VarName &b) {
        stream << b.prefix();
        switch (b.kind()) {
        case VarName::Kind::NoSuffix:
            break;
        case VarName::Kind::WithSuffix:
            stream << std::to_string(b.suffix());
            break;
        case VarName::Kind::FreshId:
            assert(false);
            break;
        case VarName::Kind::Tmp:
            assert(false);
            break;
        }
    }
    virtual void visitIntegerLiteral(const BType &type,
                                     const std::vector<std::string> &bxmlTag,
                                     const std::string &i) {
        stream << i; // Integer_Literal
    }
    virtual void visitStringLiteral(const BType &type,
                                    const std::vector<std::string> &bxmlTag,
                                    const std::string &b) {
        stream << '"' << b << '"';
    }
    virtual void visitRealLiteral(const BType &type,
                                  const std::vector<std::string> &bxmlTag,
                                  const Expr::Decimal &d) {
        stream << d.integerPart << "." << d.fractionalPart;
    }
    virtual void visitUnaryExpression(const BType &type,
                                      const std::vector<std::string> &bxmlTag,
                                      Expr::UnaryOp op, const Expr &e){
        stream << "(" << Expr::to_string(op) << " ";
        e.accept(*this);
        stream << ")";
    }
    virtual void visitBinaryExpression(const BType &type,
                                       const std::vector<std::string> &bxmlTag,
                                       Expr::BinaryOp op, const Expr &lhs,
                                       const Expr &rhs) {
        stream << "(";
        lhs.accept(*this);
        stream << " " << Expr::to_string(op) << " ";
        rhs.accept(*this);
        stream << ")";
   }
    virtual void visitTernaryExpression(const BType &type,
                                        const std::vector<std::string> &bxmlTag,
                                        Expr::TernaryOp op, const Expr &fst,
                                        const Expr &snd, const Expr &thd) {
        stream << "(" << Expr::to_string(op) << " ";
        fst.accept(*this);
        stream << " ";
        snd.accept(*this);
        stream << " ";
        thd.accept(*this);
        stream << ")";
    }
    virtual void visitNaryExpression(const BType &type,
                                     const std::vector<std::string> &bxmlTag,
                                     Expr::NaryOp op,
                                     const std::vector<Expr> &vec) {
        stream << (op == Expr::NaryOp::Sequence ? '[' : '{') << " ";
        bool first {true};
        for(const auto& arg: vec) {
            if (first) {
                first = false;
            }
            else {
                stream << ", ";
            }
            arg.accept(*this);
        }
        stream << (op == Expr::NaryOp::Sequence ? ']' : '}');
    }
    virtual void visitBooleanExpression(const BType &type,
                                        const std::vector<std::string> &bxmlTag,
                                        const Pred &p) {
        stream << "bool(";
        printPredicate(stream, p);
        stream << ")";
    }

    virtual void
    visitRecord(const BType &type, const std::vector<std::string> &bxmlTag,
                const std::vector<std::pair<std::string, Expr>> &fds) {
        stream << "rec(";
        bool first {true};
        for(const auto &pair: fds) {
            if(first) {
                first = false;
            }
            else {
                stream << ",";
            }
            stream << pair.first << ": ";
            pair.second.accept(*this);
        }
        stream << ")";
    }
    virtual void
    visitStruct(const BType &type, const std::vector<std::string> &bxmlTag,
                const std::vector<std::pair<std::string, Expr>> &fds) {
        stream << "struct(";
        bool first {true};
        for(const auto &pair: fds) {
            if(first) {
                first = false;
            }
            else {
                stream << ",";
            }
            stream << pair.first << ": ";
            pair.second.accept(*this);
        }
        stream << ")";

    }
    virtual void visitQuantifiedExpr(const BType &type,
                                     const std::vector<std::string> &bxmlTag,
                                     Expr::QuantifiedOp op,
                                     const std::vector<TypedVar> vars,
                                     const Pred &cond, const Expr &body) {
        stream << Expr::to_string(op);
        stream << "(";
        bool first = true;
        for(const auto &var: vars) {
            if(first) {
                first = false;
            } else {
                stream << ",";
            }
            printTypedVar(stream, var);
        }
        stream << ").(";
        printPredicate(stream, cond);
        stream << " | ";
        body.accept(*this);
        stream << ")";
    }
    virtual void visitQuantifiedSet(const BType &type,
                                    const std::vector<std::string> &bxmlTag,
                                    const std::vector<TypedVar> vars,
                                    const Pred &cond) {
        stream << "{";
        bool first = true;
        for(const auto &var: vars) {
            if(first) {
                first = false;
            } else {
                stream << ",";
            }
            printTypedVar(stream, var);
        }
        stream << " | ";
        printPredicate(stream, cond);
        stream << "}";
    }
    virtual void visitRecordUpdate(const BType &type,
                                   const std::vector<std::string> &bxmlTag,
                                   const Expr &rec, const std::string &label,
                                   const Expr &value) {
        rec.accept(*this);
        stream << "(|" << label;
        rec.accept(*this);
        stream << "<-" << label;
        value.accept(*this);
        stream << "|)" << label;
   }
    virtual void visitRecordAccess(const BType &type,
                                   const std::vector<std::string> &bxmlTag,
                                   const Expr &rec, const std::string &label) {
        rec.accept(*this);
        stream << '\'' << label;
   }

  private:
    ostream &stream;
};

void printExpression(ostream &stream, const Expr &p) {
    ExprPrinterVisitor visitor(stream);
    p.accept(visitor);
}
