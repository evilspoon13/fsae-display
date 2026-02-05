#!/bin/bash

cd "$(dirname "$0")/.."

for module in can-reader data-logger graphics-engine; do
    echo "Generating for $module..."
    cd "$module"
    make clean -s 2>/dev/null
    bear -- make -s || true
    cd ..
done

jq -s 'add' */compile_commands.json > compile_commands.json
rm -f */compile_commands.json
echo "Done: compile_commands.json updated"
