/*                                                                      
 * GPL3 License 
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
#include "Atom.hpp"
#include "QaspSolver.hpp"
#include "parser/Parser.hpp"

#include <qasp/qasp.h>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <cassert>


using namespace qasp;
using namespace qasp::parser;


std::string Qasp::run() {

    Parser parser(sources());
    Program program = parser.parse();

    if(program.subprograms().empty())
        return {};
        

    QaspSolver qasp(*this, program);

    bool result = qasp.run();



    __PERF_PRINT_ALL();

    if(result) {
        
        std::ostringstream output;

        for(const auto& answer : qasp.solution())
            output << answer.second << std::endl;

        return output.str();

    }


    LOG(__FILE__, ERROR) << "Program is not a valid qasp model: " 
                         << &"UNKNOWN  \0COHERENT \0INCOHERENT"[qasp.model() * 10] << std::endl;

    return "INCOHERENT\n";
    
}



__weak
bool __qasp_quiet__ = false;


