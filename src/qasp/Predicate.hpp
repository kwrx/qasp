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

#include "Atom.hpp"


namespace qasp {

    class Predicate {
        public:

            Predicate(const std::string name, const std::size_t arity)
                : __name(std::move(name))
                , __arity(arity) {}

            Predicate(const std::string name, const std::string& extensions)
                : __name(std::move(name))
                , __arity(1) {

                    __arity += std::count(extensions.begin(), extensions.end(), ',');

                }


            inline const auto& arity() const {
                return this->__arity;
            }

            inline const auto& name() const {
                return this->__name;
            }

            inline bool operator ==(const Predicate& b) const {
                return this->name()  == b.name() 
                    && this->arity() == b.arity();
            }

            inline bool operator ==(const Atom& b) const {
                return this->name()  == b.predicate() && // FIXME: Calcolo nome atomo
                       this->arity() == b.arity();
            }

            inline friend std::ostream& operator <<(std::ostream& os, const Predicate& p) {
                return os << p.name(), os;
            }


        private:
            std::string __name;
            std::size_t __arity;

    };

}