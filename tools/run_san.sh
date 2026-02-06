#!/usr/bin/env bash
set -euo pipefail

mode="${1:-asan}"

case "$mode" in
  asan)
    build_dir="build-asan"
    cxx_flags="-fsanitize=address -fno-omit-frame-pointer -O1 -g"
    ;;
  tsan)
    build_dir="build-tsan"
    cxx_flags="-fsanitize=thread -fno-omit-frame-pointer -O1 -g"
    ;;
  *)
    echo "Usage: $0 [asan|tsan]"
    exit 1
    ;;
esac

cmake -S . -B "$build_dir" -DCMAKE_CXX_FLAGS="$cxx_flags"
cmake --build "$build_dir" -j

"$build_dir/test_basic"
"$build_dir/test_future"
"$build_dir/test_shutdown"
