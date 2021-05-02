/*                                                                      
 * GPL-3.0 License 
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

#include "Program.hpp"
#include "AnswerSet.hpp"
#include "Atom.hpp"

namespace qasp {
    
    class QaspSolver {
        public:

            QaspSolver(Program program)
                : __program(std::move(program))
                , __model(MODEL_UNKNOWN) { init(); }

            inline const auto& program() const {
                return this->__program;
            }

            inline const auto& constraint() const {
                return this->__constraint;
            }

            inline const auto& solution() const {
                return this->__solution;
            }

            inline const auto& model() const {
                return this->__model;
            }

            bool run();

        private:

            Program __program;
            ProgramModel __model;
            std::optional<Program> __constraint;
            std::vector<AnswerSet> __solution;

            void init();
            bool check(const AnswerSet& answer) const;
            bool execute(std::vector<Program>::iterator chain, Assumptions assumptions = {}, AnswerSet answer = {});

            size_t get_max_incoherencies(const Program& program, const std::vector<AnswerSet>& solution) const;
            bool get_coherent_answer(const Program& program, const std::vector<AnswerSet>& solution, const size_t& max, std::vector<AnswerSet>& coherencies) const;

            inline void model(const ProgramModel value) {
                this->__model = value;
            }

    };

}