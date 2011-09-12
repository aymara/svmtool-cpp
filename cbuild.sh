#!/bin/bash
install -d build
pushd build
cmake -DCMAKE_INSTALL_PREFIX=$1 -DCMAKE_BUILD_TYPE=Debug ..
make -j2 && make install 
return_code=$?
popd
exit $return_code
