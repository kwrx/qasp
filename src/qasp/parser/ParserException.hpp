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

#include <exception>
#include <string>
#include <sstream>


class ParserException : public std::exception {

    public:
        ParserException(const std::string source, const uint32_t line, const uint32_t column, const char value)
            : __source(std::move(source))
            , __line(line)
            , __column(column)
            , __value(value) {

                std::ostringstream ss;
                ss << "unexpected character: " << (isprint(value) ? value : '?')
                   << " in " << source
                   << " at " << line << ":" << column << std::endl;

                __what = ss.str();

            }


    const auto& value() const {
        return __value;
    }

    const auto& source() const {
        return __source;
    }

    const auto& line() const {
        return __line;
    }

    const auto& column() const {
        return __column;
    }

    const char* what() const noexcept override {
        return __what.c_str();
    }


    private:
        std::string __what;
        std::string __source;
        uint32_t __line;
        uint32_t __column;
        char __value; 

};