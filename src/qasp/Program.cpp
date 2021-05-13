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

#include "Program.hpp"
#include "Assumptions.hpp"
#include "AnswerSet.hpp"
#include "grounder/Grounder.hpp"
#include "solver/Solver.hpp"
#include "utils/Performance.hpp"

#include <iostream>
#include <sstream>
#include <limits>
#include <cassert>


using namespace qasp;
using namespace qasp::grounder;
using namespace qasp::solver;


#define SMODELS_RULE_TYPE_SEPARATOR         0
#define SMODELS_RULE_TYPE_BASIC             1
#define SMODELS_RULE_TYPE_CONSTRAINT        2
#define SMODELS_RULE_TYPE_CHOICE            3
#define SMODELS_RULE_TYPE_WEIGHT            5
#define SMODELS_RULE_TYPE_MINIMIZE          6
#define SMODELS_RULE_TYPE_DISJUNCTIVE       8

#define SMODELS_RULE_BPLUS                  "B+"
#define SMODELS_RULE_BMINUS                 "B-"




const Program& Program::groundize(Assumptions assumptions) { __PERF_TIMING(grounding);

    LOG(__FILE__, INFO) << "Generating ground for program #" << id() << " with:"
                        << " assumptions(" << assumptions << ")" << std::endl;

    assert(!source().empty());
    assert(ground().empty());
    assert(atoms().empty());


    std::ostringstream input;

    input << source()
          << std::endl
          << assumptions;


    std::string output = Grounder::instance()->generate(input.str());
    std::istringstream reader(output);


    auto read = [&] (std::function<void(const atom_index_t& index)> parse) {

        while(reader.good()) {

            atom_index_t index;
            reader >> index;

            if(unlikely(index == SMODELS_RULE_TYPE_SEPARATOR))
                break;

            parse(index);

        }

    };


    // Ignore first declarations
    read([&] (const auto& index) { 
        reader.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    });

    // Parse predicate index map
    read([&] (const auto& index) {

        std::string predicate;
        reader >> predicate;

        LOG(__FILE__, TRACE) << "Extracted from smodels atom with index " << index
                             << " and predicate " << predicate << std::endl;

        this->__atoms.emplace(predicate, Atom { index, predicate });
        this->__atoms_index_offset = std::max(this->__atoms_index_offset, index + 1);

    });
   

    this->__ground = std::move(output);
    this->__assumptions = std::move(assumptions);

    return *this;

}


std::tuple<ProgramModel, std::vector<AnswerSet>> Program::solve(const AnswerSet& answer) const { __PERF_TIMING(solving);
    
    assert(!ground().empty());


    std::vector<AnswerSet> output;

    Assumptions positive;
    Assumptions negative;


    for(const auto& i : answer) {

        if(unlikely(assumptions().contains(i)))
            continue;
            
            
        if(unlikely(atoms().find(i.predicate()) == atoms().end())) {

            LOG(__FILE__, WARN) << "Program #" << id() 
                                << " does not contains {" << i << "}"
                                << " (ignored) " << std::endl;

            continue;

        }

        positive.emplace_back(map_index(i), i.predicate());

    }

    for(const auto& i : assumptions()) {

        if(answer.contains(i))
            positive.emplace_back(map_index(i), i.predicate());
        else
            negative.emplace_back(map_index(i), i.predicate());

    }


    LOG(__FILE__, INFO) << "Generating answer sets for program #" << id() << " with:"
                        << " answer(" << answer << "),"
                        << " assumptions(" << assumptions() << "),"
                        << " positive(" << positive << "),"
                        << " negative(" << negative << ")" << std::endl;
    
    ProgramModel model = Solver::instance()->solve(ground(), positive, negative, output);

    LOG(__FILE__, INFO) << "Generated answer sets for program #" << id() << " are " << &"UNKNOWN  \0COHERENT \0INCOHERENT"[model * 10] << std::endl;



    assert(model != ProgramModel::MODEL_UNKNOWN);

    return { model, output };

}