#pragma once

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

#include <string>

namespace qasp::parser {

    enum ProgramType {
        TYPE_EXISTS,
        TYPE_FOREACH,
        TYPE_CONSTRAINTS
    };

    class Program {
        public:
            Program(const std::string& source, const qasp::parser::ProgramType type)
                : __source(std::move(source))
                , __type(type) {}

            const auto& source() const {
                return this->__source;
            }

            const auto& type() const {
                return this->__type;
            }

        private:
            std::string __source;
            ProgramType __type;

    };

}