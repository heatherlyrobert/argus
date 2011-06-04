#*============================---(source-start)---============================*#


#*---(current variables)--------------*#
BASE    = eos


#*---(standard variables)-------------*#
COMP    = gcc -c -std=gnu89 -g -g -Wall -Wextra
#COMP    = tcc -c -g -Wall
INC     = 
#LINK    = tcc 
LINK    = gcc 
LIBS    = -lyLOG -lyDLST
LIBS_S  = 
COPY    = cp -f  
CLEAN   = rm -f
ECHO    = @echo


#tcc                : init.h eos.c kharon.c nyx.c
#	tcc -o eos        eos.c       ${LIBS}
#	tcc -o kharon     kharon.c    ${LIBS}
#	tcc -o nyx        nyx.c       ${LIBS}


#*---(MAIN)---------------------------*#
all                : eos kharon nyx


#*---(executables)--------------------*#
eos                : eos.o
	${LINK}  -o eos             eos.o     ${LIBS}

kharon             : kharon.o
	${LINK}  -o kharon          kharon.o  ${LIBS}

nyx                : nyx.o
	${LINK}  -o nyx             nyx.o     ${LIBS}



#*---(objects)------------------------*#
eos.o              : init.h eos.c
	${COMP}    eos.c      ${INC}

kharon.o           : init.h kharon.c
	${COMP}    kharon.c   ${INC}

nyx.o              : init.h nyx.c
	${COMP}    nyx.c      ${INC}


#*---(housecleaning)------------------*#
clean              :
	${ECHO}  cleaning out object, backup, and temp files
	${CLEAN} *.o
	${CLEAN} *~
	${CLEAN} temp*
	${CLEAN} eos kharon nyx

bigclean           :
	${CLEAN} .*.swp

install            : 
	${ECHO}  installing...
	${COPY}  eos        /sbin/
	chmod    0700       /sbin/eos
	chown    root:root  /sbin/eos
	sha1sum  eos
	${COPY}  kharon     /sbin/
	chmod    0700       /sbin/kharon
	chown    root:root  /sbin/kharon
	sha1sum  kharon
	${COPY}  nyx        /sbin/
	chmod    0700       /sbin/nyx
	chown    root:root  /sbin/nyx
	sha1sum  nyx
	call_graph eos.c

backup             : 
	export TEST="grep VER_TXT init.h"
	${ECHO}  backing up
	${ECHO} ${TEST}
	#tar -czf init.tgz ${FILES}


#*============================----(source-end)----============================*#
