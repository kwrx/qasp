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

        Program constraint = *found;

        if(constraint.source().empty())
            throw std::invalid_argument("constraint program declared but empty");


#if defined(HAVE_MODE_LOOK_AHEAD)

        size_t no_ground = 0;

        for(const auto& i : constraint.predicates())
            no_ground += !i.ground();


        if(no_ground == 0) {

            Assumptions assumptions {};

            for(const auto& i : constraint.predicates())
                assumptions.emplace_back(-1, i.arity(), static_cast<std::string>(i));  

            constraint.groundize(assumptions);

        } else {

            LOG(__FILE__, WARN) << "Unpredictable constraint: found @constraint program with "
                                << no_ground << " predicates non ground" << std::endl;

        }

#endif

        this->__constraint.emplace(std::move(constraint));
        
    }


}

bool QaspSolver::check(const AnswerSet& answer) const noexcept { __PERF_TIMING(checkings);

    LOG(__FILE__, INFO) << "Checking coherency for answerset: " << answer << std::endl;

    if(unlikely(!constraint().has_value()))
        return true;


#if defined(HAVE_MODE_LOOK_AHEAD)
    if(constraint()->ground().empty()) {
#endif

        Assumptions assumptions;
        assumptions.insert(assumptions.begin(), answer.begin(), answer.end());

        Program program = constraint().value();
        program.groundize(assumptions);


        return program.solve(answer)->coherent();

#if defined(HAVE_MODE_LOOK_AHEAD)
    } else {

        return constraint()->solve(answer)->coherent();

    }
#endif

}


#if defined(HAVE_MODE_LOOK_AHEAD)

bool QaspSolver::depends(const std::vector<Program>::iterator& chain, const AnswerSet& answer) const noexcept { __PERF_TIMING(depends);

    assert(chain != __program.subprograms().end());
    assert(!answer.empty());


    for(auto it = chain + 1; it != __program.subprograms().end(); it++) {

        if(unlikely(it->type() == TYPE_CONSTRAINTS)) {

            for(const auto& i : it->predicates()) {

                const auto& found = std::find(answer.begin(), answer.end(), i);

                if(i.negative() && found == answer.end())
                    return true; 

            }    

        } else {
            
            for(const auto& i : it->predicates()) {

                const auto& found = std::find(answer.begin(), answer.end(), i);

                if(i.positive() && found != answer.end())
                    return true;

                if(i.negative() && found == answer.end())
                    return true; 

            }

        }

    }




    LOG(__FILE__, TRACE) << "Found an answer set with no dependency: " << answer << std::endl; 

    return false;

}

#endif



void QaspSolver::promote_answer(const AnswerSet& answer) noexcept {


    LOG(__FILE__, TRACE) << "Add to solution answer: " << answer << std::endl;

    if(std::find(__solution.begin(), __solution.end(), answer) == __solution.end()) { __PERF_INC(solutions_found);
        return (void) __solution.emplace_back(answer);
    }


    __PERF_INC(solutions_discarded);

}



bool QaspSolver::check_answer(const std::vector<Program>::iterator& chain, const Program& program, const AnswerSet& answer) const noexcept {

    bool should_not_check = !program.last();

#if defined(HAVE_MODE_LOOK_AHEAD)

        if(unlikely((qasp().options().mode == QASP_SOLVING_MODE_LOOK_AHEAD) && should_not_check))
            should_not_check = depends(chain, answer);

#endif

    return (should_not_check || check(answer));

}


bool QaspSolver::execute(std::vector<Program>::iterator chain, Assumptions assumptions, AnswerSet answer) noexcept { __PERF_TIMING(executions);


    if(unlikely(chain == program().subprograms().end()))
        return true;


    if(unlikely(chain->type() == TYPE_CONSTRAINTS))
        return execute(chain + 1, std::move(assumptions), std::move(answer));




    Program program = (*chain);
    program.groundize(assumptions);



    auto solution = program.solve(answer);

    
    if(likely(solution->coherent())) {


        size_t success = 0;

        for(auto it = solution->begin(); it != solution->end(); ) {


            std::vector<AnswerSet> buffer {};

            for(size_t i = 0; it != solution->end() && i < qasp().options().bufsiz; ++i, ++it)
                buffer.emplace_back(*it);


            Assumptions knowledge(assumptions);

            for(const auto& i : buffer)
                knowledge.insert(knowledge.end(), i.begin(), i.end());



            const auto dispatch = [&] (const auto& buffer) -> bool {

                for(const auto& i : buffer) {


                    if(!check_answer(chain, program, i)) { __PERF_INC(checks_failed);
                    
                        assert(program.type() == TYPE_FORALL 
                            || program.type() == TYPE_EXISTS);

                        
                        if(program.type() == TYPE_FORALL) { 
                            success = 0; 
                            return false;
                        }

                        continue;

                    }


                    if(execute(chain + 1, knowledge, i)) {

                        success++;

                        if(unlikely(chain == __program.subprograms().begin()))
                            promote_answer(i);
                        
                        else if(unlikely(program.type() == TYPE_EXISTS))
                            return false;


                    } else {

                        if(program.type() == TYPE_FORALL) { 
                            success = 0; 
                            return false;
                        }

                    }

                }


                return true;

            };


            if(!dispatch(buffer))
                break;

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

    if(!execute(__program.subprograms().begin()))
        return model(MODEL_INCOHERENT), false;

    return model(MODEL_COHERENT), true;

}
