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
#include <mutex>
#include <atomic>
#include <deque>
#include <thread>
#include <condition_variable>
#include <functional>


namespace qasp::utils {

    template <typename R, typename... T>
    class Executor {
        
        public:

            Executor(size_t size = 8)
                : __size(size)
                , __alive(true) { init(); }

            inline const auto& size() const {
                return this->__size;
            }

            inline const auto& alive() const {
                return this->__alive.load(std::memory_order_seq_cst);
            }

            inline auto& submit(std::function<R(T...)> action, T args...) {
                std::scoped_lock<std::mutex> guard(mx_pending);
                return this->pending.emplace_back(std::move(action), std::forward(args)...), *this;
            }

            void shutdown() {

                __alive.store(false, std::memory_order_seq_cst);

                cv_status.notify_all();

                for(auto& unit : units)
                    unit.join();

            }

        private:

            size_t __size;
            std::atomic<bool> __alive;


            std::deque<std::tuple<std::function<R(T...)>, T...> pending {};
            std::deque<std::thread> units {};

            std::condition_variable cv_status {};

            std::mutex mx_status {};
            std::mutex mx_pending {};


            inline void init() {

                for(pid_t id = 0; id < size(); id++) {
                    units.emplace_back(execution_unit, *this, id);
                }

            }


            static inline void execution_unit(Executor& e, const pid_t id) {

                do {
                    
                    {
                        std::unique_lock<std::mutex> guard(e.mx_status);
                        
                        e.cv_status.wait(guard, [&] {
                            return !e.alive() && e.pending.empty();
                        });

                        if(!e.alive())
                            break;

                    }


                    std::function<R(T...)> job;
                    
                    {
                        std::scoped_lock<std::mutex> guard(e.mx_pending);

                        if(e.pending.empty())
                            continue;

                        job = e.pending.back();
                        e.pending.pop_back();

                    }

                    std::get<0>(job)(std::forward<T...>(job)...);
                    
                } while(e.alive());
            
            }

    };

};