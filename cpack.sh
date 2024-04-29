#!/bin/bash
install -d pack
pushd pack
cmake -DCMAKE_BUILD_TYPE=Release -G Ninja ..
ninja && \
ninja package
popd
