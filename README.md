# qasp
[![CMake Test](https://github.com/kwrx/qasp/actions/workflows/cmake-test.yml/badge.svg)](https://github.com/kwrx/qasp/actions/workflows/cmake-test.yml)
[![License: GPL](https://img.shields.io/badge/License-GPL-blue.svg)](/LICENSE) 

## How to install?
To install **qasp** from sources, execute the following commands:
```shell script
$ cmake -DCMAKE_BUILD_TYPE=Release -B build
$ cmake --build build
$ cmake --install build
```
**NOTE:** [CMake](https://cmake.org/) >= 3.11 required.

## Usage
```shell script
$ qasp [OPTIONS] SOURCES...
```
Or read from *stdin*
```shell script
$ cat $SOURCES | qasp
```
**NOTE:** Type ```qasp --help``` for more information.

## Description
Blabla...

## License
Copyright (c) 2021 Antonino Natale. All rights reserved.  

Licensed under the [GPLv3](/LICENSE) license.
