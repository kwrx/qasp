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

#pragma once
#if defined(HAVE_WASP)

#include "Solver.hpp"
#include "../Program.hpp"
#include "../Assumptions.hpp"

#include <wasp/WaspFacade.h>
#include <string>


namespace qasp::solver {


    class WaspAnswerSetListener : public AnswerSetListener {

        public:

            WaspAnswerSetListener(WaspFacade& __wasp, AnswerSet& __answer)
                : wasp(__wasp)
                , answer(__answer) {}

            ~WaspAnswerSetListener() {
                wasp.removeAnswerSetListener(this);
            };


            inline void foundAnswerSet() override {

                answer.clear();

                for(size_t i = 1; i <= wasp.getSolver().numberOfAssignedLiterals(); i++) {
                    
                    if(wasp.isUndefined(i))
                        continue;

                    if(wasp.isFalse(i))
                        continue;

                    if(wasp.getVariableNames().isHidden(i))
                        continue;

                    answer.emplace_back(i, wasp.getVariableNames().getName(i));

                }

                answer.sort();

                LOG(__FILE__, TRACE) << "<LISTENER> Found an answer set: " << answer << std::endl;

            }


        private:
            WaspFacade& wasp;
            AnswerSet& answer;

    };

    class WaspSolver : public Solver {

        public:

            WaspSolver(const std::string& ground, const Assumptions& positive, const Assumptions& negative)
                : Solver(ground, positive, negative)
                , listener(wasp, answer) {

                    this->__first = first();

                }


            std::optional<AnswerSet> first() noexcept override;
            std::optional<AnswerSet> enumerate() noexcept override;

        private:

            WaspAnswerSetListener listener;
            WaspFacade wasp {};

            AnswerSet answer {};

            std::vector<Literal> assumptions {};
            std::vector<Literal> choices {};
            std::vector<bool> checked {};

        
    };

}

#endif