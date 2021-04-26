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

using namespace qasp::solver;


static std::shared_ptr<Solver> __instance;

std::shared_ptr<Solver> Solver::instance() {

    if(unlikely(!__instance)) {
#if defined(HAVE_WASP)
        __instance = std::make_shared<WaspSolver>();
#elif defined(HAVE_CLASP)
        __instance = std::make_shared<ClaspSolver>();
#else
#   error "missing solver application"
#endif
    }

    return __instance;

}