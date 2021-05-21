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

#ifndef _QASP_H
#define _QASP_H

#include <stdint.h>

#define QASP_OPTIONS_DEFAULT_QUIET          0
#define QASP_OPTIONS_DEFAULT_CPUS           8
#define QASP_OPTIONS_DEFAULT_MODE           QASP_SOLVING_MODE_REGULAR

#define QASP_RESULT_COHERENT                1
#define QASP_RESULT_INCOHERENT              2
#define QASP_RESULT_UNKNOWN                 0

#define QASP_SOLVING_MODE_REGULAR           0
#define QASP_SOLVING_MODE_COUNTER_EXAMPLE   1



#if defined(__cplusplus)

#include <vector>
#include <string>

namespace qasp {

    struct Options {

        uint16_t quiet = QASP_OPTIONS_DEFAULT_QUIET;
        uint16_t cpus  = QASP_OPTIONS_DEFAULT_CPUS;
        uint16_t mode  = QASP_OPTIONS_DEFAULT_MODE;

    };

    class Qasp {

        public:
            
            Qasp(std::vector<std::string> sources)
                : __sources(std::move(sources)) {}

            Qasp(std::string source)
                : __sources({ std::move(source) }) {}

            Qasp()
                : __sources({ "-" }) {}

        
            /**
             * @brief Set options to current qasp instance
             */
            inline void options(qasp::Options&& options) {
                __options = std::move(options);
            }

            inline const qasp::Options& options() const {
                return __options;
            }

            inline const std::vector<std::string>& sources() const {
                return __sources;
            }

            inline const int& result() const {
                return __result;
            }

            std::string run();

        private:
            std::vector<std::string> __sources;
            qasp::Options __options;
            int __result;
        
    };

}

#endif


#if defined(__cplusplus)
extern "C" {
#endif

typedef struct {
    uint16_t cpus;
    uint16_t quiet;
    uint16_t mode;
} qasp_options_t;

extern int qasp_set_options(qasp_options_t* options);
extern int qasp_run(size_t size_of_sources, const char* c_sources[], const char** c_output);

#if defined(__cplusplus)
}
#endif


#endif