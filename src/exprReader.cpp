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

#include "/opt/homebrew/include/tinyxml2.h"

#include "exprReader.h"
#include "predReader.h"
#include "exprDesc.h"
#include <map>
#include <utility>
#include <cctype>

static bool strEqCaseInsensitive(const char *str, const char* uppercase);

static void splitDecimal(const char* string, std::string *integralPart, std::string *decimalPart);

namespace Xml {
    TypedVar VarNameFromId(const tinyxml2::XMLElement *id, const std::vector<BType> &typeInfos){
        if(0 == strcmp(id->Name(), "Id")){
            const char * prefix {nullptr};
            if(tinyxml2::XML_SUCCESS != id->QueryStringAttribute("value", &prefix))
                throw ExprReaderException("value attribute is empty.",id->GetLineNum());
            unsigned int typref;
            if(tinyxml2::XML_SUCCESS != id->QueryUnsignedAttribute("typref", &typref))
                throw ExprReaderException("typref attribute error.",id->GetLineNum());

            if(nullptr == id->Attribute("suffix")){
                return {VarName::makeVarWithoutSuffix(prefix),typeInfos[typref]};
            } else {
                int i;
                if(tinyxml2::XML_SUCCESS != id->QueryIntAttribute("suffix", &i))
                    throw ExprReaderException("suffix attribute error.",id->GetLineNum());
                if(i == 0)
                    return {VarName::makeVarWithoutSuffix(prefix),typeInfos[typref]}; // xx$0 may occur in while invariant, the suffix '0' could be removed in the ibxml step
                else
                    return {VarName::makeVar(prefix,i),typeInfos[typref]};
            }
        } else if(0 == strcmp(id->Name(), "Fresh_Id")){
            const char *prefix;
            if(tinyxml2::XML_SUCCESS != id->QueryStringAttribute("ref", &prefix))
                throw ExprReaderException("ref attribute error.",id->GetLineNum());
            unsigned int typref;
            if(tinyxml2::XML_SUCCESS != id->QueryUnsignedAttribute("typref", &typref))
                throw ExprReaderException("typref attribute error.",id->GetLineNum());
            return {VarName::makeFreshId(prefix),typeInfos[typref]};
        } else {
            throw ExprReaderException("Id element expected.",id->GetLineNum());
        }
        assert(false); // unreachable
    };

