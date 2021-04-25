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
#include "../Program.hpp"


#define ANNOTATION_COMMON           "common"
#define ANNOTATION_EXISTS           "exists"
#define ANNOTATION_FORALL           "forall"
#define ANNOTATION_CONSTRAINTS      "constraints"

#define TK_ANNOTATION               '@'
#define TK_SOURCE                   '.'


namespace qasp::parser {

    class Parser {
        public:
            Parser(const std::vector<std::string>& sources)
                : __sources(std::move(sources)) {}

            const auto& sources() const {
                return this->__sources;
            }

            qasp::Program parse() const;

        private:
            std::vector<std::string> __sources;

    };

}