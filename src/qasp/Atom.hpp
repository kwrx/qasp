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

#include <string>
#include <algorithm>

#include "Predicate.hpp"

namespace qasp {

    typedef int64_t atom_index_t;

    class Atom {
        public:

            Atom(const atom_index_t index, const std::size_t arity, const std::string predicate)
                : __index(index)
                , __arity(arity)
                , __predicate(std::move(predicate)) {

                    extract_name();

                }


            Atom(const atom_index_t index, const std::string predicate)
                : __index(index)
                , __arity(0)
                , __predicate(std::move(predicate)) {

                    __arity += predicate.find('(') != std::string::npos;
                    __arity += std::count(predicate.begin(), predicate.end(), ',');

                    extract_name();

                }


            inline const auto& index() const {
                return this->__index;
            }

            inline const auto& index(atom_index_t value) {
                return this->__index = value, *this;
            }

            inline const auto& arity() const {
                return this->__arity;
            }

            inline const auto& arity(std::size_t value) {
                return this->__arity = value, *this;
            }

            inline const auto& predicate() const {
                return this->__predicate;
            }

            inline const auto& name() const {
                return this->__name;
            }

            inline bool operator ==(const Atom& b) const {
                return this->predicate() == b.predicate();
            }

            inline bool operator ==(const Predicate& b) const {
                return this->name() == b.name();
            }

            inline friend std::ostream& operator <<(std::ostream& os, const Atom& a) {
                return os << a.predicate(), os;
            }


        private:

            atom_index_t __index;
            std::size_t __arity;
            std::string __predicate;
            std::string __name {};

            inline void extract_name() {

                auto found = __predicate.find('(');

                if(found != std::string::npos)
                    __name = __predicate.substr(0, found - 1);
                else
                    __name = __predicate;

            }

    };

}