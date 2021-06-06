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

#include "Predicate.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <iterator>
#include <algorithm>
#include <unordered_set>


namespace qasp {

    class Predicates : private std::unordered_set<std::size_t>
                     , private std::vector<Predicate> {

        public:

            using std::vector<Predicate>::begin;
            using std::vector<Predicate>::end;
            using std::vector<Predicate>::empty;


            inline friend std::ostream& operator <<(std::ostream& os, const Predicates& a) noexcept {
                
                if(!a.empty()) {

                    os << "{";

                    for(auto i = a.begin(); i != std::prev(a.end()); i++)
                        os << (*i) << ";";

                    os << *std::prev(a.end());
                    os << "}.";

                }

                return os;

            }


            inline const bool contains(const Predicate& p) const noexcept {
                return this->std::unordered_set<std::size_t>::find(p) != this->std::unordered_set<std::size_t>::end();
            }
            

            template <typename ...T>
            inline void emplace_back(T&&... args) noexcept {
                
                const Predicate p(std::forward<T>(args)...);

                if(unlikely(this->std::unordered_set<std::size_t>::find(p) != this->std::unordered_set<std::size_t>::end()))
                    return;

                this->std::unordered_set<std::size_t>::emplace(p);
                this->std::vector<Predicate>::emplace_back(std::move(p));

            }

            template <typename T>
            inline void insert(std::vector<Predicate>::iterator it, T first, T last) noexcept {

                for(; first != last; first++) {

                    auto p = *first;

                    if(unlikely(this->std::unordered_set<std::size_t>::find(p) != this->std::unordered_set<std::size_t>::end()))
                        continue;

                    this->std::unordered_set<std::size_t>::emplace(p);
                    this->std::vector<Predicate>::emplace_back(std::move(p));

                }

            }


            inline void clear() noexcept {

                this->std::unordered_set<std::size_t>::clear();
                this->std::vector<Predicate>::clear();

            }


    };

}