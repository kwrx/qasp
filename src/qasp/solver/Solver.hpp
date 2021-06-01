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

#include "../Program.hpp"
#include "../Assumptions.hpp"
#include "../AnswerSet.hpp"

#include <memory>
#include <string>
#include <cassert>


using namespace qasp;

namespace qasp::solver {

    class Solver {

        public:

            struct iterator {

                using iterator_category = std::input_iterator_tag;
                using difference_type   = std::ptrdiff_t;
                using value_type        = AnswerSet;
                using pointer           = value_type*;
                using reference         = value_type&;



                inline iterator(Solver& solver, std::optional<AnswerSet> answer)
                    : __solver(solver)
                    , __answer(std::move(answer)) {}


                inline const auto operator*() const {
                    return *__answer;
                }

                inline const auto operator->() const {
                    return __answer;
                }

                inline const auto& operator++() {
                    return __answer = __solver.enumerate(), *this;
                }

                friend bool operator== (const iterator& a, const iterator& b) {
                    return a.__answer == b.__answer;
                }

                friend bool operator!= (const iterator& a, const iterator& b) {
                    return a.__answer != b.__answer;
                }


                private:

                    Solver& __solver;
                    std::optional<AnswerSet> __answer;
                    

            };


            inline Solver(const std::string& ground, const Assumptions& positive, const Assumptions& negative)
                : __ground(ground)
                , __positive(positive)
                , __negative(negative) {

                    assert(!ground.empty());
                
                }

            virtual ~Solver() = default;
            virtual std::optional<AnswerSet> first() noexcept = 0;
            virtual std::optional<AnswerSet> enumerate() noexcept = 0;



            inline const iterator begin() noexcept {
                return iterator(const_cast<Solver&>(*this), first());
            }

            inline const iterator end() const noexcept {
                return iterator(const_cast<Solver&>(*this), {});
            }

            inline const iterator cbegin() noexcept {
                return begin();
            }

            inline const iterator cend() const noexcept {
                return end();
            }


            inline const auto& ground() const {
                return this->__ground;
            }

            inline const auto& positive() const {
                return this->__positive;
            }

            inline const auto& negative() const {
                return this->__negative;
            }


            static std::unique_ptr<Solver> create(const std::string& ground, const Assumptions& positive, const Assumptions& negative) noexcept;


        private:
            const std::string& __ground;
            const Assumptions& __positive;
            const Assumptions& __negative;


    };

}