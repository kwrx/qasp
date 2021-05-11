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
#include "utils/Cache.hpp"

#include <qasp/qasp.h>

namespace qasp {
    
    class QaspSolver {
        public:

            QaspSolver(Qasp qasp, Program program)
                : __qasp(std::move(qasp))
                , __program(std::move(program))
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

            inline const auto& qasp() const {
                return this->__qasp;
            }

            bool run();

        private:

            Qasp __qasp;
            Program __program;
            ProgramModel __model;
            std::optional<Program> __constraint;
            std::vector<std::pair<qasp_iteration_t, AnswerSet>> __solution;

            void init();
            bool check(const AnswerSet& answer) const;
            bool execute(qasp_iteration_t, std::vector<Program>::iterator chain, Assumptions assumptions = {}, AnswerSet answer = {});

#if defined(HAVE_ITERATIONS)
            bool prepare_next_iteration(const qasp_iteration_t& iteration, AnswerSet answer);
            bool set_iteration_answer(const qasp_iteration_t& iteration, const AnswerSet& answer);
#endif
            
            bool get_coherent_answer(const Program& program, const std::vector<AnswerSet>& solution, const size_t& max, std::vector<AnswerSet>& coherencies) const;
            size_t get_max_incoherencies(const Program& program, const std::vector<AnswerSet>& solution) const;


            inline void model(const ProgramModel value) {
                this->__model = value;
            }

    };

}