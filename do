#!/bin/sh

## Detect compiler ##
. ./lib/sh/compiler.inc

## Detect architecture ##
. ./lib/sh/arch.inc

## Target options ##
rm -f .ecflags
rm -f .ecflags_st
rm -f .elflags
rm -f .elflags_st

if [ `uname` = "Minix" ]; then
	printf -- "-I/usr/pkg/include/ " > .ecflags
	printf -- "-I/usr/pkg/include/ -DLIBFSMA_NO_THREADS " > .ecflags_st
	printf -- "/usr/pkg/lib/libpthread.so " > .elflags
	touch .elflags_st
else
	printf -- "-pthread " > .ecflags
	printf -- "-DLIBFSMA_NO_THREADS " > .ecflags_st
	printf -- "-pthread " > .elflags
	touch .elflags_st
fi

## Build ##
make

if [ $? -ne 0 ]; then
	echo "Build failed."
	exit 1
fi

touch .done

echo "Build completed."

exit 0

