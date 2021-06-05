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

#if defined(HAVE_WASP)

#include "WaspSolver.hpp"
#include "../Program.hpp"
#include "../AnswerSet.hpp"


#include <wasp/WaspFacade.h>
#include <wasp/util/WaspOptions.h>


#include <iostream>
#include <exception>
#include <mutex>


using namespace qasp;
using namespace qasp::solver;


#if defined(HAVE_WASP)
int EXIT_CODE;
#endif



static void wasp_flip_choices(const std::vector<Literal>& assumptions, std::vector<bool>& checked, std::vector<Literal>& choices) {

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



std::optional<AnswerSet> WaspSolver::first() noexcept { __PERF_TIMING(solving);

    static std::mutex wasp_options_lock;


    {
        std::scoped_lock<std::mutex> __(wasp_options_lock);
        wasp::Options::maxModels = UINT32_MAX;
        wasp::Options::setOptions(wasp);
    }

    wasp.disableOutput();
    wasp.disableVariableElimination();
    wasp.attachAnswerSetListener(&this->listener);


    {

        LOG(__FILE__, TRACE) << "Passing sources to WASP (" << ground().size() << " bytes): " 
                             << std::endl << ground() << std::endl;


        std::istringstream source(ground());
        wasp.readInput(source);

    }


    for(const auto& i : positive())
        this->assumptions.emplace_back(i.index(), POSITIVE);
    
    for(const auto& i : negative())
        this->assumptions.emplace_back(i.index(), NEGATIVE);

    for(const auto& i : this->assumptions)
        wasp.freeze(i.getVariable());


    this->choices = std::vector<Literal>(this->assumptions);



    auto& s = wasp.getSolver();

    if(unlikely(!s.preprocessing()))
        return {};


    s.onStartingSolver();

    if(s.solve(this->choices) == INCOHERENT)
        return {};


    return { this->answer };

}

std::optional<AnswerSet> WaspSolver::enumerate() noexcept { __PERF_TIMING(solving);

    auto& s = wasp.getSolver();


    s.getChoicesWithoutAssumptions(choices);

    while(checked.size() < choices.size())
        checked.emplace_back(false);



    wasp_flip_choices(assumptions, checked, choices);

    if(unlikely(choices.size() == assumptions.size()))
        return {};




    s.setComputeUnsatCores(true);

    do {


        s.unrollToZero();
        s.clearConflictStatus();

        if(s.solve(choices) == COHERENT)
            return { this->answer };
            


        const auto* clause = s.getUnsatCore();

        if(unlikely(clause->size() == 0))
            return {};

        
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


        wasp_flip_choices(assumptions, checked, choices);

        if(unlikely(choices.size() == assumptions.size()))
            return {};


    } while(true);


    assert(0 && "unreachable");

}


#endif