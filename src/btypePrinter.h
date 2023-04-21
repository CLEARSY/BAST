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

#ifndef BTYPEPRINTER_H
#define BTYPEPRINTER_H

#include<iostream>

#include "btype.h"

/// @brief pretty prints B type 
/// @param stream the output stream where the B type is printed
/// @param p the type to be printed
void printType(std::ostream &stream, const BType &p);

#endif // BTYPEPRINTER_H