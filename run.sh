#!/bin/bash

# Default compiler flags
CFLAGS="-Wall -Werror -Wextra -pedantic -std=gnu89"

# Check if at least one .c file is given
if [ $# -eq 0 ]; then
    echo "Usage: $0 file.c [more_files.c ...]"
    exit 1
fi

# Always compile into "hsh"
gcc $CFLAGS "$@" -o hsh

# Check result
if [ $? -eq 0 ]; then
    echo "✅ Compilation successful! Run with: ./hsh"
else
    echo "❌ Compilation failed!"
fi
