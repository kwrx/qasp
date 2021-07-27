#include "Context.hpp"
#include <qasp/qasp.h>

using namespace qasp;



void Context::prepare() { __PERF_TIMING(context_prepare);


    if(unlikely(programs().empty()))
        throw std::invalid_argument("no programs found");

    if(unlikely(constraint() && constraint()->source().empty()))
        throw std::invalid_argument("constraint program declared but empty");




    Assumptions assumptions {};

    for(auto& program : __programs) {

        program.groundize(assumptions);

        for(const auto& i : program.atoms())
            assumptions.emplace_back(i.second);

    }
    

    if(constraint()) {

        Program c = *constraint();

        c.groundize(assumptions);

        __constraint.reset();
        __constraint.emplace(std::move(c));

    }


}


void Context::merge() noexcept { __PERF_TIMING(context_merging);

    if(constraint()) {
        
        auto program = programs().back();
                
        program.merge(*constraint());
        program.groundize(program.assumptions());

        __programs.pop_back();
        __programs.emplace_back(program);


        LOG(__FILE__, INFO) << "Merged @constraint with program #" << program.id() << std::endl;

    }

}