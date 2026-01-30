/*============================[[    beg-code    ]]============================*/
/*===[[ ONE_LINERS ]]=========================================================*/
/*-------   --12345678  "123456789-123456789-123456789-123456789-123456789-123456789-"  */

#define     P_FOCUS     "SA (system administration)"
#define     P_NICHE     "se (security)"
#define     P_SUBJECT   "system access guardian"
#define     P_PURPOSE   "system access guardian"

#define     P_NAMESAKE  "argus-panoptes (all-seeing)"
#define     P_HERITAGE  ""
#define     P_IMAGERY   ""
#define     P_REASON    ""

#define     P_ONELINE   P_NAMESAKE " " P_SUBJECT

#define     P_BASENAME  "argus"
#define     P_FULLPATH  "/usr/sbin/argus"
#define     P_SUFFIX    ""
#define     P_CONTENT   ""

#define     P_SYSTEM    "gnu/linux   (powerful, ubiquitous, technical, and hackable)"
#define     P_LANGUAGE  "ansi-c      (wicked, limitless, universal, and everlasting)"
#define     P_CODESIZE  "small       (appoximately 1,000 slocl)"
#define     P_DEPENDS   ""

#define     P_AUTHOR    "heatherlyrobert"
#define     P_CREATED   "2012-07"

#define     P_VERMAJOR  "1.--"
#define     P_VERMINOR  "1.1-, on-going improvements"
#define     P_VERNUM    "1.1c"
#define     P_VERTXT    "migrated exporting/exim into yPARSE and unit tested"

#define     P_PRIORITY  "direct, simple, brief, vigorous, and lucid (h.w. fowler)"
#define     P_PRINCIPAL "[grow a set] and build your wings on the way down (r. bradbury)"
#define     P_REMINDER  "there are many better options, but i *own* every byte of this one"

/*-------   --12345678  "123456789-123456789-123456789-123456789-123456789-123456789-"  */
/*===[[ END ONE_LINERS ]]=====================================================*/


/*===[[ HEADER ]]=============================================================#

 *   focus         : SA.in, system administration, user information
 *   niche         : unsure
 *   heritage      : argus-panoptes (giant with one-hundred eyes, all seeing)
 *   purpose       : system access guardian
 *
 *   base_system   : gnu/linux   (powerful, ubiquitous, technical, and hackable)
 *   lang_name     : ansi-c      (wicked, limitless, universal, and everlasting)
 *   dependencies  : none
 *   size          : small       (approximately 1,000 slocL)
 * 
 *   author        : the_heatherlys
 *   created       : 2012-07
 *   priorities    : direct, simple, brief, vigorous, and lucid (h.w. fowler)
 *   end goal      : loosely coupled, strict interface, maintainable, portable
 * 
 *   simplicity is prerequisite for reliability and security, but logging,
 *   debugging code, and unit testing must never be neglected
 *
 */
/*===[[ SUMMARY ]]=============================================================*

 * 
 *   argus panoptes (all-seeing) was a giant with one-hundred eyes and great
 *   strength who was used by Hera as a watcher.  when he was killed by Hermes,
 *   Hera places his eyes on the tail of the peacock (her favorite bird).
 *
 *   argus is intended to provide access to system records related to users.
 *
 */
/*===[[ CONFIGURATION ]]=======================================================*

 * 
 *
 *   /var/log/sulog, shows all calls to "su" command.  this file is only used
 *   if defined in the "/etc/login.defs" file with the entry...
 *          SULOG_FILE  /var/log/sulog
 *   best not to change the name or location to keep the confusion down.  also,
 *   you must create the file with "touch" in order to start loging.
 *
 *
 *   /var/log/btmp, shows all failed login attempts.  this file is only used
 *   if defined in the "/etc/login.defs" file with the entry...
 *          FTMP_FILE   /var/log/btmp
 *   best not to change the name or location to keep the confusion down.  also,
 *   you must create the file with "touch" in order to start loging.
 *
 *
 *   /var/log/lastlog, shows last login information.  this file is only used
 *   if defined in the "/etc/login.defs" file with the entry...
 *          LAST_LOG_ENB  yes
 *   best not to change the name or location to keep the confusion down.  also,
 *   you must create the file with "touch" in order to start loging.
 *
 *
 */
/*============================================================================*/

/*> #include    "init.h"                                                              <*/
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <utmp.h>
#include    <time.h>
#include    <ctype.h>
#include    <unistd.h>
#include    <netinet/in.h>
#include    <arpa/inet.h>
#include    <sys/stat.h>          /* fstat, umask                             */
#include    <grp.h>               /* group structure                          */

