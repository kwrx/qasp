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

    class Atom {
        public:

            Atom(const std::string& name, const std::string& extensions)
                : __index(-1LL)
                , __name(name)
                , __extensions(extensions) {}

            Atom(const int64_t index, const std::string& name, const std::string& extensions)
                : __index(index)
                , __name(name)
                , __extensions(extensions) {}


            const auto& index() const {
                return __index;
            }

            const auto& name() const {
                return __name;
            }

            const auto& extensions() const {
                return __extensions;
            }

            const bool isGround() const {
                return this->index() != -1LL;
            }

        private:
            int64_t __index;
            std::string __name;
            std::string __extensions;
    };

}