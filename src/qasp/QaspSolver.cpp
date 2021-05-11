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

#include "QaspSolver.hpp"
#include "grounder/Grounder.hpp"
#include "solver/Solver.hpp"
#include "utils/Performance.hpp"

#include <qasp/qasp.h>
#include <iostream>
#include <cassert>
#include <vector>
#include <tuple>

using namespace qasp;
using namespace qasp::grounder;
using namespace qasp::solver;


void QaspSolver::init() {

    if(unlikely(program().subprograms().empty()))
        throw std::invalid_argument("no programs found");


    const auto& found = std::find_if(program().subprograms().begin(), program().subprograms().end(), [] (const auto& i) {
        return i.type() == TYPE_CONSTRAINTS;
    });

    if(found != program().subprograms().end()) {

        Program ground = *found;

        if(ground.source().empty())
            throw std::invalid_argument("constraint program declared but empty");

        ground.groundize();
        

        Assumptions assumptions;
        for(const auto& i : ground.atoms())
            assumptions.emplace_back(i.second);

        ground.assumptions(assumptions);


        this->__constraint.emplace(ground);
        
    }


}

bool QaspSolver::check(const AnswerSet& answer) const { __PERF_TIMING(checkings);

    LOG(__FILE__, INFO) << "Checking coherency for answerset(" << answer << ")" << std::endl;

    if(unlikely(!constraint().has_value()))
        return true;
    
    return std::get<0>(constraint().value().solve(answer)) == MODEL_COHERENT;

}



size_t QaspSolver::get_max_incoherencies(const Program& program, const std::vector<AnswerSet>& solution) const {

    switch(program.type()) {

        case TYPE_EXISTS:
            return solution.size();

        case TYPE_FORALL:
            return 1;

        default:
            throw std::runtime_error("invalid Program Type");
            
    }

}


bool QaspSolver::get_coherent_answer(const Program& program, const std::vector<AnswerSet>& solution, const size_t& max, std::vector<AnswerSet>& coherencies) const {

    size_t incoherencies = 0;

    for(const auto& s : solution) {

        if(check(s))
            coherencies.emplace_back(std::move(s));
        
        else if(++incoherencies == max)
            return false;

    }

    return true;

}



#if defined(HAVE_ITERATIONS)

bool QaspSolver::prepare_next_iteration(const qasp_iteration_t& iteration, AnswerSet answer) { __PERF_TIMING(iterations);


    if(unlikely(iteration + 1 >= qasp().options().iterations))
        return true;

    if(unlikely(answer.empty()))
        return true;
    

    Assumptions knownlegde;

    for(const auto& i : answer)
        knownlegde.emplace_back(i);


    LOG(__FILE__, INFO)  << "Running new iteration #" << iteration + 1
                         << " with new knownlegde " << knownlegde << std::endl;

    

    return execute(iteration + 1, __program.subprograms().begin(), std::move(knownlegde), std::move(answer));

}


bool QaspSolver::set_iteration_answer(const qasp_iteration_t& iteration, const AnswerSet& answer) {

    if(likely(iteration > 0)) {

        const auto& found = std::find_if(__solution.begin(), __solution.end(), [&] (const auto& i) {
                
            if(i.first >= iteration)
                return false;

            return i.second == answer;

        });


        if(unlikely(found != __solution.end()))
            return false;

    }
    

    return __solution.emplace_back(iteration, answer), true;

}


#endif



bool QaspSolver::execute(qasp_iteration_t iteration, std::vector<Program>::iterator chain, Assumptions assumptions, AnswerSet answer) { __PERF_TIMING(executions);


    if(unlikely(chain == program().subprograms().end())) {

#if defined(HAVE_ITERATIONS)
        
        if(unlikely(!set_iteration_answer(iteration, answer)))
            return true;

        return prepare_next_iteration(iteration, answer);

#else
        return __solution.emplace_back(iteration, answer), true;
#endif

    }


    if(unlikely(chain->type() == TYPE_CONSTRAINTS))
        return execute(iteration, chain + 1, assumptions, answer);



    

#if defined(HAVE_ITERATIONS)

    Program program = (*chain);
    program.groundize(assumptions);

#else

    Program& program = (*chain);

    if(unlikely(program.ground().empty()))
        program.groundize(assumptions);

#endif

    auto result = program.solve(answer);

    const auto& model = std::get<0>(result);
    const auto& solution = std::get<1>(result);


    if(model != MODEL_COHERENT)
        return false;


    std::vector<AnswerSet> coherencies;
    std::size_t max = get_max_incoherencies(program, solution);
    
    if(!get_coherent_answer(program, solution, max, coherencies)) {
     
        LOG(__FILE__, ERROR) << "Not enough coherent solutions were found for program #" 
                             << program.id() << std::endl;
     
        return false;
    }



    std::size_t fail = 0;

    for(const auto& i : coherencies)
        assumptions.insert(assumptions.end(), i.begin(), i.end());

    for(const auto& i : coherencies)
        if((fail += !execute(iteration, chain + 1, assumptions, i)) >= max)
            return false;

    return true;

}


bool QaspSolver::run() { __PERF_TIMING(running);

    assert(solution().empty());

    if(!execute(0, __program.subprograms().begin()))
        return model(MODEL_INCOHERENT), false;

    return model(MODEL_COHERENT), true;

}
