#!/usr/bin/env sh

script_dir=$(cd "$(dirname "${0}")" && pwd) || exit 1
project_dir=$(dirname "${script_dir}") || exit 1
build_dir=${project_dir}/build

if [ -z "${CLANG_TIDY_EXE}" ]; then
    CLANG_TIDY_EXE=clang-tidy
fi

processed_count=0
while read f; do
    "${CLANG_TIDY_EXE}" -p "${build_dir}" "--warnings-as-errors=*" "${f}" || exit 1
    processed_count=$((processed_count+1))
done <<EOF
$(find "${project_dir}" -iname "*.cpp" -not -iwholename "${build_dir}/*")
EOF

if [ "${processed_count}" -lt 1 ]; then
    echo "Error: No files processed." >&2
    exit 1
fi
