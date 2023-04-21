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

#ifndef SUBSTREADER_H
#define SUBSTREADER_H

#include "subst.h"
#include<QDomElement>

namespace Xml {
    class SubstReaderException : public std::exception
    {
        public:
            SubstReaderException(const std::string desc):description{desc}{};
            ~SubstReaderException() throw() {};
            const char *what() const throw(){ return description.c_str(); };
        private:
            std::string description;
    };

    Subst readSubstitution(const QDomElement &dom, const std::vector<BType> &typeInfos);
}

#endif // SUBSTREADER_H