    const std::map<std::string, Expr::EKind> etags = {
        {"Binary_Exp", Expr::EKind::BinaryExpr},
        {"Nary_Exp", Expr::EKind::NaryExpr},
        {"Boolean_Literal",Expr::EKind::TRUE},
        {"Boolean_Exp", Expr::EKind::BooleanExpr},
        {"EmptySet", Expr::EKind::EmptySet},
        {"EmptySeq", Expr::EKind::EmptySet},
        {"Id", Expr::EKind::Id},
        {"Fresh_Id", Expr::EKind::Id},
        {"Integer_Literal", Expr::EKind::IntegerLiteral},
        {"Quantified_Exp", Expr::EKind::QuantifiedExpr},
        {"Quantified_Set", Expr::EKind::QuantifiedSet},
        {"String_Literal", Expr::EKind::StringLiteral},
        {"Unary_Exp",  Expr::EKind::UnaryExpr},
        {"Struct", Expr::EKind::Struct},
        {"Record", Expr::EKind::Record},
        {"Real_Literal", Expr::EKind::RealLiteral},
        {"STRING_Literal", Expr::EKind::StringLiteral},
        {"Ternary_Exp",  Expr::EKind::TernaryExpr},
        {"Record_Field_Access",Expr::EKind::Record_Field_Access},
        {"Record_Update",Expr::EKind::Record_Field_Update},
    };
    const std::map<std::string, Expr::UnaryOp> unaryExpOp = {
        {"card", Expr::UnaryOp::Cardinality},
        {"dom", Expr::UnaryOp::Domain},
        {"ran", Expr::UnaryOp::Range},
        {"POW", Expr::UnaryOp::Subsets},
        {"POW1",Expr::UnaryOp::Non_Empty_Subsets},
        {"FIN", Expr::UnaryOp::Finite_Subsets},
        {"FIN1",Expr::UnaryOp::Non_Empty_Finite_Subsets},
        {"union", Expr::UnaryOp::Union},
        {"inter", Expr::UnaryOp::Intersection},
        {"seq", Expr::UnaryOp::Sequences},
        {"seq1", Expr::UnaryOp::Non_Empty_Sequences},
        {"iseq", Expr::UnaryOp::Injective_Sequences},
        {"iseq1", Expr::UnaryOp::Non_Empty_Injective_Sequences},
        {"-i",Expr::UnaryOp::IMinus},
        {"-r",Expr::UnaryOp::RMinus},
        {"~", Expr::UnaryOp::Inverse},
        {"size", Expr::UnaryOp::Size},
        {"perm", Expr::UnaryOp::Permutations},
        {"first", Expr::UnaryOp::First},
        {"last", Expr::UnaryOp::Last},
        {"id", Expr::UnaryOp::Identity},
        {"closure", Expr::UnaryOp::Closure},
        {"closure1", Expr::UnaryOp::Transitive_Closure},
        {"tail", Expr::UnaryOp::Tail},
        {"front", Expr::UnaryOp::Front},
        {"rev", Expr::UnaryOp::Reverse},
        {"conc", Expr::UnaryOp::Concatenation},
        {"rel", Expr::UnaryOp::Rel},
        {"fnc", Expr::UnaryOp::Fnc},
        {"real", Expr::UnaryOp::Real},
        {"floor", Expr::UnaryOp::Floor},
        {"ceiling", Expr::UnaryOp::Ceiling},
        {"imin", Expr::UnaryOp::IMinimum},
        {"imax", Expr::UnaryOp::IMaximum},
        {"rmin", Expr::UnaryOp::RMinimum},
        {"rmax", Expr::UnaryOp::RMaximum},
        {"tree", Expr::UnaryOp::Tree},
        {"btree", Expr::UnaryOp::Btree},
        {"top", Expr::UnaryOp::Top},
        {"sons", Expr::UnaryOp::Sons},
        {"prefix", Expr::UnaryOp::Prefix},
        {"postfix", Expr::UnaryOp::Postfix},
        {"sizet", Expr::UnaryOp::Sizet},
        {"mirror", Expr::UnaryOp::Mirror},
        {"left", Expr::UnaryOp::Left},
        {"right", Expr::UnaryOp::Right},
        {"infix", Expr::UnaryOp::Infix},
        {"bin", Expr::UnaryOp::Bin}
    };

    const std::map<std::string, Expr::BinaryOp> binaryExpOp = {
        {",",Expr::BinaryOp::Mapplet},
        {"*i", Expr::BinaryOp::IMultiplication},
        {"*f", Expr::BinaryOp::FMultiplication},
        {"*r", Expr::BinaryOp::RMultiplication},
        {"*s", Expr::BinaryOp::Cartesian_Product},
        {"**i", Expr::BinaryOp::IExponentiation},
        {"**r", Expr::BinaryOp::RExponentiation},
        {"+i", Expr::BinaryOp::IAddition},
        {"+r", Expr::BinaryOp::RAddition},
        {"+f", Expr::BinaryOp::FAddition},
        {"+->", Expr::BinaryOp::Partial_Functions},
        {"+->>", Expr::BinaryOp::Partial_Surjections},
        {"-i", Expr::BinaryOp::ISubtraction},
        {"-r", Expr::BinaryOp::RSubtraction},
        {"-f", Expr::BinaryOp::FSubtraction},
        {"-s", Expr::BinaryOp::Set_Difference},
        {"-->", Expr::BinaryOp::Total_Functions},
        {"-->>", Expr::BinaryOp::Total_Surjections},
        {"->", Expr::BinaryOp::Head_Insertion},
        {"..", Expr::BinaryOp::Interval},
        {"/i", Expr::BinaryOp::IDivision},
        {"/r", Expr::BinaryOp::RDivision},
        {"/f", Expr::BinaryOp::FDivision},
        {"/\\", Expr::BinaryOp::Intersection},
        {"/|\\", Expr::BinaryOp::Head_Restriction},
        {";",Expr::BinaryOp::Composition},
        {"<+", Expr::BinaryOp::Surcharge},
        {"<->", Expr::BinaryOp::Relations},
        {"<-", Expr::BinaryOp::Tail_Insertion},
        {"<<|", Expr::BinaryOp::Domain_Subtraction},
        {"<|", Expr::BinaryOp::Domain_Restriction},
        {">+>", Expr::BinaryOp::Partial_Injections},
        {">->", Expr::BinaryOp::Total_Injections},
        {">+>>", Expr::BinaryOp::Partial_Bijections},
        {">->>", Expr::BinaryOp::Total_Bijections},
        {"><", Expr::BinaryOp::Direct_Product},
        {"||",Expr::BinaryOp::Parallel_Product},
        {"\\/",Expr::BinaryOp::Union},
        {"\\|/",Expr::BinaryOp::Tail_Restriction},
        {"^",Expr::BinaryOp::Concatenation},
        {"mod",Expr::BinaryOp::Modulo},
        {"|->",Expr::BinaryOp::Mapplet},
        {"|>",Expr::BinaryOp::Range_Restriction},
        {"|>>",Expr::BinaryOp::Range_Subtraction},
        {"[", Expr::BinaryOp::Image},
        {"(",Expr::BinaryOp::Application},
        {"prj1", Expr::BinaryOp::First_Projection},
        {"prj2", Expr::BinaryOp::Second_Projection},
        {"iterate", Expr::BinaryOp::Iteration},
        {"const", Expr::BinaryOp::Const},
        {"rank", Expr::BinaryOp::Rank},
        {"father", Expr::BinaryOp::Father},
        {"subtree", Expr::BinaryOp::Subtree},
        {"arity", Expr::BinaryOp::Arity}
    };

