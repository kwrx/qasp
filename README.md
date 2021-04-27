# qasp
[![build](https://github.com/kwrx/qasp/actions/workflows/cmake-build.yml/badge.svg)](https://github.com/kwrx/qasp/actions/workflows/cmake-build.yml)
[![GitHub release (latest by date)](https://img.shields.io/github/v/release/kwrx/qasp)](https://github.com/kwrx/qasp/releases/latest)
[![License: GPL](https://img.shields.io/badge/License-GPL-blue.svg)](/LICENSE) 

## How to install?
To install **qasp** from sources, execute the following commands:
```shell script
$ cmake -DCMAKE_BUILD_TYPE=Release -B build
$ cmake --build build
$ cmake --install build
```
**NOTE:** [CMake](https://cmake.org/) >= 3.11 and [Boost C++ Libraries](https://www.boost.org/) >= 1.72.0 are required.

## Usage
Open your terminal and type:
```shell script
$ qasp [OPTIONS] SOURCES...
```
Or read from *stdin*
```shell script
$ cat SOURCES | qasp
```
**NOTE:** type ```qasp --help``` for more information.

## Example
Given a logic *asp with quantifiers program* ```program.asp```:
```asp
%@exists
a | b.
%@forall
c | d :- a.
d :- b.  
%@constraints
:- c, not d.
```
Calculate if exists an *answer set* of **@exists fragment** such that for each *answer set* of **@forall fragmemt** is a *coherent* solution.
```shell script
$ qasp program.asp
```
## Description
Blabla...

## License
Copyright (C) 2021 Antonino Natale. All rights reserved.  

Licensed under the [GPL-3.0](/LICENSE) license.
