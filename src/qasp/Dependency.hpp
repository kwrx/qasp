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



#define DEPENDENCY_SIGN_UNDEFINED               0
#define DEPENDENCY_SIGN_POSITIVE                1
#define DEPENDENCY_SIGN_NEGATIVE                2


namespace qasp {

    class Dependency {

        public:

            Dependency(std::string predicate, size_t sign)
                : __predicate(std::move(predicate))
                , __arity(0)
                , __sign(sign) {
                      

                    auto found = __predicate.find('(');

                    if(found != std::string::npos) {

                        __arity += 1;
                        __arity += std::count(__predicate.begin(), __predicate.end(), ',');

                        __name = __predicate.substr(0, found - 1);

                    } else {

                        __name = __predicate;

                    }


                    __hash = std::hash<std::string>{}(__name) ^ (std::hash<size_t>{}(__arity) ^ (std::hash<size_t>{}(__sign) << 1) << 1);

                }


            inline const auto& name() const {
                return this->__name;
            }

            inline const auto& arity() const {
                return this->__arity;
            }

            inline const auto& predicate() const {
                return this->__predicate;
            }

            inline const auto& sign() const {
                return this->__sign;
            }

            inline const auto& hash() const {
                return this->__hash;
            }

            inline const bool positive() const {
                return (sign() & DEPENDENCY_SIGN_POSITIVE);
            }

            inline const bool negative() const {
                return (sign() & DEPENDENCY_SIGN_NEGATIVE);
            }


            inline bool operator ==(const Dependency& b) const {
                return this->hash() == b.hash();
            }

            inline bool operator ==(const std::string& predicate) const {
                return this->predicate() == predicate;
            }


        private:
            std::string __predicate;
            std::string __name;
            size_t __arity;
            size_t __sign;
            size_t __hash;

    };


    struct DependencyHash {

        inline std::size_t operator()(const Dependency& b) const {
            return b.hash();
        }

    };

}