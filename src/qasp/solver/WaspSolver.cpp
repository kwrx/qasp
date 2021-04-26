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

#include "WaspSolver.hpp"
#include "../Program.hpp"

#if defined(HAVE_WASP)
#include <wasp/WaspFacade.h>
#include <wasp/util/WaspOptions.h>
#endif


#include <iostream>
#include <exception>
#include <mutex>

using namespace qasp;
using namespace qasp::solver;


#if defined(HAVE_WASP)
int EXIT_CODE;
#endif




ProgramResult WaspSolver::solve(const Program& program, const std::vector<Atom>& assumptions) const {

#if defined(HAVE_WASP)

    static std::mutex wasp_options_lock;


    WaspFacade wasp;

    {

        std::scoped_lock<std::mutex> __(wasp_options_lock);
        wasp::Options::setOptions(wasp);
   
    }

    {

        std::istringstream source(program.source());
        wasp.readInput(source);

    }


    std::vector<Literal> literals;
    std::vector<Literal> conflicts;

    for(const auto& i : assumptions) {

        const auto& e = program.atoms().find(i.predicate());
    
        if(likely(e != program.atoms().end()))
            literals.emplace_back(Literal::createLiteralFromInt(e->second.index()));
    
    }


    unsigned res = wasp.solve(literals, conflicts);

    switch(res) {

        case COHERENT:
            return ProgramResult::MODEL_COHERENT;
        case INCOHERENT:
            return ProgramResult::MODEL_INCOHERENT;
        default:
            return ProgramResult::MODEL_UNKNOWN;

    }

#else
    throw std::runtime_error("wasp was not enabled in this build")
#endif


}