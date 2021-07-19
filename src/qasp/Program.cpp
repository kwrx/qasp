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

#define SMODELS_PREDICATE_CONSTRAINT        1



void Program::merge(const Program& other) noexcept {

    assert(!other.source().empty());
    
    assert(other.type() == ProgramType::TYPE_EXISTS
        || other.type() == ProgramType::TYPE_CONSTRAINTS);


    std::ostringstream source {};
    source << this->source();
    source << other.source();



    for(const auto& i : other.predicates())
        this->__predicates.emplace_back(i);

    this->__source = source.str();
    this->__merged = true;
    

}


const Program& Program::groundize(Assumptions assumptions) { __PERF_TIMING(grounding);

    LOG(__FILE__, INFO) << "Generating ground for program #" << id() << " with:"
                        << " assumptions(" << assumptions << ")" << std::endl;

    assert(!source().empty());


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


const Program& Program::rewrite() noexcept { __PERF_TIMING(rewriting);

    assert(!source().empty());
    assert(!ground().empty());



    std::ostringstream input;

    input << source()
          << std::endl
          << assumptions()
          << "\n%% #qasp:rewritten\n";



    if(Grounder::instance()->generated(input.str())) { __PERF_INC(rewriting_cached);
        
        this->__ground = Grounder::instance()->generate(input.str());
        this->__rewritten = true;
    

    } else {


        std::istringstream reader(ground());
        std::ostringstream output {};

        atom_index_t constraint = this->__atoms_index_offset++;


        auto read = [&] (std::function<void(const atom_index_t& index)> parse) {

            while(reader.good()) {

                atom_index_t index;
                reader >> index;

                if(unlikely(index == SMODELS_RULE_TYPE_SEPARATOR))
                    break;

                parse(index);

            }

        };


        // Rewrite constraint rules
        read([&] (const auto& index) {

            output << index;
        

            switch(index) {
            
                case SMODELS_RULE_TYPE_BASIC: {


                    atom_index_t predicate;
                    reader >> predicate;

                    atom_index_t literals;
                    reader >> literals;


                    if(literals && predicate == SMODELS_PREDICATE_CONSTRAINT) {

                        output << " "
                               << constraint;

                    } else {

                        output << " "
                               << predicate;

                    }


                    output << " "
                           << literals;


                }
                
                default:
                    break;

            }


            while(reader.peek() != '\n' && reader.good())
                output.put(reader.get());

            output << std::endl;

            
        });



        output << SMODELS_RULE_TYPE_BASIC       << " "
               << SMODELS_PREDICATE_CONSTRAINT  << " "
               << 1                             << " "
               << 1                             << " "
               << constraint                    << std::endl;

        output << SMODELS_RULE_TYPE_SEPARATOR;


        
        assert(reader.good());

        while(reader.peek() && reader.good())
            output.put(reader.get());



        LOG(__FILE__, TRACE) << "Rewritten program #" << id()
                            << ":\n" << output.str() << std::endl;


        this->__ground = Grounder::instance()->generate(input.str(), std::move(output.str()));
        this->__rewritten = true;

    }

    return *this;

}


std::unique_ptr<Solver> Program::solve(const AnswerSet& answer) const noexcept {
    
    assert(!ground().empty());


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

        positive.emplace_back(map_index(i), i.arity(), i.predicate());

    }

    for(const auto& i : assumptions()) {

        if(answer.contains(i))
            positive.emplace_back(map_index(i), i.arity(), i.predicate());
        else
            negative.emplace_back(map_index(i), i.arity(), i.predicate());

    }


    LOG(__FILE__, INFO) << "Generating answer sets for program #" << id() << " with:"
                        << " answer(" << answer << "),"
                        << " assumptions(" << assumptions() << "),"
                        << " positive(" << positive << "),"
                        << " negative(" << negative << "),"
                        << " rewritten(" << rewritten() << ")" << std::endl;


    return Solver::create(ground(), positive, negative);

}