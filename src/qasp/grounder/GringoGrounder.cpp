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

#include "GringoGrounder.hpp"
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


std::string GringoGrounder::generate(const std::string& source) const {

    std::stringstream output;


#if defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 200112L

    int fd[2];  

    if(unlikely(pipe(fd) < 0))
        throw std::runtime_error("pipe() failed!");

    if(unlikely(fcntl(fd[0], F_SETFL, fcntl(fd[0], F_GETFL) | O_NONBLOCK) < 0))
        throw std::runtime_error("fcntl() failed!");
    
    if(unlikely(fcntl(fd[1], F_SETFL, fcntl(fd[1], F_GETFL) | O_NONBLOCK) < 0))
        throw std::runtime_error("fcntl() failed!");
    

    assert(fcntl(fd[0], F_GETFL) & O_NONBLOCK);
    assert(fcntl(fd[1], F_GETFL) & O_NONBLOCK);



    pid_t pid;
    if(unlikely((pid = fork()) < 0))
        throw std::runtime_error("fork() failed!");

    
    if(pid == 0) {

        dup2(fd[0], STDIN_FILENO);
        dup2(fd[1], STDOUT_FILENO);

        char* const argv[] = {
            (char*) "gringo", 
            (char*) "--output=smodels", NULL
        };

        exit(execvp(argv[0], argv));

    } else {

        if(unlikely(write(fd[1], source.c_str(), source.size()) < 0))
            throw std::runtime_error("write() failed!");

        if(unlikely(close(fd[1]) < 0))
            throw std::runtime_error("close() failed!");

        if(unlikely(waitpid(pid, NULL, 0) < 0))
            throw std::runtime_error("waitpid() failed!");


        char buffer[256];
        ssize_t size;

        while((size = read(fd[0], buffer, sizeof(buffer))) > 0)
            output.write(buffer, size);

        assert(size == 0);
        

        if(unlikely(close(fd[0]) < 0))
            throw std::runtime_error("close() failed!");

    }

#else
#error "missing a non POSIX compliant implementation"
#endif


    return output.str();

}