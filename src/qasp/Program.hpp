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
#include <string>
#include <vector>

namespace qasp {

    enum ProgramType {
        TYPE_COMMON,
        TYPE_EXISTS,
        TYPE_FORALL,
        TYPE_CONSTRAINTS
    };

    class Program {
        public:
            Program(const qasp::ProgramType type, const std::string& source, const std::vector<Atom>& atoms)
                : __type(type)
                , __source(source)
                , __atoms(atoms)
                , __subprograms({}) {}

            Program(const qasp::ProgramType type, const std::string& source, const std::vector<Atom>& atoms, const std::vector<Program> subprograms)
                : __type(type)
                , __source(source)
                , __atoms(atoms)
                , __subprograms(subprograms) {}

            const auto& source() const {
                return this->__source;
            }

            const auto& type() const {
                return this->__type;
            }

            const auto& atoms() const {
                return this->__atoms;
            }

            auto& atoms() {
                return this->__atoms;
            }

            const auto& subprograms() const {
                return this->__subprograms;
            }

            void generate();

        private:
            ProgramType __type;
            std::string __source;
            std::vector<Atom> __atoms;
            std::vector<Program> __subprograms;

    };

}