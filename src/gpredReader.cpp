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
#include "substReader.h"
#include "gpredReader.h"
#include "gpred.h"
#include <map>

namespace Xml {

    const std::map<std::string, GPred::Kind> ptags = {
        {"Binary_Pred", GPred::Kind::Implication},
        {"Exp_Comparison", GPred::Kind::ExprComparison},
        {"Quantified_Pred", GPred::Kind::Forall},
        {"Unary_Pred", GPred::Kind::Negation},
        {"Nary_Pred", GPred::Kind::Conjunction},
        {"Tag", GPred::Kind::TaggedPred},
        {"Sub_Calculus", GPred::Kind::Sub},
        {"Not", GPred::Kind::NotSubNot},
        {"Let_Fresh_Id", GPred::Kind::LetFreshId},
    };

    GPred readGPredicate(const tinyxml2::XMLElement *dom, const std::vector<BType> &typeInfos){
        if (dom == nullptr)
            throw GPredReaderException("Null dom element.");

        const char * tagName = dom->Name();

        auto it = ptags.find(tagName);
        if(it == ptags.end())
            throw GPredReaderException("Unexpected tag '" + std::string(tagName) + "'.");

        switch(it->second){
            case GPred::Kind::NotSubNot:
                {
                    const tinyxml2::XMLElement * child = dom->FirstChildElement();
                    if(child == nullptr || 0 != strcmp(child->Name(), "Sub_Calculus"))
                        throw GPredReaderException("Sub_Calculus element expected.");
                    const tinyxml2::XMLElement * sub = child->FirstChildElement();
                    const tinyxml2::XMLElement * _not = sub->NextSiblingElement();
                    if(_not == nullptr || 0 != strcmp(_not->Name(), "Not"))
                        throw GPredReaderException("Not element expected.");
                    const tinyxml2::XMLElement * prd = _not->FirstChildElement();
                    return GPred::makeNotSubNot
                        (readSubstitution(sub,typeInfos),readPredicate(prd,typeInfos));
                }

            case GPred::Kind::Sub:
                {
                    const tinyxml2::XMLElement * fst = dom->FirstChildElement();
                    const tinyxml2::XMLElement * snd = fst->NextSiblingElement();
                    bool overflow = (0 == strcmp(dom->Attribute("overflow"), "true"));
                    return GPred::makeSub
                        (readSubstitution(fst,typeInfos),readGPredicate(snd,typeInfos),overflow);
                }
            case GPred::Kind::Implication:
            case GPred::Kind::Equivalence:
                {
                    const char * op = dom->Attribute("op");
                    const tinyxml2::XMLElement * fst = dom->FirstChildElement();
                    const tinyxml2::XMLElement * snd = fst->NextSiblingElement();
                    if(0 == strcmp(op, "=>")){
                        return GPred::makeImplication(readGPredicate(fst,typeInfos),readGPredicate(snd,typeInfos));
                    } else if (0 == strcmp(op, "<=>")){
                        return GPred::makeEquivalence(readGPredicate(fst,typeInfos),readGPredicate(snd,typeInfos));
                    } else {
                        throw GPredReaderException
                            ("Unknown binary predicate operator '" + std::string(op) + "'.");
                    }
                }
            case GPred::Kind::ExprComparison:
                {
                    const char *op = dom->Attribute("op");
                    auto it = comparisonOp.find(op);
                    const tinyxml2::XMLElement * fst = dom->FirstChildElement();
                    const tinyxml2::XMLElement * snd = fst->NextSiblingElement();
                    if(it != comparisonOp.end())
                        return GPred::makeExprComparison
                            (it->second,readExpression(fst,typeInfos),readExpression(snd,typeInfos));
                    if (0 == strcmp(op, "/:"))
                        return GPred::makeNegationPred (GPred::makeExprComparison
                                (Pred::ComparisonOp::Membership,
                                 readExpression(fst,typeInfos),
                                 readExpression(snd,typeInfos)));
                    if (0 == strcmp(op, "/<:"))
                        return GPred::makeNegationPred (GPred::makeExprComparison
                                (Pred::ComparisonOp::Subset,
                                 readExpression(fst,typeInfos),
                                 readExpression(snd,typeInfos)));
                    if (0 == strcmp(op, "/<<:"))
                        return GPred::makeNegationPred (GPred::makeExprComparison
                                (Pred::ComparisonOp::Strict_Subset,
                                 readExpression(fst,typeInfos),
                                 readExpression(snd,typeInfos)));
                    if (0 == strcmp(op, "/="))
                        return GPred::makeNegationPred (GPred::makeExprComparison
                                (Pred::ComparisonOp::Equality,
                                 readExpression(fst,typeInfos),
                                 readExpression(snd,typeInfos)));
                        throw GPredReaderException
                            ("Unknown comparison operator '" + std::string(op) + "'.");

                }
            case GPred::Kind::Forall:
            case GPred::Kind::Exists:
                {
                    const char * op = dom->Attribute("type");
                    const tinyxml2::XMLElement * vars = dom->FirstChildElement("Variables");
                    if(vars == nullptr)
                        throw GPredReaderException
                            ("The 'Quantified_Pred' element is missing some 'Variables' child.");
                    std::vector<TypedVar> vec;
                    for(    const tinyxml2::XMLElement * ce = vars->FirstChildElement();
                            ce != nullptr;
                            ce = ce->NextSiblingElement() )
                    {
                        vec.push_back(VarNameFromId(ce,typeInfos));
                    }
                    if(0 == strcmp(op, "!")){
                        return GPred::makeForall(vec,
                                readGPredicate(dom->FirstChildElement("Body")->FirstChildElement(),typeInfos));
                    } else if (0 == strcmp(op, "#")){
                        return GPred::makeExists(vec,
                                readGPredicate(dom->FirstChildElement("Body")->FirstChildElement(),typeInfos));
                    } else {
                        throw GPredReaderException
                            ("Unknown type of quantified predicate '" + std::string(op) + "'.");
                    }
                }
            case GPred::Kind::Negation:
                {
                    const char * op = dom->Attribute("op");
                    if(0 != strcmp(op, "not"))
                        throw GPredReaderException
                            ("Unknown unary predicate operator '" + std::string(op) + "'.");

                    return GPred::makeNegationPred(
                            readGPredicate(dom->FirstChildElement(),typeInfos));
                }
            case GPred::Kind::Conjunction:
            case GPred::Kind::Disjunction:
                {
                    const char * op = dom->Attribute("op");
                    std::vector<GPred> vec;
                    const tinyxml2::XMLElement * ce = dom->FirstChildElement();
                    while (ce != nullptr) {
                        vec.push_back(readGPredicate(ce,typeInfos));
                        ce = ce->NextSiblingElement();
                    }
                    if(0 == strcmp(op, "&")){
                        return GPred::makeConjunction(std::move(vec));
                    } else if(0 == strcmp(op, "or")){
                        return GPred::makeDisjunction(std::move(vec));
                    } else {
                        throw GPredReaderException
                            ("Unknown n-ary predicate operator '" + std::string(op) + "'.");
                    }
                }
            case GPred::Kind::TaggedPred:
                {
                    auto elt = dom->FirstChildElement();
                    return GPred::makeTaggedPred(dom->Attribute("goalTag"),readGPredicate(elt,typeInfos));
                }
            case GPred::Kind::LetFreshId:
                {
                    auto elt = dom->FirstChildElement();
                    return GPred::makeLetFreshId(dom->Attribute("name"),readGPredicate(elt,typeInfos));
                }
        };
        assert(false); // unreachable
    };
}
