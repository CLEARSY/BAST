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

#ifndef EXPR_PRINTER_H
#define EXPR_PRINTER_H

#include<iostream>

#include "expr.h"

extern void printTypedVar(std::ostream &stream, const TypedVar &v);

/// @brief pretty prints B expression 
/// @param stream the output stream where the expression is printed
/// @param p the expression to be printed
extern void printExpression(std::ostream &stream, const Expr &p);

#endif // EXPR_PRINTER_H