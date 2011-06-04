/*============================[[ beg-of-code ]]===============================*/

/*===[[ HEADER ]]=============================================================#

 *   focus         : (SA) system_admin
 *   niche         : (st) startup
 *   application   : eos        (goddess of daybreak, rosy-fingered dawn)
 *   purpose       : consistent, reliable, and fast system initialization
 *
 *   base_system   : gnu/linux  (because it's powerful, universal, and hackable)
 *   lang_name     : gnu/ansi-c (because it's right, just, best, and universal)
 *   dependencies  : yLOG only
 *   size          : small (less than 1,000 slocL)
 * 
 *   author        : the_heatherlys
 *   created       : 2010-10
 *   priorities    : direct, simple, brief, vigorous, and lucid (h.w. fowler)
 *   end goal      : loosely coupled, strict interface, maintainable, portable
 * 
 */
/*===[[ HEADER ]]=============================================================#

 *   focus         : (SA) system_admin
 *   niche         : (in) init
 * 
 */
/*===[[ PURPOSE ]]=============================================================*

 *   init is a well-worn, familiar, time-tested, and posix-defined daemon that
 *   provides system initialization and shutdown services as well as acting as
 *   the parent of all processes on the system.  systems needs have changed a
 *   great deal since, but no common theory or clear replacements have emerged.
 *
 *   the original init was released in 1979 and was essentially directed by a
 *   single configuration file called inittab.  since then, init has diverged in
 *   at least four directions which are incompatible.  each of these directions
 *   requires a great deal of configuration, support files, and scripting which
 *   have makes them a brutally large, complex, unauditable monsters of their
 *   own.  i attempted to reengineer from gentoo's openrc, which is a good one,
 *   and it was #!@%$ painful to read and understand.
 *
 *   there are a couple of desires that have driven these diveringing systems...
 *      - full installation automation for packages that effect boot  (NO, NO)
 *      - ability to handle a huge variety of systems out-of-the-box  (NO, NO)
 *      - avoid technical knowledge to sequence boot (depends, needs) (NO, NO)
 *      - faster boot times by delaying until needed -- hotplugging   (NO, NO)
 *      - faster boot times by running in parallel                    (YES)
 *   the result of all this has been a monster of interpreted shell scripts that
 *   and interpreted code that is wicked complex and not to be touched by
 *   anyone but the maintainer.  yuck.
 *
 *   the worst of the kitchen-sink thinking is to combine init, cron, and
 *   several other critical daemons into a single hulking, integrated, hot-
 *   plugging nightmare so people can atrophy into limbless gui dwellers.  give
 *   the people bread and circuses...
 *
 *   traditional init packages have many distinct missions and come bundled with
 *   quite a few tools to accomplish those.  we would like to unbundle these
 *   focuses into separate tools in order to increase clarity and cleanliness.
 *      1) startup/initialization
 *      2) shutdown
 *      3) sleep/hybernation
 *      4) root of process tree
 *      5) process reincarnation
 *      6) killing rouge processes
 *
 *   we will be implementing these as separate modules, each with their own
 *   very specific mission and testable in their own right.
 *
 *   general requirement across each of these will be...
 *      - strict glibc/ansi-c so it can be ported to and compilied on anything
 *      - fast and efficient because we want to enable tons of automation
 *      - solid logging and status updates so you never have to guess
 *      - clean, clean code so we can maintain it after long absences
 *      - tight, easy to read, and minimal configuration files
 *      - fullsome unit testing and regression testing suite
 *      - eliminate known and potential security gaps and hacking vectors
 *      - very specific to our system config to avoid security holes
 *
 *   "do one thing and do it well (securely) and focus on technical power users"
 *
 *   we don't want to just use the system, we want to *own* it; so that means
 *   we have to fundmentally understand the critical services which only tearing
 *   down and rebuilding can really teach -- even if its more basic in the end.
 *
 *   as luck would have it, minit (minimal init) is a wonderfully straight
 *   forward and clean implementation that we can build on.  it is licenced
 *   under the gnu gpl and so is perfect as our base.  so, we fork dcron...
 *
 *   so, as always, there are many, stable, accepted, existing programs and
 *   utilities written by better programmers which are likely superior in
 *   speed, size, capability, and reliability; BUT, i would not have learned
 *   nearly as much just using them, so i follow the adage..
 *
 *   TO TRULY LEARN> do not seek to follow in the footsteps of the men of old;
 *   seek what they sought ~ Matsuo Basho
 *
 *   at the end of the day, this is wicked critical and deserves the attention
 *
 */
/*============================================================================*/



/*===[[ HEADER GUARD ]]=======================================================*/
#ifndef YINIT
#define YINIT loaded



/*===[[ VERSIONING ]]=========================================================*/
/* rapidly evolving version number to aid with visual change confirmation     */



/*===[[ PRIVATE HEADERS ]]====================================================*/
#include <yLOG.h>              /* heatherly logger                            */
#include <yDLST.h>             /* heatherly specialty 4:2 list w/deps         */



/*===[[ PUBLIC HEADERS ]]=====================================================*/
/*---(big standards)------------*/
#include <stdio.h>             /* printf, scanf                               */
#include <stdlib.h>            /* exit, system, ...                           */
#include <sys/unistd.h>        /* fork, execl, getpid, getuid                 */
#include <string.h>            /* strcat, strcpy, strlen, ...                 */

/*---(process)------------------*/
#include <sys/wait.h>          /* sigaction, waitpid, wait4                   */

/*---(timekeeping)--------------*/
#include <time.h>              /* time, localtime, strftime, ...              */
#include <sys/time.h>          /* gettimeofday                                */

/*---(filesystems)--------------*/
#include <sys/mount.h>         /* mount                                       */
#include <fcntl.h>             /* open                                        */
#include <sys/stat.h>          /* fstat, umask                                */

/*---(users)--------------------*/
#include <pwd.h>               /* getpwuid, getpwnam                          */
#include <grp.h>               /* initgroups                                  */

/*---(structures)-----------------------------------------------*/

struct cACCESSOR
{
   /*---(files)----------------*/
   char      quiet;              /* bool : 0=normal, 1=quiet                  */
   char      updates;            /* bool : 0=normal, 1=quiet                  */
   int       logger;             /* log file so that we don't close it        */
   int       locker;             /* lock file in /var/run                     */
} my;



/*---(convienence typedefs)-------------------------------------*/
typedef long   long      llong;
typedef const  int       cint;
typedef const  long      clong;
typedef const  char      cchar;

typedef struct FILE      tFILE;
typedef struct stat      tSTAT;
typedef struct passwd    tPASSWD;
typedef struct tm        tTIME;
typedef struct dirent    tDIRENT;




#endif
/*============================[[ end-of-code ]]===============================*/
