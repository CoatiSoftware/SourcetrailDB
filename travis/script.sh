#!/bin/bash

mkdir build
cd build
cmake -DBUILD_BINDINGS_PYTHON=$PYTHON_BINDING ..
make
./core/test_core
