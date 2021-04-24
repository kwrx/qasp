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

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cassert>
#include <getopt.h>

#include "qasp/parser/Parser.hpp"



int quiet = false;


static void show_usage(int argc, char** argv) {

    std::cout 
        << "Use: " << QASP_PROGRAM_NAME << " SOURCES\n"
        << "Process qasp SOURCES and blabla...\n\n"
        << "    -q, --quiet                 hide log information\n"
        << "        --help                  show this help\n"
        << "        --version               print version info and exit\n";

    exit(1);

}


static void show_version(int argc, char** argv) {

    std::cout
        << QASP_PROGRAM_NAME
        << " (asp-utils) "
        << QASP_VERSION_MAJOR << "."
        << QASP_VERSION_MINOR << "."
        << QASP_VERSION_PATCH << std::endl
        << "Copyright (c) "
        << (__DATE__ + 7) << " Antonino Natale\n"
        << "License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>.\n"
        << "This is free software: you are free to change and redistribute it.\n"
        << "There is NO WARRANTY, to the extent permitted by law.\n\n"
        << "Built with "
        << QASP_COMPILER_NAME       << " "
        << QASP_COMPILER_VERSION    << " (" 
        << __TIMESTAMP__            << ")\n";

    
    exit(1);

}



int main(int argc, char** argv) {

    
    static struct option long_options[] = {
        { "quiet",   no_argument, NULL, 'q' },
        { "help",    no_argument, NULL, 'h' },
        { "version", no_argument, NULL, 'v' },
        { NULL, 0, NULL, 0 }
    };


    quiet = 0;

    int c, idx;
    while((c = getopt_long(argc, argv, "qhv", long_options, &idx)) != -1) {

        switch(c) {
            case 'q':
                quiet = 1;
                break;
            case 'v':
                show_version(argc, argv);
                break;
            case 'h':
            case '?':
                show_usage(argc, argv);
                break;
            default:
                abort();
        }

    }


    std::vector<std::string> sources;

    if(optind >= argc)
        sources.emplace_back("-");

    else {

        for(; optind < argc; optind++) {

            LOG(__FILE__, TRACE) << "argv(" << optind << "): " 
                                << argv[optind] << std::endl; 

            sources.emplace_back(argv[optind]);

        }

    }
        

    assert(sources.size() > 0);


    try {

        qasp::parser::Parser parser(sources);
        qasp::Program program = parser.parse();

        program.generate();

        /* TODO ... */

    } catch(const std::exception& e) {
        
        std::cerr << QASP_PROGRAM_NAME << ": error: " << e.what() << std::endl;
        exit(1);

    }

    return 0;

}