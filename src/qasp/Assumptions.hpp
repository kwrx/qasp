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
#include <unordered_set>


namespace qasp {

    class Assumptions : private std::unordered_set<std::string>
                      , private std::vector<Atom> {

        public:

            using std::vector<Atom>::begin;
            using std::vector<Atom>::end;
            using std::vector<Atom>::empty;


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

            template <typename ...T>
            inline void emplace_back(T&&... args) {
                
                const Atom a(args...);

                if(unlikely(this->std::unordered_set<std::string>::find(a.predicate()) != this->std::unordered_set<std::string>::end()))
                    return;

                this->std::unordered_set<std::string>::emplace(a.predicate());
                this->std::vector<Atom>::emplace_back(std::move(a));

            }

            template <typename T>
            inline void insert(std::vector<Atom>::iterator it, T first, T last) {

                for(; first != last; first++) {

                    auto a = *first;

                    if(unlikely(this->std::unordered_set<std::string>::find(a.predicate()) != this->std::unordered_set<std::string>::end()))
                        continue;

                    this->std::unordered_set<std::string>::emplace(a.predicate());
                    this->std::vector<Atom>::emplace_back(std::move(a));

                }

            }


            inline void clear() noexcept {

                this->std::unordered_set<std::string>::clear();
                this->std::vector<Atom>::clear();

            }


    };

}