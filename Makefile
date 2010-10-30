#*============================---(source-start)---============================*#


#*---(current variables)--------------*#
BASE    = init
DAEMON  = initd
DIR     = /home/system/p_gvskav/initd.heatherly_init_daemon


#*---(standard variables)-------------*#
COMP    = gcc -c -std=gnu89 -g -pg -Wall -Wextra
INC     = 
LINK    = gcc 
LIBS    = -lyLOG
LIBS_S  = 
COPY    = cp -f  
CLEAN   = rm -f
ECHO    = @echo


FILES   = init.h initd.c eos.c kharon.c nyx.c initd_main.c Makefile initd.conf _vi.conf

#*---(MAIN)---------------------------*#
all                : ${DAEMON}  eos


#*---(executables)--------------------*#
${DAEMON}          : ${DAEMON}_main.o ${DAEMON}.o
	${LINK}  -o ${DAEMON}       ${DAEMON}_main.o ${DAEMON}.o ${LIBS}

eos                : eos.o
	${LINK}  -o eos             eos.o ${LIBS}



#*---(objects)------------------------*#
eos.o              : ${BASE}.h eos.c
	${COMP}  eos.c ${INC}

${DAEMON}.o        : ${BASE}.h ${DAEMON}.c
	${COMP}  ${DAEMON}.c ${INC}

${DAEMON}_main.o   : ${BASE}.h ${DAEMON}_main.c
	${COMP}  ${DAEMON}_main.c


#*---(housecleaning)------------------*#
clean              :
	${ECHO}  cleaning out object, backup, and temp files
	${CLEAN} *.o
	${CLEAN} *~
	${CLEAN} temp*

bigclean           :
	${CLEAN} .*.swp

install            : ${DAEMON}
	${ECHO}  installing in b_nvdo
	${COPY}  ${DAEMON}  /sbin/
	chmod    0700       /sbin/${DAEMON}
	chown    root:root  /sbin/${DAEMON}
	sha1sum  ${DAEMON}
	${COPY}  eos        /sbin/
	chmod    0700       /sbin/eos
	chown    root:root  /sbin/eos
	sha1sum  eos

backup             : 
	export TEST="grep VER_TXT init.h"
	${ECHO}  backing up
	${ECHO} ${TEST}
	#tar -czf init.tgz ${FILES}


#*============================----(source-end)----============================*#
