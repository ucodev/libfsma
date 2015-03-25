CC=`cat ../.compiler`
INCLUDEDIR=../include
CCFLAGS=-I${INCLUDEDIR}
CCFLAGS_LIBCOR=-DLIBC_OVERRIDE=1 -I${INCLUDEDIR}
ECFLAGS=`cat ../.ecflags`
ARCHFLAGS=`cat ../.archflags`
LDFLAGS=-s -shared
ELFLAGS=`cat ../.elflags`
TARGET=libfsma.`cat ../.extlib`
TARGET_LIBCOR=libfsma_libcor.`cat ../.extlib`

all:
	${CC} ${CCFLAGS} ${ECFLAGS} ${ARCHFLAGS} -c mm.c
	${CC} ${LDFLAGS} ${ELFLAGS} -o ${TARGET} mm.o
	${CC} ${CCFLAGS_LIBCOR} ${ECFLAGS} ${ARCHFLAGS} -c mm.c
	${CC} ${LDFLAGS} ${ELFLAGS} -o ${TARGET_LIBCOR} mm.o

clean:
	rm -f *.o
	rm -f ${TARGET} ${TARGET_LIBCOR}

