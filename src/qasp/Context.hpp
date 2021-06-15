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

#include <string>
#include <vector>

#include "Program.hpp"


namespace qasp {

    class Context {

        public:

            inline Context(std::string source, std::vector<Program> programs = {}, std::optional<Program> constraint = {})
                : __source(source)
                , __programs(programs)
                , __constraint(constraint) {}


            void prepare();
            void merge() noexcept;



            inline const auto& source() const {
                return this->__source;
            }

            inline const auto& programs() const {
                return this->__programs;
            }

            inline const auto& constraint() const {
                return this->__constraint;
            }

            inline const auto begin() const {
                return this->__programs.begin();
            }

            inline const auto end() const {
                return this->__programs.end();
            }

            inline auto begin() {
                return this->__programs.begin();
            }

            inline auto end() {
                return this->__programs.end();
            }

            inline const auto last() const {
                return std::prev(end());
            }


        private:

            std::string __source;
            std::vector<Program> __programs;
            std::optional<Program> __constraint;

    };

}