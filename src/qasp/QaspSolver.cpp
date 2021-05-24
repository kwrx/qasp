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

        this->__constraint.emplace(std::move(ground));
        
    }


}

bool QaspSolver::check(const AnswerSet& answer) const { __PERF_TIMING(checkings);

    LOG(__FILE__, INFO) << "Checking coherency for answerset(" << answer << ")" << std::endl;

    if(unlikely(!constraint().has_value()))
        return true;


    Assumptions assumptions;
    assumptions.insert(assumptions.begin(), answer.begin(), answer.end());

    Program program = constraint().value();
    program.groundize(assumptions);

#if defined(HAVE_MODE_COUNTER_EXAMPLE)  
    return qasp().options().mode == QASP_SOLVING_MODE_COUNTER_EXAMPLE
                ? std::get<0>(program.solve(answer, 1)) == MODEL_INCOHERENT
                : std::get<0>(program.solve(answer, 1)) == MODEL_COHERENT;
#else
    return std::get<0>(program.solve(answer, 1)) == MODEL_COHERENT;
#endif

}


#if defined(HAVE_MODE_LOOK_AHEAD)

bool QaspSolver::depends(const std::vector<Program>::iterator& chain, const AnswerSet& answer) const {

    assert(chain != __program.subprograms().end());
    assert(!answer.empty());


    for(auto it = chain + 1; it != __program.subprograms().end(); it++) {

        if(unlikely(it->type() == TYPE_CONSTRAINTS))
            continue;

        for(const auto& i : it->predicates()) {

            const auto& found = std::find(answer.begin(), answer.end(), i);

            if(i.positive() && found != answer.end())
                return true;

            if(i.negative() && found == answer.end())
                return true; 

        }

    }


    LOG(__FILE__, TRACE) << "Found an answer set with no dependency: " << answer << std::endl; 

    return false;

}

#endif



void QaspSolver::promote_candidates(const std::vector<AnswerSet>& candidates) {


    auto add = [&] (const auto& answer) { __PERF_INC(solutions_found);

        LOG(__FILE__, TRACE) << "Add to solution answer: " << answer << std::endl;

        if(std::find(__solution.begin(), __solution.end(), answer) == __solution.end())
            return (void) __solution.emplace_back(answer);


         __PERF_INC(solutions_discarded);

    };

    for(const auto& answer : candidates)
        add(answer);

}





size_t QaspSolver::get_max_incoherencies(const Program& program, const std::vector<AnswerSet>& solution) const {

    switch(program.type()) {

        case TYPE_EXISTS:

#if defined(HAVE_MODE_COUNTER_EXAMPLE) 
            return qasp().options().mode == QASP_SOLVING_MODE_COUNTER_EXAMPLE
                ? 1
                : solution.size();
#else
            return solution.size();
#endif

        case TYPE_FORALL:

#if defined(HAVE_MODE_COUNTER_EXAMPLE) 
            return qasp().options().mode == QASP_SOLVING_MODE_COUNTER_EXAMPLE
                ? solution.size()
                : 1;
#else
            return 1;
#endif

        default:
            throw std::runtime_error("invalid Program Type");
            
    }

}


bool QaspSolver::get_coherent_answer(const std::vector<Program>::iterator& chain, const Program& program, const std::vector<AnswerSet>& solution, const size_t& max, std::vector<AnswerSet>& coherencies) const {

    size_t incoherencies = 0;

    for(const auto& s : solution) {

        bool should_not_check = !program.last();


#if defined(HAVE_MODE_LOOK_AHEAD)

        if(unlikely((qasp().options().mode == QASP_SOLVING_MODE_LOOK_AHEAD) && should_not_check))
            should_not_check = depends(chain, s);

#endif

        if(should_not_check || check(s))
            coherencies.emplace_back(std::move(s));
        
        else if(++incoherencies == max)
            return false;

    }

    return true;

}



bool QaspSolver::execute(std::vector<Program>::iterator chain, std::vector<AnswerSet>&& candidates, Assumptions assumptions, AnswerSet answer) { __PERF_TIMING(executions);


    if(unlikely(chain == program().subprograms().end()))
        return candidates.emplace_back(answer), true;


    if(unlikely(chain->type() == TYPE_CONSTRAINTS))
        return execute(chain + 1, std::move(candidates), std::move(assumptions), std::move(answer));




    Program program = (*chain);
    program.groundize(assumptions);



    auto result = program.solve(answer);

    const auto& model = std::get<0>(result);
    const auto& solution = std::get<1>(result);



    if(likely(model == MODEL_COHERENT)) {


        std::vector<AnswerSet> coherencies;
        std::size_t max = get_max_incoherencies(program, solution);
        
        if(!get_coherent_answer(chain, program, solution, max, coherencies)) { __PERF_INC(checks_failed);
        
            LOG(__FILE__, ERROR) << "Not enough coherent solutions were found for program #" 
                                 << program.id() << std::endl;

#if defined(DEBUG) && DEBUG_LEVEL <= TRACE
            for(auto i : coherencies)
                LOG(__FILE__, ERROR) << "Candidate rejected: " << i << std::endl;
#endif
        
            return false;
        }



        std::size_t fail = 0;

        for(const auto& i : coherencies)
            assumptions.insert(assumptions.end(), i.begin(), i.end());

        for(const auto& i : coherencies)
            if((fail += !execute(chain + 1, std::move(candidates), assumptions, i)) >= max)
                return candidates.clear(), false;

    
    } else {

#if defined(HAVE_MODE_COUNTER_EXAMPLE)
        if(qasp().options().mode == QASP_SOLVING_MODE_COUNTER_EXAMPLE) {
            
            if(program.type() == ProgramType::TYPE_FORALL)
                return false;

        } else {
#endif

            if(program.type() != ProgramType::TYPE_FORALL)
                return false;       

#if defined(HAVE_MODE_COUNTER_EXAMPLE)
        }
#endif

        candidates.emplace_back(answer);

    }



    if(unlikely(chain == __program.subprograms().begin()))
        promote_candidates(candidates);

    return true;

}


bool QaspSolver::run() { __PERF_TIMING(running);

    assert(solution().empty());

    if(!execute(__program.subprograms().begin()))
        return model(MODEL_INCOHERENT), false;

    return model(MODEL_COHERENT), true;

}
