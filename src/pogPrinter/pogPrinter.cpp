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
#include<iostream>

#include "/opt/homebrew/include/tinyxml2.h"

#include "../btypeReader.h"
#include "../btypePrinter.h"
#include "../predReader.h"
#include "../predPrinter.h"

#include "pogPrinterConfig.h"

using std::ostream;

static void pogPrinter(ostream &stream, const tinyxml2::XMLDocument &pog){
    const tinyxml2::XMLElement *root {pog.FirstChildElement("Proof_Obligations")};
    if(nullptr == root)
        return;
    stream << "# Proof obligation file" << std::endl << std::endl;

    stream << "## Types" << std::endl << std::endl;

    // TypeInfos
    std::vector<BType> typeInfos;
    const tinyxml2::XMLElement *typeInfosElem {root->FirstChildElement("TypeInfos")};
    Xml::readTypeInfos(typeInfosElem,typeInfos);    

    for (const auto &type: typeInfos) {
        printType(stream, type);
        stream << std::endl << std::endl;
    }

    stream << "## Definitions" << std::endl << std::endl;

    // Defines
    for(const tinyxml2::XMLElement *e = root->FirstChildElement("Define");
            nullptr != e;
            e = e->NextSiblingElement("Define"))
    {
        const auto &name {e->Attribute("name")};
        stream << "### Define " << name << std::endl << std::endl; 

        for(const tinyxml2::XMLElement *ch {e->FirstChildElement()};
                nullptr != ch;
                ch = ch->NextSiblingElement())
        {
            if(0 == strcmp(ch->Name(), "Set")) {
            } else {
                Pred p = Xml::readPredicate(ch, typeInfos);
                printPredicate(stream, p);
                stream << std::endl << std::endl;
            }
        }
    }

    stream << "## Proof obligations" << std::endl << std::endl;

    // Proof_Obligation
    for(const tinyxml2::XMLElement *po {root->FirstChildElement("Proof_Obligation")};
            nullptr != po;
            po = po->NextSiblingElement("Proof_Obligation"))
    {
        // Tag
        const tinyxml2::XMLElement *e {po->FirstChildElement("Tag")};
        std::string tag {e->GetText()};

        stream << "### " << tag << std::endl << std::endl;

        stream << "#### Hypotheses" << std::endl << std::endl;

        std::vector<Pred> hyps;
        for(const tinyxml2::XMLElement *e {po->FirstChildElement("Hypothesis")};
                nullptr != e;
                e = e->NextSiblingElement("Hypothesis"))
        {
            Pred p = Xml::readPredicate(e->FirstChildElement(),typeInfos);
            printPredicate(stream, p);
            stream << std::endl << std::endl;
        }

        stream << "#### Local hypotheses" << std::endl << std::endl;
        
        std::vector<Pred> localHyps;
        for(const tinyxml2::XMLElement *e {po->FirstChildElement("Local_Hyp")};
                nullptr != e;
                e = e->NextSiblingElement("Local_Hyp"))
        {
            Pred p = Xml::readPredicate(e->FirstChildElement(),typeInfos);
            printPredicate(stream, p);
            stream << std::endl << std::endl;
        }

        // Simple Goal
        stream << "#### Goals" << std::endl << std::endl;
        
        for(const tinyxml2::XMLElement *e {po->FirstChildElement("Simple_Goal")};
                nullptr != e;
                e = e->NextSiblingElement("Simple_Goal"))
        {
            // Tag
            const tinyxml2::XMLElement *tag = e->FirstChildElement("Tag");
            std::string _tag = tag->GetText();
            // Ref Hyps

            stream << "#####" << _tag << std::endl << std::endl;
        
            // Goal
            const tinyxml2::XMLElement *goal {e->FirstChildElement("Goal")};
            Pred _goal = Xml::readPredicate(goal->FirstChildElement(),typeInfos);
            printPredicate(stream, _goal);
            stream << std::endl << std::endl;
        }
    }
}

int main(int argc, char **argv) {

  if (argc != 2) {
    std::cout << "pogPrinter version " << POG_PRINTER_VERSION_MAJOR << "."
              << POG_PRINTER_VERSION_MINOR << std::endl;
    std::cout << "Usage: " << argv[0] << " [path to POG file]" << std::endl;
    return 1;
  }
    tinyxml2::XMLDocument dom;

   tinyxml2::XMLError eResult = dom.LoadFile(argv[1]);
   if (eResult != tinyxml2::XML_SUCCESS) return 1;

   pogPrinter(std::cout, dom);
   return 0;
}