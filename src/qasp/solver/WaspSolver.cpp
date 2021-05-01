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
#include "../AnswerSet.hpp"


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


class WaspAnswerSetListener : public AnswerSetListener {

    public:

        WaspAnswerSetListener(const WaspFacade& __wasp, std::vector<AnswerSet>& __answersets)
            : wasp(__wasp)
            , answersets(__answersets) {}

        ~WaspAnswerSetListener() = default;


        inline void foundAnswerSet() override {
            
            AnswerSet answer;

            for(size_t i = 1; i <= wasp.getSolver().numberOfAssignedLiterals(); i++) {

                if(wasp.isUndefined(i))
                    continue;

                if(wasp.isFalse(i))
                    continue;

                if(VariableNames::isHidden(i))
                    continue;

                answer.emplace_back(i, VariableNames::getName(i));

            }

            LOG(__FILE__, TRACE) << "<LISTENER> Found an answer set: " << answer << std::endl;

            answersets.emplace_back(answer);

        }


    private:
        const WaspFacade& wasp;
        std::vector<AnswerSet>& answersets;

};



ProgramModel WaspSolver::solve(const std::string& ground, const Assumptions& positive, const Assumptions& negative, std::vector<AnswerSet>& output) const {

    assert(!ground.empty());
    assert( output.empty());


#if defined(HAVE_WASP)

    static std::mutex wasp_options_lock;


    WaspFacade wasp;

    auto listener = make_unique<WaspAnswerSetListener>(wasp, output);

    {

        std::scoped_lock<std::mutex> __(wasp_options_lock);
        wasp::Options::maxModels = UINT32_MAX;
        wasp::Options::setOptions(wasp);
        wasp.disableOutput();
        wasp.attachAnswerSetListener(listener.get());
   
    }

   
#if defined(HAVE_WASP_ASSUMPTIONS)

    {

        std::istringstream source(ground);
        wasp.readInput(source);

    }

    std::vector<Literal> literals;
    std::vector<Literal> conflicts;

    for(const auto& i : positive)
        literals.emplace_back(Literal(i.index(), POSITIVE));
    
    for(const auto& i : negative)
        literals.emplace_back(Literal(i.index(), NEGATIVE));

    for(const auto& i : literals)
        wasp.freeze(i.getVariable());


    // TODO: Add enumeration backtracking for assumptions

    unsigned res = wasp.solve(literals, conflicts);

#else

    {

        std::stringstream source;

        for(const auto& i : positive) 
            source << "1 " << i.index() << " 0 0" << std::endl;
        
        for(const auto& i : negative)
            source << "1 1 1 0 " << i.index() << std::endl;

        source << ground;


        LOG(__FILE__, TRACE) << "Passing source to WASP: " << std::endl
                             << source.str() << std::endl;

        wasp.readInput(source);

    }

    wasp.solve();

    unsigned res = output.size() > 0        // FIXME
        ? COHERENT
        : INCOHERENT;


#endif
    

    switch(res) {

        case COHERENT:
            return ProgramModel::MODEL_COHERENT;
        case INCOHERENT:
            return ProgramModel::MODEL_INCOHERENT;
        default:
            return ProgramModel::MODEL_UNKNOWN;

    }

#else
    throw std::runtime_error("wasp was not enabled in this build")
#endif


}