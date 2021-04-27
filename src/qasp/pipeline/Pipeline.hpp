#pragma once

#include "../Program.hpp"
#include "../Atom.hpp"

#include <iostream>
#include <vector>


namespace qasp::pipeline {

    class Pipeline {
        public:

            Pipeline(qasp::Program program, std::vector<Atom> assumptions = {})
                : __program(std::move(program))
                , __assumptions(std::move(assumptions)) {}


            inline const auto& assumptions() const {
                return __assumptions;
            }

            inline const auto& program() const {
                return __program;
            }


            void run();

        private:
            qasp::Program __program;
            std::vector<Atom> __assumptions;

    };

}