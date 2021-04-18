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

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <getopt.h>

#include "qasp/parser/Parser.hpp"


static void show_usage(int argc, char** argv) {

    std::cout 
        << "Use: " << QASP_PROGRAM_NAME << " SOURCES\n"
        << "Process qasp SOURCES and blabla...\n\n"
        << "    -v, --verbose               print process information\n"
        << "        --help                  show this help\n"
        << "        --version               print version info and exit\n";

    exit(0);

}


static void show_version(int argc, char** argv) {

    std::cout
        << QASP_PROGRAM_NAME
        << " (asp utils) "
        << QASP_VERSION_MAJOR << "."
        << QASP_VERSION_MINOR << "."
        << QASP_VERSION_PATCH << std::endl
        << "Copyright (c) "
        << (__DATE__ + 7) << " Antonino Natale\n"
        << "License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>.\n"
        << "This is free software: you are free to change and redistribute it.\n"
        << "There is NO WARRANTY, to the extent permitted by law.\n\n"
        << "Built with gcc " << __VERSION__ << " (" 
        << __TIMESTAMP__ << ")\n";

    
    exit(0);

}


int main(int argc, char** argv) {


#ifdef DEBUG

    qasp::parser::Parser parser({ "../../test/source01.asp" });
    parser.parse();

#else
    
    if(argc < 2)
        show_usage(argc, argv);
    
    static struct option long_options[] = {
        { "verbose", no_argument, NULL, 'v' },
        { "help",    no_argument, NULL, 'h' },
        { "version", no_argument, NULL, 'q' },
        { NULL, 0, NULL, 0 }
    };


    bool verbose = false;

    int c, idx;
    while((c = getopt_long(argc, argv, "vhq", long_options, &idx)) != -1) {

        switch(c) {
            case 'v':
                verbose = 1;
                break;
            case 'q':
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


    if(optind >= argc)
        show_usage(argc, argv);

#endif


    return 0;

}