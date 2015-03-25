CC=`cat ../.compiler`
CCFLAGS=-I../include/ -fstrict-aliasing -Wall -Werror
ARCHFLAGS=`cat ../.archflags`
LDFLAGS=../src/libfsma.`cat ../.extlib` -s

all:
	${CC} ${CCFLAGS} ${ARCHFLAGS} -c eg_alloc.c
	${CC} -o eg_alloc eg_alloc.o ${LDFLAGS}

clean:
	rm -f *.o
	rm -f eg_alloc

