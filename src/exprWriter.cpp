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

#include <numeric>

#include "exprWriter.h"
#include "predWriter.h"
#include "exprDesc.h"

namespace Xml {
    unsigned int getTypRef(std::map<BType,unsigned int> &typeInfos, const BType &ty){
        auto it = typeInfos.find(ty);
        if(it == typeInfos.end()){
            unsigned int i = typeInfos.size();
            typeInfos.insert({ty,i});
            return i;
        } else {
            return it->second;
        }
    }

    void writeExprAttributes(
            const BType &type,
            const std::vector<std::string> &bxmlTag,
            tinyxml2::XMLPrinter &stream,
            std::map<BType,unsigned int> &typeInfos)
    {
        stream.PushAttribute("typref",std::to_string(getTypRef(typeInfos,type)).c_str());
        if(!bxmlTag.empty()){
            std::string result = 
                std::accumulate(std::next(bxmlTag.begin()), bxmlTag.end(),
                                bxmlTag[0],
                                [](std::string& a, const std::string &b) {
                                    return a + ","+ b;
                                });
            stream.PushAttribute("tag", result.c_str());
        }
    }

    void writeTypedVar(tinyxml2::XMLPrinter &stream, std::map<BType,unsigned int> &typeInfos, const TypedVar &v){
        stream.OpenElement("Id");
        stream.PushAttribute("value", v.name.prefix().c_str());
        switch(v.name.kind()){
            case VarName::Kind::NoSuffix:
                break;
            case VarName::Kind::WithSuffix:
                stream.PushAttribute("suffix", std::to_string(v.name.suffix()).c_str());
                break;
            case VarName::Kind::FreshId:
                assert(false);
                break;
            case VarName::Kind::Tmp:
                assert(false);
                break;
        }
        stream.PushAttribute("typref", std::to_string(getTypRef(typeInfos,v.type)).c_str());
        stream.CloseElement(); // Id
    }

