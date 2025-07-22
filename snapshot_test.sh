#!/bin/bash

set -eu

mkdir -p tests/snapshots/normal tests/snapshots/battle_only

grep '#define BATTLE_ONLY false' ./tests/utils/snapshot_generator.cpp || (echo "BATTLE_ONLY should be set to false in snapshot_generator.cpp" && exit 1)

just build-tests

for file in ./tests/scenarios/*.json; do
	echo "$file"
	./tests/build/snapshot_generator "$file" "./tests/snapshots/normal/$(basename $file .json).snap"
done

# sed -i -e 's/\#define BATTLE_ONLY false/\#define BATTLE_ONLY true/g' ./tests/utils/snapshot_generator.cpp

# trap "sed -i -e 's/\#define BATTLE_ONLY true/\#define BATTLE_ONLY false/g' ./tests/utils/snapshot_generator.cpp" EXIT

just build-tests battle="true"

for file in ./tests/scenarios/*.json; do
	echo "$file"
	./tests/build/snapshot_generator "$file" "./tests/snapshots/battle_only/$(basename $file .json).snap"
done

for file in ./tests/scenarios/*.json; do
	echo diff --text ./tests/snapshots/{normal,battle_only}/$(basename $file .json).snap
	diff --text ./tests/snapshots/{normal,battle_only}/$(basename $file .json).snap
done
