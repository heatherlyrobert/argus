/*============================[[ beg-of-code ]]===============================*/

/*===[[ START HDOC ]]=========================================================*/
/*===[[ HEADER ]]=============================================================#

 *   focus         : (SA) system_admin
 *   niche         : (bo) bootup/shutdown
 *   application   : init
 *   purpose       : provides consistent, reliable system initialization
 *
 *   base_system   : gnu/linux
 *   lang_name     : gnu/ansi-c (because it's right, just, best, and universal)
 *   dependencies  : yLOG only
 *   size          : small (less than 2,000 slocL)
 * 
 *   author        : the_heatherlys
 *   created       : 2010-10
 *   priorities    : direct, simple, brief, vigorous, and lucid (h.w. fowler)
 *   end goal      : loosely coupled, strict interface, maintainable, portable
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
 *   our initd will attempt to implement the original idea of simplicity,
 *   clarity, and power with more incremental control, better recovery/restart,
 *   automated testing, stronger security, better logging, and much more
 *   consolidated configuration.  we will focus on an automation-intensive,
 *   power-user environment for our own use and not drive backward compatibility
 *
 *   "do one thing and do it well (securely) and focus on technical power users"
 *
 *   initd will provide...
 *      - running basic configuration tasks, like setting hostname
 *      - running on/off actions, like mounting and unmounting filesystems
 *      - respawning serial programs, like getty
 *      - managing daemon start, stop, and resurrection
 *      - clean and easy to read configuration file
 *      - driving startup and shutdown scheduling/tracking into a gantt
 *      - strict, explicit, static dependencies to aid launching
 *      - strict glibc/ansi-c so it can be ported to and compilied on anything
 *      - fast and efficient because we want to enable tons of automation
 *      - solid logging and status updates so you never have to guess
 *      - clean, clean code so we can maintain it after long absences
 *      - fullsome unit testing and regression testing suite
 *      - eliminate known and potential security gaps and hacking vectors
 *      - very specific to our system config to avoid security holes
 *      - force our system daemons to only be controlled through init
 *
 *   crond will not provide...
 *      - any ability for a job/install to "automagically" add itself to boot
 *      - any gui-ish screen to make it easier to maintain boot sequence
 *      - any "automagic" dependency calculators, do it yourself for god sake
 *      - output specifically to an operator console (logger can route that)
 *      - any backward compatibility with inittab (no longer useful)
 *      - automatic recognition/reaction to hardware or config changes
 *      - alternate shells (we're gonna run pure posix dash)
 *      - extended shell variables (gonna have a spartan shell environment)
 *      - run-time configuration (no, its only for us, we can update code)
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



/*===[[ HEADER GUARD ]]=======================================================*/
#ifndef YINIT
#define YINIT loaded



/* rapidly evolving version number to aid with visual change confirmation     */
#define  VER_NUM   "0.3c"
#define  VER_TXT   "eos up and going, collecting all zombies as well"



/*---(headers)--------------------------------------------------*/

/*---(custom)-------------------*/
#include <yLOG.h>              /* heatherly logger                            */

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


#define    LOCKFILE      "/var/run/initd.pid"
#define    CONF          "/home/system/p_gvskav/initd.heatherly_init_daemon/initd.conf"
#define    PERFORMANCE   "/var/log/yLOG.current/initspeed.execution_tracking"
#define    STUFF         "/var/log/yLOG.current/initextra.execution_feedback"

#define    PATH          "/sbin:/bin:/usr/sbin:/usr/bin:/opt/sbin:/opt/bin:/usr/local/sbin:/usr/local/bin"
#define    SHELL         "/bin/dash"

/*---(convienence typedefs)-------------------------------------*/
#define    NAME            11     /* max name field   */
#define    DESC            51     /* max desc field   */
#define    LINE           500     /* max crontab line */
#define    CMD            500     /* max command len  */


extern    char      debug__flow;
#define   DEBUG_FLOW     if (debug__flow == 'y') 


/*---(convienence typedefs)-------------------------------------*/
typedef long long        llong;
typedef const int        cint;
typedef const long       clong;
typedef const char       cchar;

typedef struct stat      tSTAT;
typedef struct passwd    tPASSWD;
typedef struct tm        tTIME;
typedef struct dirent    tDIRENT;


/*---(structures)-----------------------------------------------*/

struct cACCESSOR
{
   /*---(files)----------------*/
   char      quiet;              /* bool : 0=normal, 1=quiet                  */
   char      updates;            /* bool : 0=normal, 1=quiet                  */
   int       logger;             /* log file so that we don't close it        */
   int       locker;             /* lock file in /var/run                     */
} my;


typedef struct cPROC tPROC;
struct cPROC {
   /*---(input)----------------*/
   int       recd;                /* inittab record number                    */
   char      type;                /* process type                             */
   char      name[NAME];          /* short name for reference                 */
   char      desc[DESC];          /* longer description                       */
   char      user[NAME];          /* user name                                */
   int       uid;                 /* user id to use to launch job             */
   char      run[CMD];            /* command to run                           */
   /*---(processing)-----------*/
   int       rpid;                /* process id of crond                      */
   llong     start;               /* start time                               */
   llong     end;                 /* end time                                 */
   char      status;              /* -=new, r=running, c=complete             */
   /*---(milestones)------------------*/
   tPROC    *next;           /* next line in proc's doubly linked list        */
   tPROC    *prev;           /* next line in proc's doubly linked list        */
   tPROC    *pnext;          /* next line in the milestone linked list        */
   tPROC    *pprev;          /* prev line in the milestone linked list        */
} procs[200];
int        nprocs;


/*---(prototypes)-----------------------------------------------*/
char      initialize         (cchar);
char      terminate          (cchar*, cint);
void      communicate        (int a_sig, siginfo_t *a_info, void *a_nada);
char      signals            (void);
char      daemonize          (void);
char      lock               (void);
char      pulse              (void);
char      watch              (void);
char      prepare            (void);
     
char      inventory          (FILE*);
char      read_process       (char*, int);
char      run                (int);

#endif
/*============================[[ end-of-code ]]===============================*/
