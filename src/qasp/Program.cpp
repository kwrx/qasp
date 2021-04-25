/*                                                                      
 * GPLv3 License 
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
#include "grounder/Grounder.hpp"
#include <iostream>
#include <sstream>
#include <limits>

using namespace qasp;
using namespace qasp::grounder;


#define SMODELS_RULE_TYPE_SEPARATOR         0
#define SMODELS_RULE_TYPE_BASIC             1
#define SMODELS_RULE_TYPE_CONSTRAINT        2
#define SMODELS_RULE_TYPE_CHOICE            3
#define SMODELS_RULE_TYPE_WEIGHT            5
#define SMODELS_RULE_TYPE_MINIMIZE          6
#define SMODELS_RULE_TYPE_DISJUNCTIVE       8

#define SMODELS_RULE_BPLUS                  "B+"
#define SMODELS_RULE_BMINUS                 "B-"




void Program::generate() {

    LOG(__FILE__, INFO) << "Generating ground instance..." << std::endl;

    std::string output = Grounder::instance()->generate(this->source());
    std::stringstream reader(output);


    while(reader.good()) {
        
        int ch;
        reader >> ch;

        if(unlikely(ch == SMODELS_RULE_TYPE_SEPARATOR))
            break;

        reader.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    }


    while(reader.good()) {

        int64_t index;
        reader >> index;

        if(unlikely(index == SMODELS_RULE_TYPE_SEPARATOR))
            break;


        std::string argument;
        reader >> argument;

        std::stringstream atom(argument);
        std::stringstream name;


        while(atom.good()) {
            
            char ch;
            atom >> ch;

            if(unlikely(ch == '\n'))
                break;
            
            name << ch;

        }



        LOG(__FILE__, TRACE) << "Extracted from smodels atom with index " << index
                             << " and predicate " << name.str() << std::endl;

        this->atoms().emplace(name.str(), Atom { index, name.str() });

    }


}


void Program::solve() {

}