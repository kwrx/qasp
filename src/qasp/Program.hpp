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
#include "Assumptions.hpp"
#include "AnswerSet.hpp"
#include "Predicate.hpp"
#include "utils/Performance.hpp"

#include <string>
#include <vector>
#include <tuple>
#include <unordered_map>

namespace qasp {

    enum ProgramType {
        TYPE_COMMON,
        TYPE_EXISTS,
        TYPE_FORALL,
        TYPE_CONSTRAINTS
    };

    enum ProgramModel {
        MODEL_UNKNOWN,
        MODEL_COHERENT,
        MODEL_INCOHERENT
    };

    class Program {
        public:

            Program(pid_t id, const qasp::ProgramType type, const std::string source, const std::vector<Predicate> predicates = {}, const std::vector<Program> subprograms = {})
                : __id(id)
                , __type(type)
                , __source(std::move(source))
                , __subprograms(std::move(subprograms))
                , __predicates(std::move(predicates)) {}


            inline const auto& id() const {
                return this->__id;
            }

            inline const auto& source() const {
                return this->__source;
            }

            inline const auto& type() const {
                return this->__type;
            }

            inline const auto& atoms() const {
                return this->__atoms;
            }

            inline const auto& predicates() const {
                return this->__predicates;
            }

            inline const auto& subprograms() const {
                return this->__subprograms;
            }

            inline auto& subprograms() {
                return this->__subprograms;
            }

            inline const auto& ground() const {
                return this->__ground;
            }

            inline void ground(std::string value) {
                this->__ground = std::move(value);
            }

            inline const auto& assumptions() const {
                return this->__assumptions;
            }

            inline const auto& assumptions(Assumptions assumptions) {
                return this->__assumptions = std::move(assumptions), *this;
            }

            inline const auto& last() const {
                return this->__last;
            }

            inline void last(bool last) {
                this->__last = std::move(last);
            }


            const Program& groundize(Assumptions assumptions = {});
            std::tuple<ProgramModel, std::vector<AnswerSet>> solve(const AnswerSet& answer = {}, const size_t max_models = 0) const;


        private:

            pid_t __id;
            ProgramType __type;
            std::string __source;
            std::string __ground;
            std::vector<Program> __subprograms;
            std::vector<Predicate> __predicates;

            std::unordered_map<std::string, Atom> __atoms {};
            atom_index_t __atoms_index_offset = 0;
            Assumptions __assumptions {};
            bool __last = false;


            inline const atom_index_t map_index(const Atom& atom) const { __PERF_INC(mapping);
                
                const auto& found = atoms().find(atom.predicate());

                if(unlikely(found == atoms().end()))
                    return __atoms_index_offset + atom.index();
                
                return found->second.index();

            }


    };

}