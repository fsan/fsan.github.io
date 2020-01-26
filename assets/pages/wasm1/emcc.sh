#!/bin/bash
emcc -o hello.html \
	-s WASM=1 \
   	-s ENVIRONMENT=web \
    -s NO_EXIT_RUNTIME=1  \
    -s ALLOW_MEMORY_GROWTH=1 \
	-s "EXPORTED_FUNCTIONS=['_main','_e', '_d', '_width', '_height', '_render']" \
	-O3 \
	-s "EXTRA_EXPORTED_RUNTIME_METHODS=['ccall','cwrap','getValue']" \
    --std=c++14 main.cpp -o hello.html -O3  -DQUALITY=9 -DBOUNCES=70 -DFACTOR=2


