#!/usr/bin/env sh
script_dir=$(cd "$(dirname "${0}")" && pwd) || exit 1
project_dir=$(dirname "${script_dir}") || exit 1
build_dir=${project_dir}/build
run-clang-tidy -clang-tidy-binary clang-tidy -style file -p "${build_dir}" || exit 1
