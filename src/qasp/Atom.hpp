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

namespace qasp {

    typedef int64_t atom_index_t;

    class Atom {
        public:

            Atom(const atom_index_t index, const std::string& name)
                : __index(index)
                , __name(name) {}


            const auto& index() const {
                return __index;
            }

            const auto& name() const {
                return __name;
            }

        private:
            atom_index_t __index;
            std::string __name;

    };

}