CC=`cat ../compiler`
CCFLAGS=-I../include/ -fstrict-aliasing -Wall -Werror
LDFLAGS=../src/libfsma.so -s

all:
	${CC} ${CCFLAGS} -c eg_alloc.c
	${CC} -o eg_alloc eg_alloc.o ${LDFLAGS}

clean:
	rm -f *.o
	rm -f eg_alloc

