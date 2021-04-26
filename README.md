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
**NOTE:** [CMake](https://cmake.org/) >= 3.11 and [Boost C++ Libraries] >= 1.72.0 are required.

## Usage
```shell script
$ qasp [OPTIONS] SOURCES...
```
Or read from *stdin*
```shell script
$ cat SOURCES | qasp
```
**NOTE:** Type ```qasp --help``` for more information.

## Description
Blabla...

## License
Copyright (C) 2021 Antonino Natale. All rights reserved.  

Licensed under the [GPLv3](/LICENSE) license.
