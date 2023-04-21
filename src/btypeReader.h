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

#ifndef BTYPE_READER_H
#define BTYPE_READER_H

#include<vector>

#include "btype.h"
#include "tinyxml2.h"


namespace Xml {

    /// @brief reads a TypeInfos DOM element and builds the representation of the corresponding B types
    /// @param dom a TypeInfos DOM element, as found in BXML, IBXML, POXML and POG files
    /// @param typeInfos a vector where the representation of each type in the TypeInfos is stored 
    /// @pre the vector @a typeInfos is empty
    /// @post The representation of the i-th Type element child of @a dom vector is stored at the i-th
    /// position of @a typeInfos.
    void readTypeInfos(const tinyxml2::XMLElement *dom,std::vector<BType> &typeInfos);

    /// @brief exceptions that may be thrown by @a Xml::readTypeInfos on corrupt input
    class BTypeReaderException : public std::exception
    {
        public:
            BTypeReaderException(const std::string desc, int line):
                description{desc + " (line " + std::to_string(line) + ")"}
            {};
            ~BTypeReaderException() throw(){};
            const char *what() const throw(){ return description.c_str(); };
        private:
            std::string description;
    };

}

#endif // BTYPE_READER_H