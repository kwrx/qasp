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

    __context.prepare();


    switch(context().last()->type()) {

        case ProgramType::TYPE_EXISTS:

            if(context().programs().size() > 1)
                __context.merge();

            break;

        case ProgramType::TYPE_FORALL:

#if defined(HAVE_MODE_COUNTER_EXAMPLE)
            if(qasp().options().mode & QASP_SOLVING_MODE_COUNTER_EXAMPLE)
                __context.merge();
#endif

            break;

        default:
            throw std::logic_error("BUG: invalid program type");

    }  

}



bool QaspSolver::check(const AnswerSet& answer) const noexcept { __PERF_TIMING(checkings);

    LOG(__FILE__, INFO) << "Checking coherency for answerset: " << answer << std::endl;

    if(unlikely(!context().constraint()))
        return true;


    return context().constraint()->solve(answer)->coherent();

}


#if defined(HAVE_MODE_LOOK_AHEAD)

bool QaspSolver::depends(const std::vector<Program>::iterator& chain, const AnswerSet& answer) const noexcept { __PERF_TIMING(depends);

    assert(chain != std::prev(context().end()));
    assert(!answer.empty());


    for(auto it = chain + 1; it != context().end(); it++) {

       for(const auto& i : it->predicates()) {

            const auto& found = std::find(answer.begin(), answer.end(), i);

            if(i.positive() && found != answer.end())
                return true;

            if(i.negative() && found == answer.end())
                return true; 

        }

    }


    if(context().constraint()) {

        for(const auto& i : context().constraint()->predicates()) {

            const auto& found = std::find(answer.begin(), answer.end(), i);

            if(i.negative() && found == answer.end())
                return true; 

        }

    }




    LOG(__FILE__, TRACE) << "Found an answer set with no dependency: " << answer << std::endl; 

    return false;

}

#endif



bool QaspSolver::promote_answer(const AnswerSet& answer) noexcept { __PERF_INC(solutions_found);

    if(unlikely(__solution.size() >= qasp().options().models))
        return false;


    LOG(__FILE__, TRACE) << "Add to solution answer: " << answer << std::endl;

    return __solution.emplace_back(answer), true;

}



bool QaspSolver::check_answer(const std::vector<Program>::iterator& chain, const AnswerSet& answer) const noexcept {


    bool should_not_check = (chain != std::prev(context().end()));

#if defined(HAVE_MODE_LOOK_AHEAD)

        if(unlikely((qasp().options().mode & QASP_SOLVING_MODE_LOOK_AHEAD) && should_not_check))
            should_not_check = depends(chain, answer);

#endif

    return (should_not_check || check(answer));

}


bool QaspSolver::execute(std::vector<Program>::iterator chain, Assumptions assumptions, AnswerSet answer) noexcept { __PERF_TIMING(executions);


    if(unlikely(chain == context().end()))
        return true;



    Program program = (*chain);    

#if defined(HAVE_MODE_COUNTER_EXAMPLE)

    if(program.merged() && (program.type() == TYPE_FORALL))
        program.rewrite();

#endif



    auto solution = program.solve(answer);

    
    if(likely(solution->coherent())) {


        if(unlikely(program.merged()))
            return program.type() == TYPE_EXISTS;



        size_t success = 0;

        for(auto it = solution->begin(); it != solution->end(); ++it) {


            if(!check_answer(chain, *it)) { __PERF_INC(checks_failed);
                    
                assert(program.type() == TYPE_FORALL 
                    || program.type() == TYPE_EXISTS);

                
                if(program.type() == TYPE_FORALL) { 
                    success = 0; 
                    break;
                }

                continue;

            }


            if(execute(chain + 1, assumptions, *it)) {

                success++;

                if(unlikely(chain == context().begin())) {
                 
                    if(!promote_answer(*it))
                        break;

                } else if(unlikely(program.type() == TYPE_EXISTS))
                    break;


            } else {

                if(program.type() == TYPE_FORALL) { 
                    success = 0; 
                    break;
                }

            }

        }


        if(unlikely(success == 0)) {

            LOG(__FILE__, ERROR) << "Not enough coherent solutions were found for program #" 
                                 << program.id() << std::endl;

            return false;

        }
        

    } else {

        if(program.type() != ProgramType::TYPE_FORALL)
            return false;    

    }


    return true;


}


bool QaspSolver::run() { __PERF_TIMING(running);

    assert(solution().empty());


    if(!execute(__context.begin()))
        return model(MODEL_INCOHERENT), false;



    { __PERF_TIMING(solutions_check);

        std::sort(__solution.begin(), __solution.end());
        std::unique(__solution.begin(), __solution.end());
    
    }

    return model(MODEL_COHERENT), true;

}
