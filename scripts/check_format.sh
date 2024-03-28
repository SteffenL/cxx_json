#!/usr/bin/env sh
processed_count=0
while read f; do
    clang-format --dry-run --Werror "${f}" || exit 1
    processed_count=$((processed_count+1))
done <<EOF
$(find . -regextype posix-egrep -iregex '.*\.h(h|pp)?')
EOF
if [ "${processed_count}" -lt 1 ]; then
    echo "Error: No files processsed." >&2
    exit 1
fi
