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

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <cassert>
#include <csignal>
#include <getopt.h>

#include <qasp/qasp.h>



static void show_usage(int argc, char** argv) {

    std::cout 
        << "Use: " << QASP_PROGRAM_NAME << " [OPTIONS] SOURCES...\n"
        << "Process qasp SOURCES and blabla...\n\n"
        << "    -j N, --parallel=N          allow N jobs at once.\n"
        << "    -q, --quiet                 hide log information.\n"
        << "        --help                  print this message and exit.\n"
        << "        --version               print version info and exit.\n";

    exit(EXIT_FAILURE);

}


static void show_version(int argc, char** argv) {

    std::cout
        << QASP_PROGRAM_NAME
        << " (asp-utils) "
        << QASP_VERSION_MAJOR << "."
        << QASP_VERSION_MINOR << "."
        << QASP_VERSION_PATCH << "."
        << QASP_VERSION_TWEAK << "\n"
        << "Copyright (C) "
        << (&__DATE__[7]) << " Antonino Natale\n"
        << "This is free software; see the source for copying conditions.  There is NO\n"
        << "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\n"
        << "Built with "
        << QASP_COMPILER_NAME       << " "
        << QASP_COMPILER_VERSION    << "(" 
        << __TIMESTAMP__            << ")\n";

    
    exit(EXIT_FAILURE);

}


static void sighandler(int sig) {

    switch(sig) {

        case SIGINT:
        case SIGTERM:
#if defined(__unix__)
        case SIGPWR:
        case SIGQUIT:
#endif
            std::cerr << "Killed: Bye!" << std::endl;
            break;

#if defined(__unix__)
        case SIGXCPU:
            std::cerr << "Killed: CPU time limit exceeded" << std::endl;
            break;
#endif

        default:
            return;

    }


    exit(sig);

}


int main(int argc, char** argv) {


    std::signal(SIGINT,  sighandler);
    std::signal(SIGTERM, sighandler);

#if defined(__unix__)
    std::signal(SIGPWR,  sighandler);
    std::signal(SIGQUIT, sighandler);
    std::signal(SIGXCPU, sighandler);
#endif    



    static struct option long_options[] = {
        { "quiet",      no_argument, NULL, 'q' },
        { "parallel",   required_argument, NULL, 'j' },
        { "help",       no_argument, NULL, 'h' },
        { "version",    no_argument, NULL, 'v' },
        { NULL, 0, NULL, 0 }
    };


    qasp::Options options;

    int c, idx;
    while((c = getopt_long(argc, argv, "qj:hv", long_options, &idx)) != -1) {

        switch(c) {
            case 'q':
                options.quiet = 1;
                break;
            case 'j':
                options.cpus = atoi(optarg);
                break;
            case 'v':
                show_version(argc, argv);
                break;
            case 'h':
            case '?':
                show_usage(argc, argv);
                break;
            default:
                return EXIT_FAILURE;
        }

    }


    if(unlikely(options.cpus < 1)) {
        options.cpus = std::numeric_limits<decltype(options.cpus)>().max();
    }



    std::vector<std::string> sources;

    if(optind >= argc)
        sources.emplace_back("-");

    else {

        for(; optind < argc; optind++) {

            LOG(__FILE__, INFO) << "argv(" << optind << "): " 
                                << argv[optind] << std::endl; 

            sources.emplace_back(argv[optind]);

        }

    }
        

    assert(sources.size() > 0);


    try {


        qasp::Qasp qasp(sources);
        qasp.options(std::move(options));
        
        if(!qasp.options().quiet)
            std::cout << QASP_PROGRAM_GREETINGS << "\n\n";
        
        std::cout << qasp.run();


    } catch(const std::exception& e) {
        
        std::cerr << QASP_PROGRAM_NAME << ": error: " << e.what() << std::endl;
        return EXIT_FAILURE;

    }


    return EXIT_SUCCESS;

}
