#pragma once

#if defined(HAVE_THREADS)

#include "Pipeline.hpp"

#include <condition_variable>
#include <thread>
#include <atomic>
#include <deque>
#include <mutex>
#include <functional>


namespace qasp::pipeline {

    class PipelineExecutor {

        public:

            PipelineExecutor(size_t pool_size)
                : __is_active(true)
                , __pool_size(pool_size) {
                    this->init();
                }

            ~PipelineExecutor() {
                shutdown();
            }

            inline const auto& size() const {
                return __pool_size;
            }

            inline const auto running() const {
                return __running.load(std::memory_order_seq_cst);
            }

            inline const bool active() const {
                return __is_active.load(std::memory_order_seq_cst);
            }


            void submit(Pipeline runnable);
            void shutdown();
            void join();


        private:

            std::deque<Pipeline> __pending;
            std::mutex __pending_lock;

            std::deque<std::thread> __ex_units;
            std::deque<std::atomic<bool>> __ex_ready;

            std::condition_variable __cv_status;
            std::condition_variable __cv_on_exit;
            std::mutex __cv_lock;

            std::atomic<uint_fast32_t> __running;
            std::atomic<bool> __is_active;

            size_t __pool_size;


            void init();
            Pipeline pop_pending();

            static void execution_unit(PipelineExecutor* instance, unsigned id);

    };

}

#endif