    class ExprWriterVisitor : public Expr::Visitor {
        public:
            void visitConstant(const BType &type, const std::vector<std::string> &bxmlTag,Expr::Visitor::EConstant c){
                switch (c){
                    case Expr::Visitor::EConstant::MaxInt:
                        {
                            stream.OpenElement("Id");
                            stream.PushAttribute("value","MAXINT");
                            writeExprAttributes(type,bxmlTag,stream,typeInfos);
                            stream.CloseElement(); // Id
                            break;
                        }
                    case Expr::Visitor::EConstant::MinInt:
                        {
                            stream.OpenElement("Id");
                            stream.PushAttribute("value","MININT");
                            writeExprAttributes(type,bxmlTag,stream,typeInfos);
                            stream.CloseElement(); // Id
                            break;
                        }
                    case Expr::Visitor::EConstant::INTEGER:
                        {
                            stream.OpenElement("Id");
                            stream.PushAttribute("value","INTEGER");
                            writeExprAttributes(type,bxmlTag,stream,typeInfos);
                            stream.CloseElement(); // Id
                            break;
                        }
                    case Expr::Visitor::EConstant::NATURAL:
                        {
                            stream.OpenElement("Id");
                            stream.PushAttribute("value","NATURAL");
                            writeExprAttributes(type,bxmlTag,stream,typeInfos);
                            stream.CloseElement(); // Id
                            break;
                        }
                    case Expr::Visitor::EConstant::NATURAL1:
                        {
                            stream.OpenElement("Id");
                            stream.PushAttribute("value","NATURAL1");
                            writeExprAttributes(type,bxmlTag,stream,typeInfos);
                            stream.CloseElement(); // Id
                            break;
                        }
                    case Expr::Visitor::EConstant::INT:
                        {
                            stream.OpenElement("Id");
                            stream.PushAttribute("value","INT");
                            writeExprAttributes(type,bxmlTag,stream,typeInfos);
                            stream.CloseElement(); // Id
                            break;
                        }
                    case Expr::Visitor::EConstant::NAT:
                        {
                            stream.OpenElement("Id");
                            stream.PushAttribute("value","NAT");
                            writeExprAttributes(type,bxmlTag,stream,typeInfos);
                            stream.CloseElement(); // Id
                            break;
                        }
                    case Expr::Visitor::EConstant::NAT1:
                        {
                            stream.OpenElement("Id");
                            stream.PushAttribute("value","NAT1");
                            writeExprAttributes(type,bxmlTag,stream,typeInfos);
                            stream.CloseElement(); // Id
                            break;
                        }
                    case Expr::Visitor::EConstant::STRING:
                        {
                            stream.OpenElement("Id");
                            stream.PushAttribute("value","STRING");
                            writeExprAttributes(type,bxmlTag,stream,typeInfos);
                            stream.CloseElement(); // Id
                            break;
                        }
                    case Expr::Visitor::EConstant::BOOL:
                        {
                            stream.OpenElement("Id");
                            stream.PushAttribute("value","BOOL");
                            writeExprAttributes(type,bxmlTag,stream,typeInfos);
                            stream.CloseElement(); // Id
                            break;
                        }
                    case Expr::Visitor::EConstant::REAL:
                        {
                            stream.OpenElement("Id");
                            stream.PushAttribute("value","REAL");
                            writeExprAttributes(type,bxmlTag,stream,typeInfos);
                            stream.CloseElement(); // Id
                            break;
                        }
                    case Expr::Visitor::EConstant::FLOAT:
                        {
                            stream.OpenElement("Id");
                            stream.PushAttribute("value","FLOAT");
                            writeExprAttributes(type,bxmlTag,stream,typeInfos);
                            stream.CloseElement(); // Id
                            break;
                        }
                    case Expr::Visitor::EConstant::TRUE:
                        {
                            stream.OpenElement("Boolean_Literal");
                            stream.PushAttribute("value","TRUE");
                            writeExprAttributes(type,bxmlTag,stream,typeInfos);
                            stream.CloseElement(); // Boolean_Literal
                            break;
                        }
                    case Expr::Visitor::EConstant::FALSE:
                        {
                            stream.OpenElement("Boolean_Literal");
                            stream.PushAttribute("value","FALSE");
                            writeExprAttributes(type,bxmlTag,stream,typeInfos);
                            stream.CloseElement(); // Boolean_Literal
                            break;
                        }
                    case Expr::Visitor::EConstant::EmptySet:
                        {
                            stream.OpenElement("EmptySet");
                            writeExprAttributes(type,bxmlTag,stream,typeInfos);
                            stream.CloseElement(); // EmptySet
                            break;
                        }
                    case Expr::Visitor::EConstant::Successor:
                        {
                            stream.OpenElement("Id");
                            stream.PushAttribute("value","succ");
                            writeExprAttributes(type,bxmlTag,stream,typeInfos);
                            stream.CloseElement(); // Id
                            break;
                        }
                    case Expr::Visitor::EConstant::Predecessor:
                        {
                            stream.OpenElement("Id");
                            stream.PushAttribute("value","pred");
                            writeExprAttributes(type,bxmlTag,stream,typeInfos);
                            stream.CloseElement(); // Id
                            break;
                        }
                }
            }
            void visitIntegerLiteral(const BType &type, const std::vector<std::string> &bxmlTag,const std::string &i){
                stream.OpenElement("Integer_Literal");
                stream.PushAttribute("value", i.c_str());
                writeExprAttributes(type,bxmlTag,stream,typeInfos);
                stream.CloseElement(); // Integer_Literal
            }
            void visitStringLiteral(const BType &type, const std::vector<std::string> &bxmlTag,const std::string &b){
                stream.OpenElement("STRING_Literal");
                stream.PushAttribute("value", b.c_str());
                writeExprAttributes(type,bxmlTag,stream,typeInfos);
                stream.CloseElement(); // STRING_Literal
            }
            void visitRealLiteral(const BType &type, const std::vector<std::string> &bxmlTag,const Expr::Decimal &d){
                stream.OpenElement("Real_Literal");
                stream.PushAttribute("value", std::string(d.integerPart + "." + d.fractionalPart).c_str());
                writeExprAttributes(type,bxmlTag,stream,typeInfos);
                stream.CloseElement(); // Real_Literal
            }
            void visitIdent(const BType &type, const std::vector<std::string> &bxmlTag, const VarName &v){
                stream.OpenElement("Id");
                stream.PushAttribute("value", v.prefix().c_str());
                switch(v.kind()){
                    case VarName::Kind::NoSuffix:
                        break;
                    case VarName::Kind::WithSuffix:
                        stream.PushAttribute("suffix", std::to_string(v.suffix()).c_str());
                        break;
                    case VarName::Kind::FreshId:
                        assert(false);
                        break;
                    case VarName::Kind::Tmp:
                        assert(false);
                        break;
                }
                writeExprAttributes(type,bxmlTag,stream,typeInfos);
                stream.CloseElement(); // Id
            }
            void visitUnaryExpression(const BType &type, const std::vector<std::string> &bxmlTag,Expr::UnaryOp op,const Expr &e){
                stream.OpenElement("Unary_Exp");
                stream.PushAttribute("op", Expr::to_string(op).c_str());
                writeExprAttributes(type,bxmlTag,stream,typeInfos);
                e.accept(*this);
                stream.CloseElement(); // Unary_Exp
            }
            void visitBinaryExpression(const BType &type, const std::vector<std::string> &bxmlTag,Expr::BinaryOp op, const Expr &lhs, const Expr &rhs){
                stream.OpenElement("Binary_Exp");
                stream.PushAttribute("op", Expr::to_string(op).c_str());
                writeExprAttributes(type,bxmlTag,stream,typeInfos);
                lhs.accept(*this);
                rhs.accept(*this);
                stream.CloseElement(); // Binary_Exp
            }
            void visitTernaryExpression(const BType &type, const std::vector<std::string> &bxmlTag,Expr::TernaryOp op, const Expr &fst, const Expr &snd, const Expr &thd){
                stream.OpenElement("Ternary_Exp");
                stream.PushAttribute("op", Expr::to_string(op).c_str());
                writeExprAttributes(type,bxmlTag,stream,typeInfos);
                fst.accept(*this);
                snd.accept(*this);
                thd.accept(*this);
                stream.CloseElement(); // Ternary_Exp
            }
            void visitNaryExpression(const BType &type, const std::vector<std::string> &bxmlTag,Expr::NaryOp op, const std::vector<Expr> &vec){
                stream.OpenElement("Nary_Exp");
                stream.PushAttribute("op", Expr::to_string(op).c_str());
                writeExprAttributes(type,bxmlTag,stream,typeInfos);
                for(auto &e : vec)
                    e.accept(*this);
                stream.CloseElement(); // Nary_Exp
            }
            void visitBooleanExpression(const BType &type, const std::vector<std::string> &bxmlTag,const Pred &p){
                stream.OpenElement("Boolean_Exp");
                writeExprAttributes(type,bxmlTag,stream,typeInfos);
                writePredicate(stream,typeInfos,p);
                stream.CloseElement(); // Boolean_Exp
            }
            void visitRecord(const BType &type, const std::vector<std::string> &bxmlTag,const std::vector<std::pair<std::string,Expr>> &fds){
                stream.OpenElement("Record");
                writeExprAttributes(type,bxmlTag,stream,typeInfos);
                for(auto &pair : fds){
                    stream.OpenElement("Record_Item");
                    stream.PushAttribute("label", pair.first.c_str());
                    pair.second.accept(*this);
                    stream.CloseElement(); // Record_Item
                };
                stream.CloseElement(); // Record
            }
            void visitStruct(const BType &type, const std::vector<std::string> &bxmlTag,const std::vector<std::pair<std::string,Expr>> &fds){
                stream.OpenElement("Struct");
                writeExprAttributes(type,bxmlTag,stream,typeInfos);
                for(auto &pair : fds){
                    stream.OpenElement("Record_Item");
                    stream.PushAttribute("label", pair.first.c_str());
                    pair.second.accept(*this);
                    stream.CloseElement(); // Record_Item
                };
                stream.CloseElement(); // Struct
            }
            void visitQuantifiedExpr(const BType &type, const std::vector<std::string> &bxmlTag,Expr::QuantifiedOp op,const std::vector<TypedVar> vars,const Pred &cond, const Expr &body){
                stream.OpenElement("Quantified_Exp");
                stream.PushAttribute("type", Expr::to_string(op).c_str());
                writeExprAttributes(type,bxmlTag,stream,typeInfos);

                stream.OpenElement("Variables");
                for(auto &id : vars)
                    writeTypedVar(stream,typeInfos,id);
                stream.CloseElement(); // Variables

                stream.OpenElement("Pred");
                writePredicate(stream,typeInfos,cond);
                stream.CloseElement(); // Pred

                stream.OpenElement("Body");
                body.accept(*this);
                stream.CloseElement(); // Body

                stream.CloseElement(); // Quantified_Exp
            }
            void visitQuantifiedSet(const BType &type, const std::vector<std::string> &bxmlTag,const std::vector<TypedVar> vars, const Pred &cond){
                stream.OpenElement("Quantified_Set");
                writeExprAttributes(type,bxmlTag,stream,typeInfos);

                stream.OpenElement("Variables");
                for(auto &id : vars)
                    writeTypedVar(stream,typeInfos,id);
                stream.CloseElement(); // Variables

                stream.OpenElement("Body");
                writePredicate(stream,typeInfos,cond);
                stream.CloseElement(); // Body

                stream.CloseElement(); // Quantified_Set
            };

