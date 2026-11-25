#!/bin/bash
# bash build.sh       -> builds web/studio.js and web/studio.wasm from the C++ code
# bash build.sh test  -> runs tests/test_prices.cpp (no browser needed)
set -e
cd "$(dirname "$0")"

if [ "$1" = "test" ]; then
  c++ -std=c++17 -I src $(ls src/*.cpp | grep -v bindings.cpp) tests/test_prices.cpp -o tests/test_prices
  ./tests/test_prices
  exit 0
fi

emcc -std=c++17 -O2 -fexceptions -I src src/*.cpp -lembind \
  -sMODULARIZE=1 -sEXPORT_NAME=createStudioModule -sALLOW_MEMORY_GROWTH=1 \
  -o web/studio.js
echo "Done: web/studio.js and web/studio.wasm"