    const std::map<std::string, Expr::TernaryOp> ternaryExpOp = {
        {"bin",Expr::TernaryOp::Bin},
        {"son",Expr::TernaryOp::Son},
    };

    const std::map<std::string, Expr::NaryOp> naryExpOp = {
        {"[",Expr::NaryOp::Sequence},
        {"{",Expr::NaryOp::Set}
    };

    const std::map<std::string, Expr::QuantifiedOp> quantifiedExprOp = {
        {"%",Expr::QuantifiedOp::Lambda},
        {"rSIGMA",Expr::QuantifiedOp::RSum},
        {"iSIGMA",Expr::QuantifiedOp::ISum},
        {"rPI",Expr::QuantifiedOp::RProduct},
        {"iPI",Expr::QuantifiedOp::IProduct},
        {"INTER",Expr::QuantifiedOp::Intersection},
        {"UNION",Expr::QuantifiedOp::Union},
        {"rPI",Expr::QuantifiedOp::RProduct},
        {"rSIGMA",Expr::QuantifiedOp::RSum}
    };

    const std::map<std::string, Expr::EKind> constantExpr = {
        {"MAXINT",Expr::EKind::MaxInt},
        {"MININT",Expr::EKind::MinInt},
        {"INTEGER",Expr::EKind::INTEGER},
        {"NATURAL",Expr::EKind::NATURAL},
        {"NATURAL1",Expr::EKind::NATURAL1},
        {"INT",Expr::EKind::INT},
        {"NAT",Expr::EKind::NAT},
        {"NAT1",Expr::EKind::NAT1},
        {"STRING",Expr::EKind::STRING},
        {"BOOL",Expr::EKind::BOOL},
        {"REAL",Expr::EKind::REAL},
        {"FLOAT",Expr::EKind::FLOAT},
        {"TRUE",Expr::EKind::TRUE},
        {"FALSE",Expr::EKind::FALSE},
        {"succ", Expr::EKind::Successor},
        {"pred", Expr::EKind::Predecessor}
    };

