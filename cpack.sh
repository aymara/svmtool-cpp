#!/bin/bash
install -d pack
pushd pack
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j2 && \
make package
popd
