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
#if defined(HAVE_CLASP)

#include "Solver.hpp"
#include "../Program.hpp"
#include "../Assumptions.hpp"


#include <clasp/clasp_facade.h>
#include <string>
#include <sstream>

namespace qasp::solver {

    using namespace Clasp;

    class ClaspSolver : public Solver {

        public:

            ClaspSolver(const std::string& ground, const Assumptions& positive, const Assumptions& negative)
                : Solver(ground, positive, negative) {

                    //config.solve.enumMode = EnumOptions::enum_user;
                    config.solve.numModels = 0;

                    this->__first = first();

                }


            std::optional<AnswerSet> first() noexcept override;
            std::optional<AnswerSet> enumerate() noexcept override;

        private:

            ClaspFacade clasp {};
            ClaspConfig config {};
            LitVec assumptions {};
            AnswerSet answer {};

            std::unique_ptr<ClaspFacade::SolveHandle> handle {};


            const AnswerSet& commit(const OutputTable& output, const Model& model);

    };

}


#endif