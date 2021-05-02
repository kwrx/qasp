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

#include <iostream>
#include <cassert>
#include <vector>
#include <tuple>

using namespace qasp;
using namespace qasp::grounder;
using namespace qasp::solver;


void QaspSolver::init() {

    if(unlikely(program().subprograms().empty()))
        throw std::invalid_argument("empty fragments collection");

    // if(unlikely(program().subprograms()[0].type() != TYPE_EXISTS))
    //     throw std::invalid_argument("fragment #0 must be @exists");


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

bool QaspSolver::check(const AnswerSet& answer) const {

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


bool QaspSolver::execute(std::vector<Program>::iterator chain, Assumptions assumptions, AnswerSet answer) {


    if(unlikely(chain == program().subprograms().end()))
        return __solution.emplace_back(answer), true;

    if(unlikely((*chain).type() == TYPE_CONSTRAINTS))
        return execute(chain + 1, assumptions, answer);



    Program& program = (*chain);

    if(unlikely(program.ground().empty()))
        program.groundize(assumptions);


    auto result = program.solve(answer);

    const auto& model = std::get<0>(result);
    const auto& solution = std::get<1>(result);


    if(model != MODEL_COHERENT)
        return false;


    std::vector<AnswerSet> coherencies;
    std::size_t max = get_max_incoherencies(program, solution);
    
    if(!get_coherent_answer(program, solution, max, coherencies)) {
     
        LOG(__FILE__, ERROR) << "No sufficient coherent solution found for program #" 
                             << program.id() << std::endl;
     
        return false;
    }



    std::size_t fail = 0;

    for(const auto& i : coherencies)
        assumptions.insert(assumptions.end(), i.begin(), i.end());

    for(const auto& i : coherencies)
        if((fail += !execute(chain + 1, assumptions, i)) >= max)
            return false;

    return true;

}


bool QaspSolver::run() {

    if(!execute(__program.subprograms().begin()))
        return model(MODEL_INCOHERENT), false;

    return model(MODEL_COHERENT), true;

}
