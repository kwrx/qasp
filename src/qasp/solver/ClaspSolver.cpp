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

#if defined(HAVE_CLASP)

#include "ClaspSolver.hpp"
#include "../Program.hpp"
#include "../AnswerSet.hpp"


#include <iostream>
#include <sstream>
#include <exception>
#include <clasp/solver.h>


using namespace qasp;
using namespace qasp::solver;

using namespace Clasp;



const AnswerSet& ClaspSolver::commit(const OutputTable& output, const Model& model) {

    answer.clear();

    for(auto it = output.fact_begin(); it != output.fact_end(); ++it) {

        answer.emplace_back(-1, std::string(it->c_str()));

    }

    for(auto it = output.pred_begin(); it != output.pred_end(); ++it) {

        if(!model.isTrue(it->cond))
            continue;

        answer.emplace_back(it->cond.var(), std::string(it->name.c_str()));

    }

    LOG(__FILE__, TRACE) << "<LISTENER> Found an answer set: " << answer << std::endl;

    return answer;
    
}


std::optional<AnswerSet> ClaspSolver::first() noexcept {


    {

        LOG(__FILE__, TRACE) << "Passing sources to CLASP (" << ground().size() << " bytes): " 
                             << std::endl << ground() << std::endl;

        std::istringstream source(ground());

        clasp.start(this->config, source);
        clasp.read();
        clasp.prepare();

    }


    
    if(unlikely(!clasp.ok()))
        return {};


    for(const auto& i : positive())
        this->assumptions.push_back(Literal(i.index(), false));
    
    for(const auto& i : negative())
        this->assumptions.push_back(Literal(i.index(), true));

    for(const auto& i : assumptions)
        clasp.ctx.setFrozen(i.var(), true);



    handle = std::make_unique<ClaspFacade::SolveHandle>(clasp.solve(Clasp::SolveMode_t::Yield, assumptions));

    if(!this->handle->model())
        return {};


    return { commit(clasp.ctx.output, *this->handle->model()) };

}

std::optional<AnswerSet> ClaspSolver::enumerate() noexcept {
    
    assert(this->handle);

    if(!this->handle->next())
        return {};

    return { commit(clasp.ctx.output, *this->handle->model()) };

}


#endif