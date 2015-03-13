CC=`cat ../.compiler`
INCLUDEDIR=../include
CCFLAGS=-I${INCLUDEDIR} -fPIC -O3 -fstrict-aliasing -Wall -Wextra -Werror
CCFLAGS_ST=-I${INCLUDEDIR} -fPIC -O3 -fstrict-aliasing -Wall -Wextra -Werror
CCFLAGS_LIBCOR=-DLIBC_OVERRIDE=1 -I${INCLUDEDIR} -fPIC -O3 -fstrict-aliasing -Wall -Werror
CCFLAGS_LIBCOR_ST=-DLIBC_OVERRIDE=1 -I${INCLUDEDIR} -fPIC -O3 -fstrict-aliasing -Wall -Werror
ECFLAGS=`cat ../.ecflags`
ECFLAGS_ST=`cat ../.ecflags_st`
ARCHFLAGS=`cat ../.archflags`
LDFLAGS=-s -shared
ELFLAGS=`cat ../.elflags`
LDFLAGS_ST=-s -shared
ELFLAGS_ST=`cat ../.elflags_st`
TARGET=libfsma.so
TARGET_ST=libfsma_st.so
TARGET_LIBCOR=libfsma_libcor.so
TARGET_LIBCOR_ST=libfsma_libcor_st.so

all:
	${CC} ${CCFLAGS} ${ECFLAGS} ${ARCHFLAGS} -c mm.c
	${CC} ${LDFLAGS} ${ELFLAGS} -o ${TARGET} mm.o
	${CC} ${CCFLAGS_ST} ${ECFLAGS_ST} ${ARCHFLAGS} -c mm.c
	${CC} ${LDFLAGS_ST} ${ELFLAGS_ST} -o ${TARGET_ST} mm.o
	${CC} ${CCFLAGS_LIBCOR} ${ECFLAGS} ${ARCHFLAGS} -c mm.c
	${CC} ${LDFLAGS} ${ELFLAGS} -o ${TARGET_LIBCOR} mm.o
	${CC} ${CCFLAGS_LIBCOR_ST} ${ECFLAGS_ST} ${ARCHFLAGS} -c mm.c
	${CC} ${LDFLAGS_ST} ${ELFLAGS_ST} -o ${TARGET_LIBCOR_ST} mm.o

clean:
	rm -f *.o
	rm -f ${TARGET} ${TARGET_ST} ${TARGET_LIBCOR} ${TARGET_LIBCOR_ST}

