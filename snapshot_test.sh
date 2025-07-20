#!/bin/bash

mkdir -p tests/snapshots/normal tests/snapshots/battle_only

just build-tests

for file in ./tests/scenarios/*.json; do
	echo "$file"
	./tests/build/snapshot_generator "$file" "./tests/snapshots/normal/$(basename $file .json).snap"
done

just build-tests true

for file in ./tests/scenarios/*.json; do
	echo "$file"
	./tests/build/snapshot_generator "$file" "./tests/snapshots/battle_only/$(basename $file .json).snap"
done

for file in ./tests/scenarios/*.json; do
	echo diff --text ./tests/snapshots/{normal,battle_only}/$(basename $file .json).snap
	diff_out=$(diff --text ./tests/snapshots/{normal,battle_only}/$(basename $file .json).snap)
	test -z "$diff_out" || echo "$file".snap differs: "$diff_out"
done
