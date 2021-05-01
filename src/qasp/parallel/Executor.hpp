#pragma once

#include <iostream>
#include <mutex>
#include <atomic>
#include <deque>
#include <thread>
#include <condition_variable>
#include <functional>


namespace qasp::parallel {

    class Executor {
        
        public:

            Executor(size_t size = 8)
                : __size(size)
                , __alive(true) { init(); }

            inline const auto& size() const {
                return __size;
            }

            inline const auto alive() const {
                return __alive.load(std::memory_order_seq_cst);
            }

            void submit(std::function<void()> job) {
                std::scoped_lock<std::mutex> guard(mx_pending);
                pending.emplace_back(std::move(job));
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


            std::deque<std::function<void()>> pending;
            std::deque<std::thread> units;

            std::condition_variable cv_status;

            std::mutex mx_status;
            std::mutex mx_pending;


            void init() {

                for(pid_t id = 0; id < size(); id++) {
                    units.emplace_back(execution_unit, *this, id);
                }

            }


            static void execution_unit(Executor& e, const pid_t id) {

                do {
                    
                    {
                        std::unique_lock<std::mutex> guard(e.mx_status);
                        
                        e.cv_status.wait(guard, [&] {
                            return !e.alive() && e.pending.empty();
                        });

                        if(!e.alive())
                            break;

                    }


                    std::function<void()> job;
                    
                    {
                        std::scoped_lock<std::mutex> guard(e.mx_pending);

                        if(e.pending.empty())
                            continue;

                        job = e.pending.back();
                        e.pending.pop_back();

                    }

                    job();
                    
                } while(e.alive());
            
            }

    };

};