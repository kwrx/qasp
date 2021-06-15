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

#include "../utils/Cache.hpp"

#include <memory>
#include <string>


namespace qasp::grounder {

    class Grounder {

        public:
            static std::shared_ptr<Grounder> instance() noexcept;

            virtual ~Grounder() = default;
            std::string generate(const std::string& source);
            std::string generate(const std::string& source, std::string output) noexcept;

            const bool generated(const std::string& source) noexcept;


        protected:
            Grounder() {}
            virtual std::string execute(const std::string& source) const = 0;

        private:
            qasp::utils::Cache<std::size_t, std::string> cache {};


    };

}