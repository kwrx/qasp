# Quantifying over ASP
[![build](https://github.com/kwrx/qasp/actions/workflows/cmake-build.yml/badge.svg)](https://github.com/kwrx/qasp/actions/workflows/cmake-build.yml)
[![GitHub release (latest by date)](https://img.shields.io/github/v/release/kwrx/qasp)](https://github.com/kwrx/qasp/releases/latest)
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.5425783.svg)](https://doi.org/10.5281/zenodo.5425783)
[![License: GPL](https://img.shields.io/badge/License-GPL-blue.svg)](/LICENSE) 

For my bachelor's thesis in logic programming and artificial intelligence, I developed a tool that can be used to solve a wide range of problems in answer set programming, particularly those involving the use of quantifiers. This can be useful in a variety of applications, such as planning and scheduling, decision making, and artificial intelligence. Overall, my project aims to demonstrate the power and versatility of logic programming for solving complex problems.

In addition to its versatility, my project also aims to be highly performant compared to the current state of the art in answer set programming. This means that I will be focusing on developing a model that is efficient in terms of both time and space complexity, making it well-suited for solving large and complex problems. 

Furthermore, I have also included a number of heuristics in my model that are designed to significantly improve its performance. These heuristics are based on my research into the latest advancements in answer set programming, and are intended to help the solver find solutions more quickly and accurately. By leveraging these heuristics, my project aims to provide a more efficient and effective solution to problems in answer set programming, compared to existing methods.

## :page_facing_up: Abstract
We present here, a new ASP solver called **Quantifying over ASP**, or briefly **QASP**, for ASP(Q) problems resolution, i.e. an extensive ASP formalism, inspired by *Quantified Boolean Formula*, which adds quantifiers over stable models, in order to model every PSPACE problem.
We describe the key components of this computational methodology by introducing all technical and formal aspects needed to a proper understanding of this thesis.
Moreover, we present resolution algorithms and we observe how through rewriting and source analysis, we can reduce computational size of a given ASP(Q) problem by using alternative strategies like **counter-example** or **look-ahead**.
Furthermore, we present the results of an experimental analysis we conducted in order to evaluate performance-wise impact. Specifically, given benchmarks of our system with respect to current state-of-art ASP(Q) solver *q_asp*, we observe how QASP can be very competitive in most cases, and even faster in other cases.


## :hammer_and_wrench: Build from sources
To build **qasp** from sources, execute the following commands:
```shell script
# Clone repository
$ git clone https://github.com/kwrx/qasp && cd qasp
$ git submodule update --init --recursive

# Configure
$ mkdir -p build
$ cmake -DCMAKE_BUILD_TYPE=Release -B build .

# Build
$ cmake --build build -t clingo
$ cmake --build build

# Install
$ sudo cmake --install build
```
**NOTE:** [CMake](https://cmake.org/) >= 3.20, [Boost C++ Libraries](https://www.boost.org/) >= 1.72.0 and [Re2c](https://re2c.org/) >= 2.2 are required.

## :computer: Usage 
Open your terminal and type:
```shell script
$ qasp [OPTIONS] SOURCES...
```
Or read from *stdin*
```shell script
$ cat SOURCE | qasp [OPTIONS]
```
**NOTE:** type ```qasp --help``` for more information.

## :books: Example
Given a logic *asp with quantifiers program* ```program.asp```:
```asp
%@exists
a | b.
%@forall
c | d :- a.
d :- b.  
%@constraint
:- c, not d.
```
Calculate if exists all *answer set* of **@exists** program such that for each *answer set* of **@forall** program is a *coherent* solution.
```shell script
$ qasp -n0 program.asp
```

## :globe_with_meridians: License
Copyright (C) 2021 Antonino Natale. All rights reserved.  

Licensed under the [GPL-3.0](/LICENSE) license.
