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
#include <sstream>

#include "Atom.hpp"


#define PREDICATE_POSITIVE           1
#define PREDICATE_NEGATIVE          -1



namespace qasp {

    class Predicate {
        public:


            Predicate(const std::string name, const std::string& extensions, const int sign)
                : __name(std::move(name))
                , __arity(0)
                , __sign(sign) {

                    __arity += !extensions.empty();
                    __arity += std::count(extensions.begin(), extensions.end(), ',');
                    __ground = std::find_if(extensions.begin(), extensions.end(), [] (const auto& i) { return isupper(i); }) == extensions.end();
                    
                    __hash = std::hash<std::string>() (
                        __name
                        + std::to_string(__arity)
                        + std::to_string(__sign)
                    );

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

            inline const auto& ground() const {
                return this->__ground;
            }

            inline const auto& hash() const {
                return this->__hash;
            }

            inline const bool positive() const {
                return this->__sign >= 0;
            }

            inline const bool negative() const {
                return this->__sign < 0;
            }





            inline bool operator ==(const Predicate& b) const noexcept {
                return this->name()  == b.name() 
                    && this->arity() == b.arity()
                    && this->sign()  == b.sign();
            }

            inline operator std::string() const {
                std::ostringstream ss;
                ss << *this;
                return ss.str();
            }

            inline operator std::size_t() const {
                return this->hash();
            }


            inline friend std::ostream& operator <<(std::ostream& os, const Predicate& p) {
                
                os << p.name();

                if(p.arity() > 0) {

                    os << "(";

                    for(size_t i = 0; i < p.arity() - 1; i++)
                        os << "u___,";

                    os << "u___)";

                }

                return os;

            }


        private:
            std::string __name;
            std::size_t __arity;
            std::size_t __hash;
            int __sign;
            bool __ground;
            

    };

}