            void visitRecordUpdate(const BType &type, const std::vector<std::string> &bxmlTag, const Expr &rec, const std::string &label, const Expr &value){
                stream.OpenElement("Record_Update");
                stream.PushAttribute("label", label.c_str());
                writeExprAttributes(type,bxmlTag,stream,typeInfos);
                rec.accept(*this);
                value.accept(*this);
                stream.CloseElement(); // Record_Update
            }

            void visitRecordAccess(const BType &type, const std::vector<std::string> &bxmlTag, const Expr &rec, const std::string &label){
                stream.OpenElement("Record_Field_Access");
                stream.PushAttribute("label", label.c_str());
                writeExprAttributes(type,bxmlTag,stream,typeInfos);
                rec.accept(*this);
                stream.CloseElement(); // Record_Field_Access
            }

            ExprWriterVisitor(tinyxml2::XMLPrinter &s, std::map<BType,unsigned int> &typeInfos):
                stream{s},
                typeInfos{typeInfos}
            {};
        private:
            tinyxml2::XMLPrinter &stream;
            std::map<BType,unsigned int> &typeInfos;
    };

    void writeExpression(tinyxml2::XMLPrinter &stream, std::map<BType,unsigned int> &typeInfos, const Expr &p){
        ExprWriterVisitor v(stream,typeInfos);
        p.accept(v);
    }
}
