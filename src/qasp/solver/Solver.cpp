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

#include "Solver.hpp"
#include "WaspSolver.hpp"
#include "../Assumptions.hpp"

using namespace qasp;


std::unique_ptr<qasp::solver::Solver> qasp::solver::Solver::create(const std::string& ground, const Assumptions& positive, const Assumptions& negative) noexcept {

#if defined(HAVE_WASP)
    return std::make_unique<WaspSolver>(ground, positive, negative);

#elif defined(HAVE_CLASP)
    return std::make_unique<ClaspSolver>(ground, positive, negative);

#else
    #error "missing solver implementation"
#endif

}
