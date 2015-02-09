#!/bin/sh

## Detect compiler ##
. ./lib/sh/compiler.inc

## Detect architecture ##
. ./lib/sh/arch.inc

## Build ##
make

if [ $? -ne 0 ]; then
	echo "Build failed."
	exit 1
fi

touch .done

echo "Build completed."

exit 0

