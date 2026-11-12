#!/usr/bin/env bash

gcc -g -O0 main.c -o a.out || exit 1

run_big=false
if [[ "${1:-}" == "-big" ]]; then
    run_big=true
fi

for input in ./testcase/*; do
    [[ -f "$input" ]] || continue

    if [[ "$input" == *large_random_article* && "$run_big" != true ]]; then
        continue
    fi

    echo "=== $input ==="
    ./a.out "$input" -s -t 2 -f pro -w cat
    echo
done
