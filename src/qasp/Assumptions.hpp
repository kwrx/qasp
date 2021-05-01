/*                                                                      
 * GPL3 License 
 * 
 * Copyright (C) 2021 Antonino Natale
 * This file is part of QASP.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "Atom.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <iterator>
#include <algorithm>


namespace qasp {

    class Assumptions : public std::vector<Atom> {
        public:

            inline friend std::ostream& operator <<(std::ostream& os, const Assumptions& a) {
                
                if(!a.empty()) {

                    os << "{";

                    for(auto i = a.begin(); i != std::prev(a.end()); i++)
                        os << (*i) << ";";

                    os << *std::prev(a.end());
                    os << "}.";

                }

                return os;

            }

    };

}