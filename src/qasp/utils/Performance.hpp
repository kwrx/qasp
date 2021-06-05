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

#pragma once

#if defined(HAVE_PERFORMANCE)

#include <chrono>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <iomanip>


#define __PERF_INC(stats)                                               \
    qasp::utils::__trace_performance::__counter_##stats++

#define __PERF_TIMING(stats)                                            \
    __PERF_INC(stats);                                                  \
    qasp::utils::__trace_performance_timing __timings_##stats__guard    \
        (&qasp::utils::__trace_performance::__timings_##stats)



#if defined(DEBUG) && defined(LOG)
#define PERF_LOG(a, b) LOG(a, b)
#else
#define PERF_LOG(a, b) std::cerr
#endif


#define __PERF_PADDING 24


#define __PERF_PRINT_ALL() {                                            \
    __PERF_PRINT(running);                                              \
    __PERF_PRINT(grounding);                                            \
    __PERF_PRINT(grounding_cached);                                     \
    __PERF_PRINT(solving);                                              \
    __PERF_PRINT(parsing);                                              \
    __PERF_PRINT(checkings);                                            \
    __PERF_PRINT(depends);                                              \
    __PERF_PRINT(iterations);                                           \
    __PERF_PRINT(executions);                                           \
    __PERF_PRINT(mapping);                                              \
    __PERF_PRINT(answerset_comparing);                                  \
    __PERF_PRINT(solutions_found);                                      \
    __PERF_PRINT(solutions_discarded);                                  \
    __PERF_PRINT(checks_failed);                                        \
}


#define __PERF_PRINT(stats) {                                           \
    if(qasp::utils::__trace_performance::__timings_##stats.empty()) {   \
        PERF_LOG("Performance", INFO) << std::setw(__PERF_PADDING)      \
            << #stats << " => count: "                                  \
            << qasp::utils::__trace_performance::__counter_##stats      \
            << std::endl;                                               \
    } else {                                                            \
        auto min = *std::min_element(                                   \
            qasp::utils::__trace_performance::__timings_##stats.begin(),\
            qasp::utils::__trace_performance::__timings_##stats.end()   \
        );                                                              \
        auto max = *std::max_element(                                   \
            qasp::utils::__trace_performance::__timings_##stats.begin(),\
            qasp::utils::__trace_performance::__timings_##stats.end()   \
        );                                                              \
        auto avg = std::accumulate(                                     \
            qasp::utils::__trace_performance::__timings_##stats.begin(),\
            qasp::utils::__trace_performance::__timings_##stats.end(),  \
            0.0                                                         \
        ) / qasp::utils::__trace_performance::__timings_##stats.size(); \
        auto tot = std::accumulate(                                     \
            qasp::utils::__trace_performance::__timings_##stats.begin(),\
            qasp::utils::__trace_performance::__timings_##stats.end(),  \
            0.0                                                         \
        );                                                              \
        PERF_LOG("Performance", INFO) << std::setw(__PERF_PADDING)      \
            << #stats << " => count: "                                  \
            << qasp::utils::__trace_performance::__counter_##stats      \
            << "; min: " << std::fixed << min << "s"                    \
            << "; max: " << std::fixed << max << "s"                    \
            << "; avg: " << std::fixed << avg << "s"                    \
            << "; tot: " << std::fixed << tot << "s" << std::endl;      \
    }                                                                   \
}


#define PERF_DECL_T(name)                                           \
    static std::vector<double> __timings_##name;                    \
    static std::size_t __counter_##name;

namespace qasp::utils {

    class __trace_performance {
        public:
            PERF_DECL_T(running);
            PERF_DECL_T(grounding);
            PERF_DECL_T(grounding_cached);
            PERF_DECL_T(solving);
            PERF_DECL_T(parsing);
            PERF_DECL_T(checkings);
            PERF_DECL_T(depends);
            PERF_DECL_T(iterations);
            PERF_DECL_T(executions);
            PERF_DECL_T(mapping);
            PERF_DECL_T(answerset_comparing);
            PERF_DECL_T(solutions_found);
            PERF_DECL_T(solutions_discarded);
            PERF_DECL_T(checks_failed);
    };

    class __trace_performance_timing {
        public:
            
            inline  __trace_performance_timing(std::vector<double>* ref) {
                this->ref = ref;
                this->t0 = std::chrono::steady_clock::now();
            }

            inline ~__trace_performance_timing() {
                this->t1 = std::chrono::steady_clock::now();
                auto diff = t1 - t0;
                this->ref->emplace_back((double) std::chrono::duration_cast<std::chrono::microseconds>(diff).count() / 1000000.0);
            }

        private:
            std::vector<double>* ref;
            std::chrono::steady_clock::time_point t0;
            std::chrono::steady_clock::time_point t1;
    };

}

#else
#define __PERF_TIMING(stats)            (void) 0
#define __PERF_INC(stats)               (void) 0
#define __PERF_PRINT(out, stats)        (void) 0
#define __PERF_PRINT_ALL()              (void) 0
#endif