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



#if defined(HAVE_WASP_ASSUMPTIONS)

static inline void wasp_flip_choices(const std::vector<Literal>& assumptions, std::vector<bool>& checked, std::vector<Literal>& choices) {

    assert(choices.size() >= assumptions.size());
    assert(checked.size() >= assumptions.size());
    assert(checked.size() == choices.size());

    while(choices.size() > assumptions.size()) {

        if(checked[choices.size() - 1]) {

            choices.pop_back();
            checked.pop_back();

        } else
            break;

    }

    if(unlikely(choices.size() == assumptions.size()))
        return;


    choices[choices.size() - 1] = choices[choices.size() - 1].getOppositeLiteral();
    checked[choices.size() - 1] = true;

}


static unsigned wasp_enumeration(WaspFacade& wasp, const std::vector<Literal>& assumptions) {

    auto& s = wasp.getSolver();
    
    if(unlikely(!s.preprocessing()))
        return INCOHERENT;


    std::vector<Literal> choices(assumptions);
    std::vector<bool> checked;


    s.onStartingSolver();

    if(s.solve(choices) == INCOHERENT)
        return INCOHERENT;



    s.getChoicesWithoutAssumptions(choices);

    if(unlikely(choices.size() == assumptions.size()))
        return COHERENT;



    while(checked.size() < choices.size())
        checked.emplace_back(false);

    wasp_flip_choices(assumptions, checked, choices);

    



    s.setComputeUnsatCores(true);

    for(;;) {

        s.unrollToZero();
        s.clearConflictStatus();


        if(s.solve(choices) == INCOHERENT) {

            const auto* clause = s.getUnsatCore();

            if(unlikely(clause->size() == 0))
                return INCOHERENT;

            
            assert(choices.empty() > assumptions.size());
            assert(checked.empty() > assumptions.size());

            if(s.getCurrentDecisionLevel() == 0 || clause->size() == 1) {

                assert(choices.size() == checked.size());

                size_t i, k;
                for(i = k = 0; i < choices.size(); i++) {

                    choices[k] = choices[i];
                    checked[k] = checked[i];

                    if(s.getDecisionLevel(choices[i]) != 0)
                        k++;

                }

                assert(choices.size() == checked.size());

                choices.resize(k);
                checked.resize(k);

            }


        } else {

            s.getChoicesWithoutAssumptions(choices);

            while(checked.size() < choices.size())
                checked.emplace_back(false);

        }


        wasp_flip_choices(assumptions, checked, choices);

        if(unlikely(choices.size() == assumptions.size()))
            return COHERENT;
        

    }

}


#endif



ProgramModel WaspSolver::solve(const std::string& ground, const Assumptions& positive, const Assumptions& negative, std::vector<AnswerSet>& output) const {

    assert(!ground.empty());
    assert( output.empty());


#if defined(HAVE_WASP)

    static std::mutex wasp_options_lock;


#if defined(HAVE_WASP_RESET)
    VariableNames::reset();
#endif

    WaspFacade wasp;

    auto listener = std::make_unique<WaspAnswerSetListener>(wasp, output);

    {

        std::scoped_lock<std::mutex> __(wasp_options_lock);
        wasp::Options::maxModels = UINT32_MAX;
        wasp::Options::setOptions(wasp);
        wasp.disableOutput();
        wasp.attachAnswerSetListener(listener.get());
   
    }

   
#if defined(HAVE_WASP_ASSUMPTIONS)

    {

        LOG(__FILE__, TRACE) << "Passing sources to WASP: " << std::endl
                             << ground << std::endl;

        std::istringstream source(ground);
        wasp.readInput(source);

    }

    std::vector<Literal> literals;


    for(const auto& i : positive)
        literals.emplace_back(i.index(), POSITIVE);
    
    for(const auto& i : negative)
        literals.emplace_back(i.index(), NEGATIVE);

    for(const auto& i : literals)
        wasp.freeze(i.getVariable());


    unsigned res = wasp_enumeration(wasp, literals);

#else

    {

        std::stringstream source;

        for(const auto& i : positive) 
            source << "1 " << i.index() << " 0 0" << std::endl;
        
        for(const auto& i : negative)
            source << "1 1 1 0 " << i.index() << std::endl;

        source << ground;


        LOG(__FILE__, TRACE) << "Passing sources to WASP: " << std::endl
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