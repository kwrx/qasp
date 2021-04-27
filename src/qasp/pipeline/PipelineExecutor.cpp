#if defined(HAVE_THREADS)

#include "PipelineExecutor.hpp"
#include "Pipeline.hpp"

#include <iostream>
#include <cassert>
#include <mutex>
#include <functional>

using namespace qasp;
using namespace qasp::pipeline;


void PipelineExecutor::execution_unit(PipelineExecutor* thiz, unsigned id) {

    assert(thiz);
    assert(thiz->__ex_units.size() > id);
    assert(thiz->__ex_ready.size() > id);


    do {

        {

            std::unique_lock<std::mutex> guard(thiz->__cv_lock);

            thiz->__cv_status.wait(guard, [&] {
                bool expected = true;
                return !thiz->active() || thiz->__ex_ready[id].compare_exchange_strong(expected, false, std::memory_order::memory_order_seq_cst);
            });


            if(unlikely(!thiz->active()))
                break;

        }



        assert(!thiz->__pending.empty());


        // Run pipeline
        thiz->pop_pending().run();


        thiz->__running.fetch_sub(1, std::memory_order_seq_cst);
        thiz->__cv_on_exit.notify_all();

    } while(thiz->active());


}

void PipelineExecutor::init() {

    for(size_t id = 0; id < size(); id++) {
        __ex_ready.emplace_back(false);
        __ex_units.emplace_back(PipelineExecutor::execution_unit, this, id);
    }
    
}


Pipeline PipelineExecutor::pop_pending() {

    std::scoped_lock<std::mutex> guard(__pending_lock);

    assert(!__pending.empty());

    Pipeline& back = __pending.back();
    __pending.pop_back();

    return back;

}





void PipelineExecutor::shutdown() {

    __is_active.store(false, std::memory_order_seq_cst);
    __cv_status.notify_all();

    for(auto& thread : __ex_units)
        thread.join();

}

void PipelineExecutor::join() {

    std::unique_lock<std::mutex> guard(__cv_lock);

    __cv_on_exit.wait(guard, [&] {
        return running() == 0;
    });

}

void PipelineExecutor::submit(Pipeline runnable) {

    std::scoped_lock<std::mutex> guard(__pending_lock);
    __pending.push_back(std::move(runnable));
    
}




#endif