#!/usr/bin/env sh
script_dir=$(cd "$(dirname "${0}")" && pwd) || exit 1
while read f; do
    clang-format -i "${f}" || exit 1
done <<EOF
$(find . -regextype posix-egrep -iregex '.*\.h(h|pp)?')
EOF
"${script_dir}/check_format.sh" || exit 1
