SYSBINDIR=`cat .dirbin`
SYSCACHEDIR=`cat .dircache`
SYSCONFDIR=`cat .dirconf`
SYSDOCDIR=`cat .dirdoc`
SYSINCLUDEDIR=`cat .dirinclude`
SYSLIBDIR=`cat .dirlib`
SYSMANDIR=`cat .dirman`
SYSRUNDIR=`cat .dirrun`
SYSSBINDIR=`cat .dirsbin`
SYSSHAREDIR=`cat .dirshare`
SYSTMPDIR=`cat .dirtmp`

all:
	cd src && make && cd ..
	cd example && make && cd ..

install_all:
	mkdir -p ${SYSLIBDIR}/
	mkdir -p ${SYSINCLUDEDIR}/fsma
	cp src/libfsma* ${SYSLIBDIR}/
	cp include/*.h ${SYSINCLUDEDIR}/fsma

clean:
	cd src && make clean && cd ..
	cd example && make clean && cd ..