    Expr readExpression(const tinyxml2::XMLElement *dom, const std::vector<BType> &typeInfos){
        if (nullptr == dom)
            throw ExprReaderException("Null dom element.",-1);

        std::string tagName = dom->Name();
        unsigned int typref;
        if(tinyxml2::XML_SUCCESS != dom->QueryUnsignedAttribute("typref", &typref))
            throw ExprReaderException("typref attribute error.", dom->GetLineNum());
        BType type = typeInfos[typref];
        std::vector<std::string> bxmlTag;
        auto _bxmlTag {dom->Attribute("tag")};
        if(nullptr != _bxmlTag && 0 != strcmp("", _bxmlTag))
            bxmlTag.push_back(_bxmlTag);

        auto it = etags.find(tagName);
        if(it == etags.end())
            throw ExprReaderException("Unexpected tag '" + tagName + "'.", dom->GetLineNum());

        switch(it->second){
            case Expr::EKind::BinaryExpr:
                {
                    auto op = dom->Attribute("op");
                    auto it = binaryExpOp.find(op);
                    if(it == binaryExpOp.end())
                        throw ExprReaderException
                            ("Unknown binary expression operator '" + std::string(op) + "'.", dom->GetLineNum());
                    const tinyxml2::XMLElement *fst {dom->FirstChildElement()};
                    const tinyxml2::XMLElement * snd {fst->NextSiblingElement()};
                    Expr lhs = readExpression(fst,typeInfos);
                    Expr rhs = readExpression(snd,typeInfos);
                    return Expr::makeBinaryExpr(it->second,std::move(lhs),std::move(rhs),type,bxmlTag);
                }
            case Expr::EKind::TernaryExpr:
                {
                    auto op = dom->Attribute("op");
                    auto it = ternaryExpOp.find(op);
                    if(it == ternaryExpOp.end())
                        throw ExprReaderException
                            ("Unknown ternary expression operator '" + std::string(op) + "'.", dom->GetLineNum());
                    const tinyxml2::XMLElement * fst = dom->FirstChildElement();
                    const tinyxml2::XMLElement * snd = fst->NextSiblingElement();
                    const tinyxml2::XMLElement * thd = snd->NextSiblingElement();
                    Expr efst = readExpression(fst,typeInfos);
                    Expr esnd = readExpression(snd,typeInfos);
                    Expr ethd = readExpression(thd,typeInfos);
                    return Expr::makeTernaryExpr(it->second,std::move(efst),std::move(esnd),std::move(ethd),type,bxmlTag);
                }
            case Expr::EKind::NaryExpr:
                {
                    auto op = dom->Attribute("op");
                    auto it = naryExpOp.find(op);
                    if(it == naryExpOp.end())
                        throw ExprReaderException
                            ("Unknown n-ary expression operator '" + std::string(op) + "'.",dom->GetLineNum());
                    std::vector<Expr> lst;
                    const tinyxml2::XMLElement * ce = dom->FirstChildElement();
                    while (nullptr != ce) {
                        lst.push_back(readExpression(ce,typeInfos));
                        ce = ce->NextSiblingElement();
                    }
                    return Expr::makeNaryExpr(it->second,std::move(lst),type, bxmlTag);
                }
            case Expr::EKind::BooleanExpr:
                {
                    return Expr::makeBooleanExpr(
                            readPredicate(dom->FirstChildElement(),typeInfos),
                            bxmlTag );
                }
            case Expr::EKind::EmptySet:
                {
                    return Expr::makeEmptySet(type, bxmlTag);
                }
            case Expr::EKind::Id:
                {
                    if(tagName == "Fresh_Id"){
                        TypedVar tv = VarNameFromId(dom,typeInfos);
                        return Expr::makeIdent(tv.name, tv.type, bxmlTag);
                    }

                    auto v = dom->Attribute("value");
                    auto it = constantExpr.find(v);

                    if(it == constantExpr.end()){
                        TypedVar tv = VarNameFromId(dom,typeInfos);
                        return Expr::makeIdent(tv.name, tv.type, bxmlTag);
                    }

                    switch (it->second){
                        case Expr::EKind::MaxInt:
                            return Expr::makeMaxInt(bxmlTag);
                        case Expr::EKind::MinInt:
                            return Expr::makeMinInt(bxmlTag);
                        case Expr::EKind::INTEGER:
                            return Expr::makeINTEGER(bxmlTag);
                        case Expr::EKind::NATURAL:
                            return Expr::makeNATURAL(bxmlTag);
                        case Expr::EKind::NATURAL1:
                            return Expr::makeNATURAL1(bxmlTag);
                        case Expr::EKind::INT:
                            return Expr::makeINT(bxmlTag);
                        case Expr::EKind::NAT:
                            return Expr::makeNAT(bxmlTag);
                        case Expr::EKind::NAT1:
                            return Expr::makeNAT1(bxmlTag);
                        case Expr::EKind::STRING:
                            return Expr::makeSTRING(bxmlTag);
                        case Expr::EKind::BOOL:
                            return Expr::makeBOOL(bxmlTag);
                        case Expr::EKind::REAL:
                            return Expr::makeREAL(bxmlTag);
                        case Expr::EKind::FLOAT:
                            return Expr::makeFLOAT(bxmlTag);
                        case Expr::EKind::TRUE:
                            return Expr::makeTRUE(bxmlTag);
                        case Expr::EKind::FALSE:
                            return Expr::makeFALSE(bxmlTag);
                        case Expr::EKind::Successor:
                            return Expr::makeSuccessor(type,bxmlTag);
                        case Expr::EKind::Predecessor:
                            return Expr::makePredecessor(type,bxmlTag);
                        default:
                            assert(false); // unreachable
                    };
                }
            case Expr::EKind::IntegerLiteral:
                {
                    return Expr::makeInteger(dom->Attribute("value"),bxmlTag);
                }
            case Expr::EKind::RealLiteral:
                {
                    const char * val {dom->Attribute("value")};
                    std::vector<std::string> lst;
                    std::string integralPart;
                    std::string decimalPart;
                    splitDecimal(val, &integralPart, &decimalPart);
                    if(lst.size() == 1 || lst.size() == 2){
                        std::string integerPart = lst[0];
                        if(lst.size() == 1){
                            return Expr::makeReal(Expr::Decimal(integerPart),bxmlTag);
                        } else /* lst.size() == 2 */ {
                            std::string decimalPart = lst[1];
                            return Expr::makeReal(Expr::Decimal(integerPart,decimalPart),bxmlTag);
                        }
                    } else {
                        throw ExprReaderException("Incorrect decimal value ("+ std::string(dom->Attribute("value")) + ").",dom->GetLineNum());
                    }
                }
            case Expr::EKind::StringLiteral:
                {
                    return Expr::makeString(dom->Attribute("value"),bxmlTag);
                }
            case Expr::EKind::QuantifiedExpr:
                {
                    auto op = dom->Attribute("type");
                    auto it = quantifiedExprOp.find(op);
                    if(it == quantifiedExprOp.end())
                        throw ExprReaderException
                            ("Unknown type of quantified expression '" + std::string(op) + "'.",dom->GetLineNum());

                    const tinyxml2::XMLElement * vars = dom->FirstChildElement("Variables");
                    if(nullptr == vars)
                        throw ExprReaderException
                            ("The 'Quantified_Exp' element is missing some 'Variables' child.",dom->GetLineNum());
                    std::vector<TypedVar> ids;
                    for(    const tinyxml2::XMLElement * ce = vars->FirstChildElement();
                            nullptr != ce;
                            ce = ce->NextSiblingElement() )
                    {
                        ids.push_back(VarNameFromId(ce,typeInfos));
                    }
                    Pred pre = readPredicate(dom->FirstChildElement("Pred")->FirstChildElement(),typeInfos);
                    Expr body = readExpression(dom->FirstChildElement("Body")->FirstChildElement(),typeInfos);
                    return Expr::makeQuantifiedExpr(it->second,ids,std::move(pre),std::move(body),type,bxmlTag );
                }
            case Expr::EKind::QuantifiedSet:
                {
                    const tinyxml2::XMLElement * vars = dom->FirstChildElement("Variables");
                    if(nullptr == vars)
                        throw ExprReaderException
                            ("The 'Quantified_Set' element is missing some 'Variables' child.",dom->GetLineNum());
                    std::vector<TypedVar> ids;
                    for(    const tinyxml2::XMLElement * ce = vars->FirstChildElement();
                            nullptr != ce;
                            ce = ce->NextSiblingElement() )
                    {
                        ids.push_back(VarNameFromId(ce,typeInfos));
                    }
                    Pred body = readPredicate(dom->FirstChildElement("Body")->FirstChildElement(),typeInfos);
                    return Expr::makeQuantifiedSet(ids,std::move(body),type,bxmlTag );
                }
            case Expr::EKind::UnaryExpr:
                {
                    auto op = dom->Attribute("op");
                    auto it = unaryExpOp.find(op);
                    if(it == unaryExpOp.end())
                        throw ExprReaderException
                            ("Unknown unary expression operator '" + std::string(op) + "'.",dom->GetLineNum());
                    Expr content = readExpression(dom->FirstChildElement(),typeInfos);
                    return Expr::makeUnaryExpr(it->second,std::move(content),type,bxmlTag);
                }
            case Expr::EKind::Struct:
                {
                    std::vector<std::pair<std::string,Expr>> vec;
                    for(const tinyxml2::XMLElement * recItem = dom->FirstChildElement("Record_Item");
                            nullptr != recItem;
                            recItem = recItem->NextSiblingElement("Record_Item"))
                    {
                        if(nullptr == recItem->Attribute("label"))
                            throw ExprReaderException
                                ("The 'Record_Item' element is missing a 'label' attribute.",dom->GetLineNum());
                        vec.push_back(std::make_pair(
                                    recItem->Attribute("label"),
                                    readExpression(recItem->FirstChildElement(),typeInfos)));
                    };
                    std::sort(vec.begin(),vec.end(),RecordFieldCmp);
                    return Expr::makeStruct(std::move(vec),type, bxmlTag);
                }
            case Expr::EKind::Record:
                {
                    std::vector<std::pair<std::string,Expr>> vec;
                    for(const tinyxml2::XMLElement * recItem = dom->FirstChildElement("Record_Item");
                            nullptr != recItem;
                            recItem = recItem->NextSiblingElement("Record_Item"))
                    {
                        if(nullptr == recItem->Attribute("label"))
                            throw ExprReaderException
                                ("The 'Record_Item' element is missing a 'label' attribute.",dom->GetLineNum());
                        vec.push_back(std::make_pair(
                                    recItem->Attribute("label"),
                                    readExpression(recItem->FirstChildElement(),typeInfos)));
                    };
                    std::sort(vec.begin(),vec.end(),RecordFieldCmp);
                    return Expr::makeRecord(std::move(vec),type, bxmlTag);
                }
            case Expr::EKind::TRUE: // Boolean_Literal
                {
                    const char * lt {dom->Attribute("value")};
                    if(strEqCaseInsensitive(lt, "TRUE")) return Expr::makeTRUE(bxmlTag);
                    else if(strEqCaseInsensitive(lt, "FALSE")) return Expr::makeFALSE(bxmlTag);
                    else
                        throw ExprReaderException("Unknown boolean literal '"
                                + std::string(lt) + "'.",dom->GetLineNum());
                }
            case Expr::EKind::Record_Field_Update:
                {
                    const tinyxml2::XMLElement * fst = dom->FirstChildElement();
                    const tinyxml2::XMLElement * snd = fst->NextSiblingElement();
                    Expr rec = readExpression(fst,typeInfos);
                    std::string label = dom->Attribute("label");
                    Expr fval = readExpression(snd,typeInfos);
                    return Expr::makeRecordFieldUpdate(std::move(rec),label,std::move(fval),type,bxmlTag);
                }
            case Expr::EKind::Record_Field_Access:
                {
                    const tinyxml2::XMLElement * fst = dom->FirstChildElement();
                    Expr rec = readExpression(fst,typeInfos);
                    std::string label = dom->Attribute("label");
                    return Expr::makeRecordFieldAccess(std::move(rec),label,type,bxmlTag);
                }
            case Expr::EKind::MaxInt:
            case Expr::EKind::MinInt:
            case Expr::EKind::INTEGER:
            case Expr::EKind::NATURAL:
            case Expr::EKind::NATURAL1:
            case Expr::EKind::INT:
            case Expr::EKind::NAT:
            case Expr::EKind::NAT1:
            case Expr::EKind::STRING:
            case Expr::EKind::BOOL:
            case Expr::EKind::REAL:
            case Expr::EKind::FLOAT:
            case Expr::EKind::FALSE:
            case Expr::EKind::Successor:
            case Expr::EKind::Predecessor:
                {
                    assert(false); // unreachable
                }
        };
        assert(false); // unreachable
    };
}

static bool strEqCaseInsensitive(const char * lhs, const char * rhs) {
    const char *pl {lhs};
    const char *pr {rhs};
    while (*pl != '\0' && *pr != '\0' && toupper(*pl) == toupper(*pr)) {
        ++pl; ++pr;
    }
    return *pl == '\0' && *pr == '\0';
}

void splitDecimal(const char *str, std::string *integralPart, std::string *decimalPart)
{
    unsigned pos1 = 0;
    const char *p;
    p = str;
    while (*p != '\0' && *p != '.') {
        ++p; ++pos1;
    }
    *integralPart = std::string(str, pos1);
    unsigned pos2 = 0;
    while (*p != '\0') {
        ++p; ++pos2;
    }
    if(pos2 == 0)
        *decimalPart = std::string();
    else 
        *decimalPart = std::string(str+pos1+1, pos2);
}
