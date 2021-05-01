#pragma once

#include "Program.hpp"
#include "AnswerSet.hpp"
#include "Atom.hpp"

namespace qasp {
    
    class QaspSolver {
        public:

            QaspSolver(Program program)
                : __program(std::move(program))
                , __model(MODEL_UNKNOWN) { init(); }

            inline const auto& program() const {
                return this->__program;
            }

            inline const auto& constraint() const {
                return __constraint;
            }

            inline const auto& solution() const {
                return __solution;
            }

            inline const auto& model() const {
                return __model;
            }

            bool run();

        private:

            Program __program;
            ProgramModel __model;
            std::optional<Program> __constraint;
            std::vector<AnswerSet> __solution;

            void init();
            bool check(const AnswerSet& answer);
            bool execute(std::vector<Program>::iterator chain, Assumptions assumptions = {}, AnswerSet answer = {});

            bool get_coherent_answer(const Program& program, const std::vector<AnswerSet>& solution, std::vector<AnswerSet>& coherencies);

            inline void model(const ProgramModel value) {
                __model = value;
            }

    };

}