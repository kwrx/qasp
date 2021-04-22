/*                                                                      
 * GPL3 License 
 *
 * Author(s):                                                              
 *      Antonino Natale <antonio.natale97@hotmail.com>  
 * 
 * 
 * Copyright (C) 2021 Antonino Natale
 *
 * This file is part of qasp.  
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


void Program::generate() {

    LOG(__FILE__, INFO) << "Generating ground instance..." << std::endl;

    std::string output = Grounder::instance()->generate(this->source());
    std::stringstream reader(output);


    while(reader.good()) {
        
        char ch;
        reader >> ch;

        if(ch == '0')
            break;

        reader.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    }


    while(reader.good()) {

        uint64_t index;
        reader >> index;

        if(index == 0ULL)
            break;

        std::string argument;
        reader >> argument;

        std::stringstream atom(argument);
        std::stringstream name;
        std::stringstream extensions;

        while(atom.good()) {
            
            char ch;
            atom >> ch;

            if(ch == '(')
                break;
            
            name << ch;

        }

        while(atom.good()) {

            char ch;
            atom >> ch;

            if(ch == ')')
                break;
            
            extensions << ch;

        }


        LOG(__FILE__, TRACE) << "Extracted from smodels atom with index " << index
                             << " and atom " << name.str() 
                             << "(" << extensions.str() << ")" << std::endl;

        this->atoms().emplace_back(index, name.str(), extensions.str());

    }


}