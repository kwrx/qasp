#include <iostream>
#include <cstdlib>
#include <getopt.h>


static void show_usage(int argc, char** argv) {

    std::cout 
        << "Use: " << argv[0] << " SOURCES\n"
        << "Process qasp SOURCES and blabla...\n\n"
        << "    -v, --verbose               print process information\n"
        << "        --help                  show this help\n"
        << "        --version               print version info and exit\n";

    exit(0);

}


static void show_version(int argc, char** argv) {

    std::cout
        << argv[0]
        << " (asp utils) "
        << QASP_VERSION_MAJOR << "."
        << QASP_VERSION_MINOR << "."
        << QASP_VERSION_PATCH << std::endl
        << "Copyright (c) "
        << (__DATE__ + 7) << " Antonino Natale.\n"
        << "Built with gcc " << __VERSION__ << " (" 
        << __TIMESTAMP__ << ")\n";
    
    exit(0);

}


int main(int argc, char** argv) {
    
    if(argc < 2)
        show_usage(argc, argv);
    
    static struct option long_options[] = {
        { "verbose", no_argument, NULL, 'v'},
        { "help", no_argument, NULL, 'h'},
        { "version", no_argument, NULL, 'q'},
        { NULL, 0, NULL, 0 }
    };


    bool verbose = false;

    int c, idx;
    while((c = getopt_long(argc, argv, "z", long_options, &idx)) != -1) {

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


    std::cout << "Hello World!" << std::endl;
    return 0;

}