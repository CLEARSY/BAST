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

#include "substReader.h"
#include "exprReader.h"
#include "predReader.h"
#include <map>

namespace Xml {
    const std::map<std::string, Subst::SKind> ptags = {
        {"Bloc_Sub", Subst::SKind::Block},
        {"Skip", Subst::SKind::Skip},
        {"Assert_Sub", Subst::SKind::Assert},
        {"PRE_Sub", Subst::SKind::Assert},
        {"If_Sub", Subst::SKind::IfThenElse},
        {"Simple_Assignement_Sub", Subst::SKind::SimpleAssignment},
        {"Select", Subst::SKind::Select},
        {"Case_Sub", Subst::SKind::Case},
        {"ANY_Sub", Subst::SKind::Any},
        {"Operation_Call", Subst::SKind::OperationCall},
        {"While", Subst::SKind::While},
        {"Witness", Subst::SKind::Witness},
    };

    Subst readSubstitution(const tinyxml2::XMLElement *dom, const std::vector<BType> &typeInfos){
        if (nullptr == dom)
            throw SubstReaderException("Null dom element.");

        const char *tagName = dom->Name();
        Subst::SKind kind;
        if(0 == strcmp(tagName, "Nary_Sub")) {
            const char *op = dom->Attribute("op");
            if(0 == strcmp(op, "||"))
                kind = Subst::SKind::Parallel;
            else if(0 == strcmp(op, ";"))
                kind = Subst::SKind::Sequence;
            else if(0 == strcmp(op, "CHOICE"))
                kind = Subst::SKind::Choice;
            else
                throw SubstReaderException("Unknown nary substitution operator '"+ std::string(op) +"'.");
        }
        else
        {
            auto it = ptags.find(tagName);
            if(it == ptags.end())
                throw SubstReaderException("Unexpected tag '" + std::string(tagName) + "'.");
            kind = it->second;
        };

        switch(kind){
            case Subst::SKind::Block:
                return Subst::makeBlock(readSubstitution(dom->FirstChildElement(),typeInfos));
            case Subst::SKind::Skip:
                return Subst::makeSkip();
            case Subst::SKind::Assert:
                {
                    const tinyxml2::XMLElement * guard;
                    if(0 == strcmp(tagName, "PRE_Sub")){
                        guard = dom->FirstChildElement("Precondition");
                        if(guard == nullptr)
                            throw SubstReaderException("Missing child 'Precondition' in PRE_Sub element.");
                    } else {
                        guard = dom->FirstChildElement("Guard");
                        if(guard == nullptr)
                            throw SubstReaderException("Missing child 'Guard' in Assert_Sub element.");
                    }
                    const tinyxml2::XMLElement *body = dom->FirstChildElement("Body");
                    if(body == nullptr)
                        throw SubstReaderException("Missing child 'Body' in Assert_Sub or PRE_Sub element.");
                    return Subst::makeAssert(
                            readPredicate(guard->FirstChildElement(),typeInfos),
                            readSubstitution(body->FirstChildElement(),typeInfos)
                            );
                }
            case Subst::SKind::IfThen:
            case Subst::SKind::IfThenElse:
                {
                    const tinyxml2::XMLElement * condition = dom->FirstChildElement("Condition");
                    if(condition == nullptr)
                        throw SubstReaderException("Missing child 'Condition' in 'If_Sub' element.");
                    const tinyxml2::XMLElement * then = dom->FirstChildElement("Then");
                    if(then == nullptr)
                        throw SubstReaderException("Missing child 'Then' in 'If_Sub' element.");
                    const tinyxml2::XMLElement * els = dom->FirstChildElement("Else");
                    if(els == nullptr)
                        return Subst::makeIfThen(
                                readPredicate(condition->FirstChildElement(),typeInfos),
                                readSubstitution(then->FirstChildElement(),typeInfos)
                                );
                    else
                        return Subst::makeIfThenElse(
                                readPredicate(condition->FirstChildElement(),typeInfos),
                                readSubstitution(then->FirstChildElement(),typeInfos),
                                readSubstitution(els->FirstChildElement(),typeInfos)
                                );
                }
            case Subst::SKind::SimpleAssignment:
                {
                    const tinyxml2::XMLElement * vars = dom->FirstChildElement("Variables");
                    if(vars == nullptr)
                        throw SubstReaderException("Missing child 'Variables' in 'Simple_Assignement_Sub' element.");
                    std::vector<TypedVar> vec;
                    for(    const tinyxml2::XMLElement * ce = vars->FirstChildElement();
                            ce != nullptr;
                            ce = ce->NextSiblingElement() )
                    {
                        vec.push_back(VarNameFromId(ce,typeInfos));
                    }
                    const tinyxml2::XMLElement * values = dom->FirstChildElement("Values");
                    if(values == nullptr)
                        throw SubstReaderException("Missing child 'Values' in 'Simple_Assignement_Sub' element.");
                    std::vector<Expr> vec2;
                    for(    const tinyxml2::XMLElement * ce = values->FirstChildElement();
                            ce != nullptr;
                            ce = ce->NextSiblingElement() )
                    {
                        vec2.push_back(readExpression(ce,typeInfos));
                    }
                    return Subst::makeSimpleAssignment(vec,std::move(vec2));
                }
            case Subst::SKind::Select:
            case Subst::SKind::SelectElse:
                {
                    const tinyxml2::XMLElement * clauses = dom->FirstChildElement("When_Clauses");
                    if(clauses == nullptr)
                        throw SubstReaderException("Missing child 'When_Clauses' in 'Select' element.");
                    std::vector<std::pair<Pred,Subst>> vec;
                    for(    const tinyxml2::XMLElement * ce = clauses->FirstChildElement("When");
                            ce != nullptr;
                            ce = ce->NextSiblingElement("When") )
                    {
                        const tinyxml2::XMLElement * cond = ce->FirstChildElement("Condition");
                        if(cond == nullptr)
                            throw SubstReaderException("Missing child 'Condition' in 'When' element.");
                        const tinyxml2::XMLElement * then = ce->FirstChildElement("Then");
                        if(then == nullptr)
                            throw SubstReaderException("Missing child 'Then' in 'When' element.");
                        vec.push_back( {
                                readPredicate(cond->FirstChildElement(),typeInfos),
                                readSubstitution(then->FirstChildElement(),typeInfos) });
                    }
                    const tinyxml2::XMLElement * els = dom->FirstChildElement("Else");
                    if(els == nullptr)
                        return Subst::makeSelect(std::move(vec));
                    else
                        return Subst::makeSelectElse(std::move(vec),
                                readSubstitution(els->FirstChildElement(),typeInfos) );
                }
            case Subst::SKind::Case:
            case Subst::SKind::CaseElse:
                {
                    const tinyxml2::XMLElement * value = dom->FirstChildElement("Value");
                    if(value== nullptr)
                        throw SubstReaderException("Missing child 'Value' in 'Case_Sub' element.");
                    const tinyxml2::XMLElement * choices = dom->FirstChildElement("Choices");
                    if(choices== nullptr)
                        throw SubstReaderException("Missing child 'Choices' in 'Case_Sub' element.");
                    std::vector<Subst::CaseChoice> vec;
                    for(    const tinyxml2::XMLElement * ce = choices->FirstChildElement("Choice");
                            ce != nullptr;
                            ce = ce->NextSiblingElement("Choice") )
                    {
                        const tinyxml2::XMLElement * v = ce->FirstChildElement("Value");
                        Subst::CaseChoice ch;
                        for(    const tinyxml2::XMLElement * v = ce->FirstChildElement("Value");
                                v != nullptr;
                                v = v->NextSiblingElement("Value") )
                        {
                            ch.values.push_back(readExpression(v->FirstChildElement(),typeInfos));
                        }
                        if(ch.values.empty())
                            throw SubstReaderException("Missing child 'Value' in 'Choice' element.");
                        const tinyxml2::XMLElement * then = ce->FirstChildElement("Then");
                        if(then== nullptr)
                            throw SubstReaderException("Missing child 'Then' in 'Choice' element.");
                        ch.body = readSubstitution(then->FirstChildElement(),typeInfos);
                        vec.push_back(std::move(ch));
                    }
                    const tinyxml2::XMLElement * els = dom->FirstChildElement("Else");
                    if(els != nullptr)
                        return Subst::makeCase(readExpression(value->FirstChildElement(),typeInfos),std::move(vec));
                    else {
                        return Subst::makeCaseElse(readExpression(value->FirstChildElement(),typeInfos),
                                std::move(vec),
                                readSubstitution(els->FirstChildElement(),typeInfos) );
                    }
                }
            case Subst::SKind::Any:
                {
                    const tinyxml2::XMLElement * vars = dom->FirstChildElement("Variables");
                    if(vars == nullptr)
                        throw SubstReaderException("Missing child 'Variables' in 'ANY_Sub' element.");
                    std::vector<TypedVar> vec;
                    for(    const tinyxml2::XMLElement * ce = vars->FirstChildElement();
                            ce != nullptr;
                            ce = ce->NextSiblingElement() )
                    {
                        vec.push_back(VarNameFromId(ce,typeInfos));
                    }
                    const tinyxml2::XMLElement * pred = dom->FirstChildElement("Pred");
                    if(pred != nullptr)
                        throw SubstReaderException("Missing child 'Pred' in 'ANY_Sub' element.");
                    const tinyxml2::XMLElement * then = dom->FirstChildElement("Then");
                    if(then != nullptr)
                        throw SubstReaderException("Missing child 'Then' in 'ANY_Sub' element.");
                    const tinyxml2::XMLElement * fc = then->FirstChildElement();
                    return Subst::makeAny(
                            vec,
                            readPredicate(pred->FirstChildElement(),typeInfos),
                            readSubstitution(fc,typeInfos) );
                }
            case Subst::SKind::Witness:
                {
                    const tinyxml2::XMLElement * wt = dom->FirstChildElement("Witnesses");
                    if(wt != nullptr)
                        throw SubstReaderException("Missing child 'Witnesses' in 'Witness' element.");
                    std::map<std::string,Expr> witnesses;
                    const tinyxml2::XMLElement * wt_child = wt->FirstChildElement();
                    if(wt_child != nullptr)
                        throw SubstReaderException("Missing child in 'Witnesses' element.");
                    if(0 == strcmp(wt_child->Name(), "Nary_Pred")) {
                        if(0 != strcmp(wt_child->Attribute("op"), "&"))
                            throw SubstReaderException("Expected Nary_Pred with attribute op '&'.");
                        for(    const tinyxml2::XMLElement * ce = wt_child->FirstChildElement("Exp_Comparison");
                                ce != nullptr;
                                ce = ce->NextSiblingElement("Exp_Comparison") )
                        {
                            if(0 != strcmp(ce->Attribute("op"), "="))
                                throw SubstReaderException("Expected Exp_Comparison with attribute op '='.");
                            const tinyxml2::XMLElement * id = ce->FirstChildElement();
                            if(0 != strcmp(id->Name(), "Id"))
                                throw SubstReaderException("Id element expected.");
                            if(nullptr == id->Attribute("value"))
                                throw SubstReaderException("value attribute expected.");
                            const tinyxml2::XMLElement * expr = id->NextSiblingElement();
                            std::pair<std::string,Expr> pair = {id->Attribute("value"), readExpression(expr,typeInfos)};
                            witnesses.insert(std::move(pair));
                        }
                    }
                    else if(0 == strcmp(wt_child->Name(), "Exp_Comparison")){
                        if(0 == strcmp(wt_child->Attribute("op"), "="))
                            throw SubstReaderException("Expected Exp_Comparison with attribute op '='.");
                        const tinyxml2::XMLElement * id = wt_child->FirstChildElement();
                        if(0 == strcmp(id->Name(), "Id"))
                            throw SubstReaderException("Id element expected.");
                        if(nullptr == id->Attribute("value"))
                            throw SubstReaderException("value attribute expected.");
                        const tinyxml2::XMLElement * expr = id->NextSiblingElement();
                        std::pair<std::string,Expr> pair = {id->Attribute("value"), readExpression(expr,typeInfos)};
                        witnesses.insert(std::move(pair));
                    } else {
                        throw SubstReaderException("Nary_Pred or Exp_Comparison element expected.");
                    }
                    const tinyxml2::XMLElement * body = dom->FirstChildElement("Body");
                    if(body== nullptr)
                        throw SubstReaderException("Missing child 'Body' in 'Witness' element.");
                    return Subst::makeWitness(
                            std::move(witnesses),
                            readSubstitution(body->FirstChildElement(),typeInfos) );
                }
            case Subst::SKind::OperationCall:
                {
                    const tinyxml2::XMLElement * name = dom->FirstChildElement("Name");
                    if(name == nullptr)
                        throw SubstReaderException("Missing child 'Name' in 'Operation_Call' element.");
                    const tinyxml2::XMLElement * id = name->FirstChildElement("Id");
                    if(id == nullptr)
                        throw SubstReaderException("Missing child 'Id' in 'Name' element.");
                    if(nullptr == id->Attribute("value"))
                        throw SubstReaderException("Missing attribute 'value' in 'Id' element.");

                    // Inputs (Effective)
                    std::vector<Expr> v_input;
                    const tinyxml2::XMLElement * input = dom->FirstChildElement("Input_Parameters");
                    if(input != nullptr){
                        for(    const tinyxml2::XMLElement * ce = input->FirstChildElement();
                                ce != nullptr;
                                ce = ce->NextSiblingElement() )
                        {
                            v_input.push_back(readExpression(ce,typeInfos));
                        }
                    }
                    // Outputs (Effective)
                    std::vector<TypedVar> v_output;
                    const tinyxml2::XMLElement * output = dom->FirstChildElement("Output_Parameters");
                    if(output != nullptr){
                        for(    const tinyxml2::XMLElement * ce = output->FirstChildElement();
                                ce != nullptr;
                                ce = ce->NextSiblingElement() )
                        {
                            v_output.push_back(VarNameFromId(ce,typeInfos));
                        }
                    }
                    const tinyxml2::XMLElement * operation = dom->FirstChildElement("Operation");
                    if(operation== nullptr)
                        throw SubstReaderException("Missing child 'Operation' in 'Operation_Call' element.");
                    if(nullptr == operation->Attribute("name"))
                        throw SubstReaderException("Missing attribute 'name' in 'Operation' element.");

                    // Outputs (Formal)
                    std::vector<TypedVar> op_outputs;
                    const tinyxml2::XMLElement * op_out_params = operation->FirstChildElement("Output_Parameters");
                    if(op_out_params != nullptr){
                        for(    const tinyxml2::XMLElement * ce = op_out_params->FirstChildElement("Id");
                                ce != nullptr;
                                ce = ce->NextSiblingElement("Id") )
                        {
                            op_outputs.push_back(VarNameFromId(ce,typeInfos));
                        }
                    }

                    // Inputs (Formal)
                    std::vector<TypedVar> op_inputs;
                    const tinyxml2::XMLElement * op_in_params = operation->FirstChildElement("Input_Parameters");
                    if(op_in_params != nullptr){
                        for(    const tinyxml2::XMLElement * ce = op_in_params->FirstChildElement("Id");
                                ce != nullptr;
                                ce = ce->NextSiblingElement("Id") )
                        {
                            op_inputs.push_back(VarNameFromId(ce,typeInfos));
                        }
                    }

                    if(v_input.size() != op_inputs.size())
                        throw SubstReaderException("Wrong number of input parameters in call to operation "
                                + std::string(id->Attribute("value"))
                                + " (Formal: " + std::to_string(op_inputs.size()) + ", "
                                + "Effective: " +std::to_string(v_input.size()) + ")");

                    if(v_output.size() != op_outputs.size())
                        throw SubstReaderException("Wrong number of output parameters in call to operation "
                                + std::string(id->Attribute("value"))
                                + " (Formal: " + std::to_string(op_outputs.size()) + ", "
                                + "Effective: " +std::to_string(v_output.size()) + ")");

                    // Precondition
                    const tinyxml2::XMLElement * op_pre = operation->FirstChildElement("Precondition");
                    Pred pre = op_pre == nullptr ?
                        Pred::makeTrue() :
                        readPredicate(op_pre->FirstChildElement(),typeInfos);

                    // Body
                    const tinyxml2::XMLElement * op_body = operation->FirstChildElement("Body");
                    if(op_body== nullptr)
                        throw SubstReaderException("Missing body in call to operation " + std::string(id->Attribute("value")));

                    return Subst::makeOpCall(
                            id->Attribute("value"),
                            std::move(v_input),
                            v_output,
                            op_inputs,
                            op_outputs,
                            std::move(pre),
                            readSubstitution(op_body->FirstChildElement(),typeInfos));
                }
            case Subst::SKind::While:
                {
                    const tinyxml2::XMLElement * cond = dom->FirstChildElement("Condition");
                    if(cond == nullptr)
                        throw SubstReaderException("Missing child 'Condition' in 'While' element.");
                    const tinyxml2::XMLElement * body = dom->FirstChildElement("Body");
                    if(body == nullptr)
                        throw SubstReaderException("Missing child 'Body' in 'While' element.");
                    const tinyxml2::XMLElement * inv = dom->FirstChildElement("Invariant");
                    if(inv == nullptr)
                        throw SubstReaderException("Missing child 'Invariant' in 'While' element.");
                    const tinyxml2::XMLElement * var = dom->FirstChildElement("Variant");
                    if(var == nullptr)
                        throw SubstReaderException("Missing child 'Variant' in 'While' element.");
                    return Subst::makeWhile(
                            readPredicate(cond->FirstChildElement(),typeInfos),
                            readSubstitution(body->FirstChildElement(),typeInfos),
                            readPredicate(inv->FirstChildElement(),typeInfos),
                            readExpression(var->FirstChildElement(),typeInfos) );
                }
            case Subst::SKind::Sequence:
                {
                    std::vector<Subst> vec;
                    const tinyxml2::XMLElement * ce = dom->FirstChildElement();
                    while (ce != nullptr) {
                        vec.push_back(readSubstitution(ce,typeInfos));
                        ce = ce->NextSiblingElement();
                    }
                    return Subst::makeSequence(std::move(vec));
                }
            case Subst::SKind::Parallel:
                {
                    std::vector<Subst> vec;
                    const tinyxml2::XMLElement * ce = dom->FirstChildElement();
                    while (ce != nullptr) {
                        vec.push_back(readSubstitution(ce,typeInfos));
                        ce = ce->NextSiblingElement();
                    }
                    return Subst::makeParallel(std::move(vec));
                }
            case Subst::SKind::Choice:
                {
                    std::vector<Subst> vec;
                    const tinyxml2::XMLElement * ce = dom->FirstChildElement();
                    while (ce != nullptr) {
                        vec.push_back(readSubstitution(ce,typeInfos));
                        ce = ce->NextSiblingElement();
                    }
                    return Subst::makeChoice(std::move(vec));
                }
        };
        assert(false); // unreachable
    };
}
