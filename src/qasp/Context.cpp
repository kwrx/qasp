#include "Context.hpp"
#include <qasp/qasp.h>

using namespace qasp;



void Context::prepare() { __PERF_TIMING(context_prepare);

    if(unlikely(programs().empty()))
        throw std::invalid_argument("no programs found");

    if(unlikely(constraint() && constraint()->source().empty()))
        throw std::invalid_argument("constraint program declared but empty");
    

    if(constraint()) {


#if defined(HAVE_MODE_LOOK_AHEAD)

        size_t no_ground = 0;

        for(const auto& i : constraint()->predicates())
            no_ground += !i.ground();


        if(no_ground == 0) {

            Assumptions assumptions {};

            for(const auto& i : constraint()->predicates())
                assumptions.emplace_back(-1, i.arity(), static_cast<std::string>(i));  


            Program c = *constraint();

            c.groundize(assumptions);

            __constraint.reset();
            __constraint.emplace(std::move(c));


        } else {

            LOG(__FILE__, WARN) << "Unpredictable constraint: found @constraint program with "
                                << no_ground << " predicates non ground" << std::endl;

        }

#endif


    }

}


void Context::merge() noexcept { __PERF_TIMING(context_merging);

    auto program = programs().back();
            
    program.merge(*constraint());

    __programs.pop_back();
    __programs.emplace_back(program);


    LOG(__FILE__, INFO) << "Merged @constraint with program #" << program.id() << std::endl;

}