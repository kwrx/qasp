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

#include "GringoGrounder.hpp"
#include <iostream>
#include <sstream>
#include <exception>

#ifdef __unix__
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#endif

using namespace qasp;
using namespace qasp::grounder;


std::string GringoGrounder::generate(const std::string& source) const {

    std::stringstream output;

#ifdef __unix__

    int fd[2];  

    if(pipe2(fd, O_NONBLOCK) == -1)
        throw std::runtime_error("pipe2() failed!");
    
    pid_t pid;
    if((pid = fork()) == -1)
        throw std::runtime_error("fork() failed!");

    
    if(pid == 0) {

        dup2(fd[0], 0);
        dup2(fd[1], 1);

        char* const argv[] = {
            (char*) "gringo", 
            (char*) "--output=smodels", NULL
        };

        exit(execvp(argv[0], argv));

    } else {

        write(fd[1], source.c_str(), source.size());
        close(fd[1]);

        if(waitpid(pid, NULL, 0) == -1)
            throw std::runtime_error("wait() failed!");


        char buffer[256];
        ssize_t size;

        while((size = read(fd[0], buffer, sizeof(buffer))))
            output.write(buffer, size);

        close(fd[0]);

    }

#else
#error "TODO: define non unix implementation"
#endif

    return output.str();

}