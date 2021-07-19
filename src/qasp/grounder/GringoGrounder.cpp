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

#if defined(HAVE_GRINGO)

#include "GringoGrounder.hpp"

#include <gringo/ground/program.hh>
#include <gringo/input/programbuilder.hh>
#include <gringo/input/nongroundparser.hh>
#include <gringo/input/program.hh>
#include <gringo/output/output.hh>
#include <clingo/scripts.hh>

#include <iostream>
#include <sstream>
#include <exception>
#include <cassert>

#ifdef __unix__
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#endif

using namespace qasp;
using namespace qasp::grounder;



std::string GringoGrounder::execute(const std::string& source) const {

    std::ostringstream oss {};

    LOG(__FILE__, TRACE) << "Passing sources to GRINGO (" << source.size() << " bytes): " 
                         << std::endl << source << std::endl;



    Gringo::Logger debug = {
#if defined(DEBUG)
        [](const auto warn, const auto message) {
            LOG(__FILE__, TRACE) << "[GRINGO] " << message << std::endl;
        }
#endif
    };


    try {


        bool incremental = false;

#if defined(DEBUG) && DEBUG_LEVEL <= TRACE
        debug.enable(Gringo::Warnings::OperationUndefined,  true);
        debug.enable(Gringo::Warnings::AtomUndefined,       true);
        debug.enable(Gringo::Warnings::VariableUnbounded,   true);
        debug.enable(Gringo::Warnings::FileIncluded,        true);
        debug.enable(Gringo::Warnings::GlobalVariable,      true);
        debug.enable(Gringo::Warnings::Other,               true);
#else
        debug.enable(Gringo::Warnings::OperationUndefined,  false);
        debug.enable(Gringo::Warnings::AtomUndefined,       false);
        debug.enable(Gringo::Warnings::VariableUnbounded,   false);
        debug.enable(Gringo::Warnings::FileIncluded,        false);
        debug.enable(Gringo::Warnings::GlobalVariable,      false);
        debug.enable(Gringo::Warnings::Other,               false);
#endif


        Potassco::TheoryData theory;
        theory.update();
        

        Gringo::Output::OutputBase output(theory, {}, oss, Gringo::Output::OutputFormat::SMODELS, {
#if defined(DEBUG)
            Gringo::Output::OutputDebug::ALL,
#else
            Gringo::Output::OutputDebug::NONE,
#endif
            false, false
        });


        Gringo::Defines defines {};
        Gringo::Input::Program program {};
        Gringo::Ground::Parameters params {};

        Gringo::Input::NongroundProgramBuilder builder { Gringo::g_scripts(), program, output, defines, false };
        Gringo::Input::NonGroundParser parser { builder, incremental };


        {

            parser.pushStream("-", Gringo::gringo_make_unique<std::stringstream>(source), debug);
            parser.parse(debug);

            if(unlikely(debug.hasError()))
                throw std::runtime_error("an error occurred while parsing gringo");


        }

        defines.init(debug);


        output.init(incremental);
        output.beginStep();

        {

            program.rewrite(defines, debug);
            program.check(debug);

            if(unlikely(debug.hasError()))
                throw std::runtime_error("an error occurred while running gringo");


            params.add("base", {});

            program.toGround({ Gringo::Sig("base", 0, {}) }, output.data, debug)
                .ground(params, Gringo::g_scripts(), output, debug);


        }

        output.endStep({ nullptr, 0 });


    } catch(std::exception const& e) {

        LOG(__FILE__, ERROR) << "[CLINGO] Raised exception: " 
                             << e.what() << std::endl;

        throw std::runtime_error("an error occurred while running gringo");

    }

    return oss.str();

}


#endif