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

    if(unlikely(program().subprograms()[0].type() != TYPE_EXISTS))
        throw std::invalid_argument("fragment #0 must be @exists");


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

bool QaspSolver::check(const AnswerSet& answer) {

    LOG(__FILE__, INFO) << "Checking coherency for answerset(" << answer << ")" << std::endl;

    if(unlikely(!constraint().has_value()))
        return true;
    
    return std::get<0>(constraint().value().solve(answer)) == MODEL_COHERENT;

}



bool QaspSolver::get_coherent_answer(const Program& program, const std::vector<AnswerSet>& solution, std::vector<AnswerSet>& coherencies) {

    size_t incoherencies = 0;
    size_t max = 0;

    switch(program.type()) {

        case TYPE_EXISTS:
            max = solution.size();
            break;

        case TYPE_FORALL:
            max = 1;
            break;

        default:
            assert(0 && "BUG! Invalid Program Type");
            return false;
    }

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


    // TODO: DUBBIO: COERENZA ALLA FINE O NEL MENTRE?
    std::vector<AnswerSet> coherencies;
    
    if(!get_coherent_answer(program, solution, coherencies)) {
     
        LOG(__FILE__, ERROR) << "No sufficient coherent solution found for program #" 
                            << program.id() << std::endl;
     
        return false;
    }



    size_t success = 0;
    assumptions.clear();

    for(const auto& i : coherencies)
        assumptions.insert(assumptions.end(), i.begin(), i.end());

    for(const auto& i : coherencies)
        success += execute(chain + 1, assumptions, i) ? 1 : 0;


    return success > 0;

}


bool QaspSolver::run() {

    if(!execute(__program.subprograms().begin()))
        return model(MODEL_INCOHERENT), false;

    return model(MODEL_COHERENT), true;

}