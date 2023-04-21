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

#include "exprReader.h"
#include "predReader.h"
#include <map>

namespace Xml {
    const std::map<std::string, Pred::PKind> ptags = {
        {"Binary_Pred", Pred::PKind::Implication},
        {"Exp_Comparison", Pred::PKind::ExprComparison},
        {"Quantified_Pred", Pred::PKind::Forall},
        {"Unary_Pred", Pred::PKind::Negation},
        {"Nary_Pred", Pred::PKind::Conjunction},
    };

    const std::map<std::string, Pred::ComparisonOp> comparisonOp = {
                {":", Pred::ComparisonOp::Membership},
        {"<:",Pred::ComparisonOp::Subset},
        {"<<:",Pred::ComparisonOp::Strict_Subset},
        {"=", Pred::ComparisonOp::Equality},
        {">=i",Pred::ComparisonOp::Ige},
        {">i",Pred::ComparisonOp::Igt},
        {"<i",Pred::ComparisonOp::Ilt},
        {"<=i",Pred::ComparisonOp::Ile},
        {">=r",Pred::ComparisonOp::Rge},
        {">r",Pred::ComparisonOp::Rgt},
        {"<r",Pred::ComparisonOp::Rlt},
        {"<=r",Pred::ComparisonOp::Rle},
        {">=f",Pred::ComparisonOp::Fge},
        {">f",Pred::ComparisonOp::Fgt},
        {"<f",Pred::ComparisonOp::Flt},
        {"<=f",Pred::ComparisonOp::Fle},
    };

    Pred readPredicate(const tinyxml2::XMLElement *dom, const std::vector<BType> &typeInfos){
        if (nullptr == dom)
            throw PredReaderException("Null dom element.");

        const auto &tagName {dom->Name()};

        if(0 == strcmp(tagName, "Tag"))
            return readPredicate(dom->FirstChildElement(), typeInfos);

        auto it = ptags.find(tagName);
        if(it == ptags.end())
            throw PredReaderException("Unexpected tag '" + std::string(tagName) + "'.");

        const auto &op {dom->Attribute("op")};
        switch(it->second){
            case Pred::PKind::Implication:
            case Pred::PKind::Equivalence:
                {
                    const tinyxml2::XMLElement *fst {dom->FirstChildElement()};
                    const tinyxml2::XMLElement *snd {fst->NextSiblingElement()};
                    if(0 == strcmp(op, "=>")){
                        return Pred::makeImplication(readPredicate(fst,typeInfos),readPredicate(snd,typeInfos));
                    } else if(0 == strcmp(op, "<=>")){
                        return Pred::makeEquivalence(readPredicate(fst,typeInfos),readPredicate(snd,typeInfos));
                    } else {
                        throw PredReaderException
                            ("Unknown binary predicate operator '" + std::string(op) + "'.");
                    }
                }
            case Pred::PKind::ExprComparison:
                {
                    auto it {comparisonOp.find(op)};
                    const tinyxml2::XMLElement *fst {dom->FirstChildElement()};
                    const tinyxml2::XMLElement *snd {fst->NextSiblingElement()};
                    if(it != comparisonOp.end())
                        return Pred::makeExprComparison
                            (it->second,readExpression(fst,typeInfos),readExpression(snd,typeInfos));
                    if (0 == strcmp(op, "/:"))
                        return Pred::makeNegation (Pred::makeExprComparison
                                (Pred::ComparisonOp::Membership,
                                 readExpression(fst,typeInfos),
                                 readExpression(snd,typeInfos)));
                    if (0 == strcmp(op, "/<:"))
                        return Pred::makeNegation (Pred::makeExprComparison
                                (Pred::ComparisonOp::Subset,
                                 readExpression(fst,typeInfos),
                                 readExpression(snd,typeInfos)));
                    if (0 == strcmp(op, "/<<:"))
                        return Pred::makeNegation (Pred::makeExprComparison
                                (Pred::ComparisonOp::Strict_Subset,
                                 readExpression(fst,typeInfos),
                                 readExpression(snd,typeInfos)));
                    if (0 == strcmp(op, "/="))
                        return Pred::makeNegation (Pred::makeExprComparison
                                (Pred::ComparisonOp::Equality,
                                 readExpression(fst,typeInfos),
                                 readExpression(snd,typeInfos)));
                    throw PredReaderException
                            ("Unknown comparison operator '" + std::string(op) + "'.");
                }
            case Pred::PKind::Forall:
            case Pred::PKind::Exists:
                {
                    const tinyxml2::XMLElement * vars {dom->FirstChildElement("Variables")};
                    if(nullptr == vars)
                        throw PredReaderException
                            ("The 'Quantified_Pred' element is missing some 'Variables' child.");
                    std::vector<TypedVar> vec;
                    for(    const tinyxml2::XMLElement *ce = vars->FirstChildElement("Id");
                            nullptr != ce;
                            ce = ce->NextSiblingElement("Id") )
                    {
                        vec.push_back(VarNameFromId(ce,typeInfos));
                    }

                    if(0 == strcmp(op, "!")){
                        return Pred::makeForall(vec,
                                readPredicate(dom->FirstChildElement("Body")->FirstChildElement(),typeInfos));
                    } else if (0 == strcmp(op, "#")){
                        return Pred::makeExists(vec,
                                readPredicate(dom->FirstChildElement("Body")->FirstChildElement(),typeInfos));
                    } else
                        throw PredReaderException
                            ("Unknown type of quantified predicate '" + std::string(op) + "'.");
                }
            case Pred::PKind::Negation:
                {
                    if(0 != strcmp(op, "not"))
                        throw PredReaderException
                            ("Unknown unary predicate operator '" + std::string(op) + "'.");

                    return Pred::makeNegation(
                            readPredicate(dom->FirstChildElement(),typeInfos));
                }
            case Pred::PKind::Conjunction:
            case Pred::PKind::Disjunction:
                {
                    const tinyxml2::XMLElement * ce = dom->FirstChildElement();
                    std::vector<Pred> vec;
                    while (nullptr != ce) {
                        vec.push_back(readPredicate(ce,typeInfos));
                        ce = ce->NextSiblingElement();
                    }

                    if(0 == strcmp(op, "&")){
                        return Pred::makeConjunction(std::move(vec));
                    } else if (0 == strcmp(op, "or")){
                        return Pred::makeDisjunction(std::move(vec));
                    } else {
                        throw PredReaderException
                            ("Unknown n-ary predicate operator '" + std::string(op) + "'.");
                    }

                }
            case Pred::PKind::True:
            case Pred::PKind::False:
                assert(false); // unreachable
        };
        assert(false); // unreachable
    };
}
