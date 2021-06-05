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
#include <cstring>


using namespace qasp;
using namespace qasp::parser;


__weak
qasp::Options __qasp_c_options_instance {};

__weak
bool __qasp_quiet__ = false;




std::string Qasp::run() {

    __qasp_quiet__ = !!options().quiet;


#if !defined(HAVE_MODE_LOOK_AHEAD)
    if(unlikely(options().mode == QASP_SOLVING_MODE_LOOK_AHEAD))
        throw std::invalid_argument("missing QASP_SOLVING_MODE_LOOK_AHEAD support");
#endif


    Parser parser(sources());
    Program program = parser.parse(options());

    if(program.subprograms().empty())
        return {};
        

    QaspSolver qasp(*this, program);

    bool coherent = qasp.run();

    __result = qasp.model();



    __PERF_PRINT_ALL();

    if(coherent) {
        
        std::ostringstream output;

        for(const auto& answer : qasp.solution())
            output << answer << std::endl;

        return output.str();

    }


    LOG(__FILE__, ERROR) << "Program is not a valid qasp model: " 
                         << &"UNKNOWN  \0COHERENT \0INCOHERENT"[qasp.model() * 10] << std::endl;

    return "INCOHERENT\n";
    
}




extern "C" int qasp_set_options(qasp_options_t* c_options) {

    static_assert(sizeof(qasp_options_t) == sizeof(qasp::Options));
    assert(c_options);

    return std::memcpy(&__qasp_c_options_instance, c_options, sizeof(__qasp_c_options_instance)), 0;

}

extern "C" int qasp_run(size_t size_of_sources, const char* c_sources[], const char** c_output) {

    assert(c_sources);
    assert(c_output);

    std::vector<std::string> sources;

    size_t i;
    for(i = 0; i < size_of_sources; i++) {

        assert(c_sources[i]);

        sources.emplace_back(c_sources[i]);

    }


    Qasp qasp { sources };
    qasp.options(std::move(__qasp_c_options_instance));

    (*c_output) = strdup(qasp.run().c_str());

    return qasp.result();

}




