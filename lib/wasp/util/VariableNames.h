/*
 *
 *  Copyright 2013 Mario Alviano, Carmine Dodaro, and Francesco Ricca.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

#ifndef WASP_VARIABLENAMES_H
#define WASP_VARIABLENAMES_H

#if defined(__qasp__)
#include <thread>
#endif

#include <cassert>
#include <iostream>
#include <vector>
#include "WaspConstants.h"
using namespace std;

class VariableNames
{
    public:

        bool isHidden( Var v );
        const string& getName( Var v );
        void setName( Var v, string name );
        void setToBePrinted( Var v );
        bool hasToBePrinted( Var v );
        void addVariable();
        const vector< string >& getVariables();
        
    private:

        vector< string > variables;
        bool toBePrinted;
        
};

#endif
