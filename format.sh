#!/bin/bash

### This script is used to run clang-format on the source files.

# Directory containing this bash script.
readonly DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

format() {
  find "$1" -type f -name '*.cpp' -o -name '*.hpp' | xargs clang-format -i
}

format "$DIR/library"
format "$DIR/server"
exit 0

