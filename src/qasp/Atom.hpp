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

namespace qasp {

    typedef int64_t atom_index_t;

    class Atom {
        public:

            Atom(const atom_index_t index, const std::string predicate)
                : __index(index)
                , __predicate(std::move(predicate)) {}


            inline const auto& index() const {
                return __index;
            }

            inline const auto& index(atom_index_t value) {
                return __index = value, *this;
            }

            inline const auto& predicate() const {
                return __predicate;
            }

            inline bool operator ==(const Atom& b) const {
                return this->predicate() == b.predicate();
            }

            inline friend std::ostream& operator <<(std::ostream& os, const Atom& a) {
                return os << a.predicate(), os;
            }

        private:
            atom_index_t __index;
            std::string __predicate;

    };

}