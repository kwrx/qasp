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
#include "../AnswerSet.hpp"

#include <memory>
#include <string>
#include <cassert>


namespace qasp::solver {

    class Solver {

        public:

            struct iterator {

                using iterator_category = std::input_iterator_tag;
                using difference_type   = std::ptrdiff_t;
                using value_type        = AnswerSet;
                using pointer           = value_type*;
                using reference         = value_type&;



                inline iterator(std::shared_ptr<Solver> solver)
                    : __solver(std::move(solver)) { next(); }


                inline const auto operator*() const {
                    return *__ptr;
                }

                inline const auto operator->() const {
                    return __ptr;
                }

                inline const auto& operator++() {
                    return next();
                }

                friend bool operator== (const iterator& a, const iterator& b) {
                    return a.__ptr == b.__ptr;
                }

                friend bool operator!= (const iterator& a, const iterator& b) {
                    return a.__ptr != b.__ptr;
                }



                private:

                    std::shared_ptr<Solver> __solver;
                    value_type __value;
                    pointer __ptr;


                    inline const iterator& next() {

                        if(__solver) {

                            const auto answer = __solver->solve();

                            if(answer.has_value()) {
                                __value = std::move(answer.value());
                                __ptr = &__value;
                            } else {
                                __solver = nullptr;
                                __ptr = nullptr;
                            }

                        } else {
                            __ptr = nullptr;
                        }

                        return *this;

                    }

            };


            inline Solver(const std::string& ground, const Assumptions& positive, const Assumptions& negative)
                : __ground(ground)
                , __positive(positive)
                , __negative(negative) {

                    assert(!ground.empty());
                
                }

            virtual ~Solver() = default;
            virtual std::optional<AnswerSet> solve() const noexcept = 0;



            inline constexpr const iterator& begin() const noexcept {
                return iterator(std::make_shared<Solver>(*this));
            }

            inline constexpr const iterator& end() const noexcept {
                return iterator(nullptr);
            }

            inline constexpr const iterator& cbegin() const noexcept {
                return begin();
            }

            inline constexpr const iterator& cend() const noexcept {
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


        private:
            const std::string& __ground;
            const Assumptions& __positive;
            const Assumptions& __negative;


    };

}