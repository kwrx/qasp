/*                                                                      
 * GPL-3.0 License 
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

#include <iostream>
#include <cassert>
#include <mutex>
#include <unordered_map>

namespace qasp::utils {

    template <typename T, typename D>
    class Cache : private std::unordered_map<T, D> {

        public:

            using std::unordered_map<T, D>::begin;
            using std::unordered_map<T, D>::end;
            using std::unordered_map<T, D>::size;

            

            inline const bool contains(const T& id) noexcept {
#if defined(HAVE_THREADS)
                std::scoped_lock<std::mutex> guard(m_lock);
#endif
                return this->std::unordered_map<T, D>::find(id) != this->std::unordered_map<T, D>::end();
            }


            inline const D& get(const T& id) noexcept {

                assert(contains(id));

#if defined(HAVE_THREADS)
                std::scoped_lock<std::mutex> guard(m_lock);
#endif
                return this->std::unordered_map<T, D>::find(id)->second;

            }


            inline void push(const T& id, D value) noexcept {

#if defined(HAVE_THREADS)
                std::scoped_lock<std::mutex> guard(m_lock);
#endif

                this->std::unordered_map<T, D>::emplace(id, std::move(value));
            }


            template <typename... A>
            inline void emplace(const T& id, A&&... args) noexcept {

#if defined(HAVE_THREADS)
                std::scoped_lock<std::mutex> guard(m_lock);
#endif

                this->std::unordered_map<T, D>::emplace(id, args...);
            }

        private:

#if defined(HAVE_THREADS)
            std::mutex m_lock {};
#endif

    };

}