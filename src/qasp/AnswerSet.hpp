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
#include "utils/Performance.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <iterator>
#include <algorithm>


namespace qasp {

    class AnswerSet : public std::vector<Atom> {
        public:

            inline friend bool operator==(const AnswerSet& a, const AnswerSet& b) noexcept { __PERF_TIMING(answerset_comparing);

                if(a.size() != b.size())
                    return false;


                std::vector<Atom> v1 = a;
                std::vector<Atom> v2 = b;

                std::sort(v1.begin(), v1.end(), [] (const auto& i, const auto& j) { return i.predicate() < j.predicate(); });
                std::sort(v2.begin(), v2.end(), [] (const auto& i, const auto& j) { return i.predicate() < j.predicate(); });

                return std::equal(v1.begin(), v1.end(), v2.begin());

            }

            inline friend std::ostream& operator <<(std::ostream& os, const AnswerSet& a) noexcept {

                os << "{";
                
                if(!a.empty()) {

                    for(auto i = a.begin(); i != std::prev(a.end()); i++)
                        os << (*i) << ",";

                    os << *std::prev(a.end());

                }

                return os << "}", os;

            }


            inline const bool contains(const Atom& atom) const noexcept {
                return std::find(this->begin(), this->end(), atom) != this->end();
            }

    };

}