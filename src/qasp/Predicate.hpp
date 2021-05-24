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

#include <iostream>
#include <string>
#include <algorithm>

#include "Atom.hpp"


#define PREDICATE_POSITIVE          1
#define PREDICATE_NEGATIVE          -1



namespace qasp {

    class Predicate {
        public:

            Predicate(const std::string name, const std::size_t arity, const int sign)
                : __name(std::move(name))
                , __arity(arity)
                , __sign(sign) {}

            Predicate(const std::string name, const std::string& extensions, const int sign)
                : __name(std::move(name))
                , __arity(0)
                , __sign(sign) {

                    __arity += !extensions.empty();
                    __arity += std::count(extensions.begin(), extensions.end(), ',');

                }


            inline const auto& arity() const {
                return this->__arity;
            }

            inline const auto& name() const {
                return this->__name;
            }

            inline const auto& sign() const {
                return this->__sign;
            }

            inline const bool positive() const {
                return __sign >= 0;
            }

            inline const bool negative() const {
                return __sign < 0;
            }

            inline bool operator ==(const Predicate& b) const {
                return this->name()  == b.name() 
                    && this->arity() == b.arity();
            }

            inline friend std::ostream& operator <<(std::ostream& os, const Predicate& p) {
                
                os << p.name();

                if(p.arity() > 0) {

                    os << "(";

                    for(size_t i = 0; i < p.arity() - 1; i++)
                        os << "_,";

                    os << "_)";

                }

                return os;

            }


        private:
            std::string __name;
            std::size_t __arity;
            int __sign;

    };

}