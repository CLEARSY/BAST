/*
   This file is part of BAST.
   Copyright © CLEARSY 2022-2025
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


#ifndef EXPRWRITER_H
#define EXPRWRITER_H

#include "tinyxml2.h"

#include "expr.h"

namespace Xml {
    using TypeMap_t = std::map<BType, size_t>;
    void writeTypedVar(tinyxml2::XMLPrinter &stream, TypeMap_t &typeInfos, const TypedVar &v);
    void writeExpression(tinyxml2::XMLPrinter &stream, TypeMap_t &typeInfos, const Expr &p);
}

#endif // EXPRWRITER_H
