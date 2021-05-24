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

#include "Solver.hpp"
#include "../Program.hpp"
#include "../Assumptions.hpp"

#include <string>


namespace qasp::solver {

    class WaspSolver : public Solver {

        public:
            ProgramModel solve(const std::string& ground, const Assumptions& positive, const Assumptions& negative, std::vector<AnswerSet>& output, size_t max_models = 0) const override;

    };

}