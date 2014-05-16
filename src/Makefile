CC=`cat ../compiler`
INCLUDEDIR=../include
CCFLAGS=-I${INCLUDEDIR} -fPIC -O3 -fstrict-aliasing -Wall -Wextra -Werror -pthread
CCFLAGS_ST=-I${INCLUDEDIR} -fPIC -O3 -fstrict-aliasing -Wall -Wextra -Werror
CCFLAGS_LIBCOR=-DLIBC_OVERRIDE=1 -I${INCLUDEDIR} -fPIC -O3 -fstrict-aliasing -Wall -Werror -pthread
CCFLAGS_LIBCOR_ST=-DLIBC_OVERRIDE=1 -I${INCLUDEDIR} -fPIC -O3 -fstrict-aliasing -Wall -Werror
LDFLAGS=-s -shared -pthread
LDFLAGS_ST=-s -shared
TARGET=libfsma.so
TARGET_ST=libfsma_st.so
TARGET_LIBCOR=libfsma_libcor.so
TARGET_LIBCOR_ST=libfsma_libcor_st.so

all:
	${CC} ${CCFLAGS} -c mm.c
	${CC} ${LDFLAGS} -o ${TARGET} mm.o
	${CC} ${CCFLAGS_ST} -c mm.c
	${CC} ${LDFLAGS_ST} -o ${TARGET_ST} mm.o
	${CC} ${CCFLAGS_LIBCOR} -c mm.c
	${CC} ${LDFLAGS} -o ${TARGET_LIBCOR} mm.o
	${CC} ${CCFLAGS_LIBCOR_ST} -c mm.c
	${CC} ${LDFLAGS_ST} -o ${TARGET_LIBCOR_ST} mm.o

clean:
	rm -f *.o
	rm -f ${TARGET} ${TARGET_ST} ${TARGET_LIBCOR} ${TARGET_LIBCOR_ST}

