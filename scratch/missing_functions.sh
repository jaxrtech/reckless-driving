#!/usr/bin/env bash
cmake . >/dev/null && make >/dev/null 2>| sed -n -r -e "s/.*?undefined reference to \`([^']+)'/\1/pg" | sort | uniq
