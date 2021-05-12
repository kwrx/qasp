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

#include "Performance.hpp"

#if defined(HAVE_PERFORMANCE) && defined(DEBUG)

#include <vector>
#include <cstdint>

using namespace qasp::utils;


#define PERF_VALUE_T(name)                                                  \
    std::vector<double> __trace_performance::__timings_##name = {};         \
    std::size_t __trace_performance::__counter_##name = {};


    PERF_VALUE_T(grounding);
    PERF_VALUE_T(grounding_cached);
    PERF_VALUE_T(solving);
    PERF_VALUE_T(parsing);
    PERF_VALUE_T(running);
    PERF_VALUE_T(checkings);
    PERF_VALUE_T(iterations);
    PERF_VALUE_T(executions);
    PERF_VALUE_T(mapping);
    PERF_VALUE_T(answerset_comparing);
    PERF_VALUE_T(solutions_found);
    PERF_VALUE_T(solutions_discarded);
    PERF_VALUE_T(checks_failed);

#endif