#include    <yLOG.h>
#include    <yURG.h>
#include    <yENV.h>
#include    <ySTR.h>
#include    <yPARSE.h>
#include    <yDLST_solo.h>
#include    <yCOLOR_solo.h>

#define     ARGUS_TEMP       "/tmp/argus.txt"

#define     PASSWD           "/etc/passwd"
#define     SHADOW           "/etc/shadow"
#define     GROUP            "/etc/group"
#define     GSHADOW          "/etc/gshadow"
#define     TTY              "/etc/securetty"
#define     HESTIA           "/var/log/hestia.logging"


#define     UTMP             "/var/run/utmp"
#define     WTMP             "/var/log/wtmp"
#define     BTMP             "/var/log/btmp"
#define     SULOG            "/var/log/sulog"
#define     LASTLOG          "/var/log/lastlog"

typedef struct stat      tSTAT;
typedef struct passwd    tPASSWD;
typedef struct group     tGRP;
typedef struct tm        tTIME;

typedef     struct      cMY         tMY;
struct cMY {
   /*---(internal)------------*/
   char        m_cmdline   [LEN_FULL];
   /*---(reporting)-----------*/
   char        m_runas;
   char        m_exim;
   char        m_titles;
   char        m_file      [LEN_PATH];
   FILE       *m_output;
   /*---(done)----------------*/
};
extern      tMY         my;

#define      RUN_FILES      'F'
#define      RUN_USERS      'U'
#define      RUN_GROUPS     'G'
#define      RUN_MATRIX     'M'
#define      RUN_UTMP       'u'
#define      RUN_WTMP       'w'
#define      RUN_BTMP       'b'
#define      RUN_CURRENT    'c'
#define      RUN_LASTLOG    'l'

#define      RPTG_FILES     if (my.m_runas  == RUN_FILES)
#define      RPTG_USERS     if (my.m_runas  == RUN_USERS)
#define      RPTG_GROUPS    if (my.m_runas  == RUN_GROUPS)
#define      RPTG_MATRIX    if (my.m_runas  == RUN_MATRIX)
#define      RPTG_UTMP      if (my.m_runas  == RUN_UTMP)
#define      RPTG_WTMP      if (my.m_runas  == RUN_WTMP)
#define      RPTG_BTMP      if (my.m_runas  == RUN_BTMP)
#define      RPTG_CURRENT   if (my.m_runas  == RUN_CURRENT)
#define      RPTG_LASTLOG   if (my.m_runas  == RUN_LASTLOG)

#define      SHOW_FILES     (my.m_runas == RUN_FILES  ) ? 'y' : '·'
#define      SHOW_USERS     (my.m_runas == RUN_USERS  ) ? 'y' : '·'
#define      SHOW_GROUPS    (my.m_runas == RUN_GROUPS ) ? 'y' : '·'
#define      SHOW_MATRIX    (my.m_runas == RUN_MATRIX ) ? 'y' : '·'
#define      SHOW_UTMP      (my.m_runas == RUN_UTMP   ) ? 'y' : '·'
#define      SHOW_WTMP      (my.m_runas == RUN_WTMP   ) ? 'y' : '·'
#define      SHOW_BTMP      (my.m_runas == RUN_BTMP   ) ? 'y' : '·'
#define      SHOW_CURRENT   (my.m_runas == RUN_CURRENT) ? 'y' : '·'
#define      SHOW_LASTLOG   (my.m_runas == RUN_LASTLOG) ? 'y' : '·'

extern char   g_print      [LEN_RECD];
extern char   unit_answer  [LEN_RECD];


/*===[[ argus_prog.c ]]=======================================================*/
/*········´ ´·············support·´ ´·········································*/
char*       PROG_version            (void);
char        PROG_vershow            (void);
char        PROG_usage              (void);
/*········´ ´···········debugging·´ ´·········································*/
char        PROG_debugging          (int a_argc, char *a_argv[]);
/*········´ ´·············startup·´ ´·········································*/
char        PROG__init              (int a_argc, char *a_argv[]);
char        PROG__args              (int a_argc, char *a_argv[]);
char        PROG__begin             (void);
char        PROG_startup            (int a_argc, char *a_argv[]);
/*········´ ´··············driver·´ ´·········································*/
char        PROG_pseudo             (int a_argc, char *a_argv[]);
/*········´ ´············shutdown·´ ´·········································*/
char        PROG__end               (void);
char        PROG_shutdown           (void);
/*········´ ´················DONE·´ ´·········································*/




/*============================[[ end-of-code ]]===============================*/
