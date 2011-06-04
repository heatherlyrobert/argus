/*============================[[ beg-of-code ]]===============================*/

/*===[[ HEADER ]]=============================================================#

 *   focus         : (SA) system_admin
 *   niche         : (st) startup
 *   application   : eos        (goddess of daybreak, rosy-fingered dawn)
 *   purpose       : clean, light, consistent, reliable system initialization
 *
 *   base_system   : gnu/linux  (because it's powerful, universal, and hackable)
 *   lang_name     : gnu/ansi-c (because it's right, just, best, and universal)
 *   dependencies  : yLOG, yDLST, yEXEC, dash
 *   size          : small      (approximately 1,000 slocL)
 * 
 *   author        : the_heatherlys
 *   created       : 2010-10
 *   priorities    : direct, simple, brief, vigorous, and lucid (h.w. fowler)
 *   end goal      : loosely coupled, strict interface, maintainable, portable
 * 
 */
/*===[[ SUMMARY ]]=============================================================*
 * 
 *   eos is a clean, reliable, and light system initialization program to bring
 *   a unix/linux system from grub to login and ready for action.  its goal is
 *   to be consistent, steady, tight, and compact.
 *
 *   current methods of system startup have severely promoted flexibility,
 *   on-the-fly adaptation, automated update, and cross-system support.  all
 *   worthy goals, but the result is an almost unintelligible raft of scripts
 *   that are nearly beyond an administrator and allow for easy updates during
 *   software installation.  not for me.
 *
 *   eos will allow for...
 *      - a single configuration file (/etc/eos.conf)
 *      - consistent and fullsome logging using yLOG
 *      - dependency configuration using yDLST requirements/dependencies
 *      - only multiuser command line mode (makes it harder for hackers)
 *
 *   eos will not allow for...
 *      - different run levels (there are just better ways)
 *      - multiple configuration files (makes it harder to verify configuration)
 *      - alternate configuration file (potential hacker vector)
 *      - init/telinit fifo at /dev/initctl (not a fullsome deal)
 *      - inittab (we are going a diffenent way)
 *
 */
/*============================================================================*/


#include      "init.h"

#define  EOS_VER_NUM   "0.6a"
#define  EOS_VER_TXT   "read into new data structure and yDLST successfully"


#define    LOCKFILE      "/var/run/initd.pid"
/*> #define    CONF          "/home/system/p_gvskav/initd.heatherly_init_daemon/initd.conf"   <*/
#define    CONF          "/etc/eos.conf"
#define    PERFORM       "/var/log/yLOG/eos_speed.execution_tracking"
#define    STUFF         "/var/log/yLOG/eos_execs.execution_feedback"

#define    PATH          "/sbin:/bin:/usr/sbin:/usr/bin:/opt/sbin:/opt/bin:/usr/local/sbin:/usr/local/bin"
#define    SHELL         "/bin/dash"


/*---(convienence typedefs)-------------------------------------*/
#define    NAME            50     /* max name field   */
#define    DESC            50     /* max desc field   */
#define    LINE           200     /* max crontab line */
#define    CMD            200     /* max command len  */



#define   ERR_NOT_ROOT          -1
#define   ERR_NOT_PID_ONE       -2
#define   ERR_NO_TMPFS          -3
#define   ERR_NO_LOGGER         -4


#define   MAX_RECD       1000
char      recd [MAX_RECD];
int       nrecd;



/*---(structures)-----------------------------------------------*/

typedef struct  cPROC tPROC;
struct cPROC {
   /*---(input)----------------*/
   int       recd;                /* inittab record number                    */
   char      name[NAME];          /* short name for reference                 */
   char      type;                /* process type                             */
   char      desc[DESC];          /* longer description                       */
   char      user[NAME];          /* user name                                */
   int       uid;                 /* user id to use to launch job             */
   char      run[CMD];            /* command to run                           */
   /*---(processing)-----------*/
   int       rpid;                /* process id of crond                      */
   llong     start;               /* start time                               */
   llong     end;                 /* end time                                 */
   char      status;              /* -=new, r=running, c=complete             */
   long      u_time;              /* user time in ms                          */
   long      s_time;              /* system time in ms                        */
   int       swaps;               /* number of process switches               */
   int       rc;                  /* return code                              */
};




typedef struct cPROC_O tPROC_O;
struct cPROC_O {
   /*---(input)----------------*/
   char      block[NAME];         /* block name for processes                 */
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
   long      u_time;              /* user time in ms                          */
   long      s_time;              /* system time in ms                        */
   int       swaps;               /* number of process switches               */
   int       rc;                  /* return code                              */
   /*---(milestones)------------------*/
   tPROC_O  *next;           /* next line in proc's doubly linked list        */
   tPROC_O  *prev;           /* next line in proc's doubly linked list        */
   tPROC_O  *pnext;          /* next line in the milestone linked list        */
   tPROC_O  *pprev;          /* prev line in the milestone linked list        */
} procs[200];
int        nprocs;


/*---(prototypes)------------------------*/
int        main              (int argc, char *argv[]);
char       prog_urgent       (int argc, char *argv[]);
char       prog_init         (void);
char       prog_args         (int argc, char *argv[]);
char       prog_begin        (void);
char       prog_end          (void);

char      str_parse          (char*);
char     *str_trim           (char*);

/*> char      eos_init           (void);                                              <*/
void      eos_comm           (int a_sig, siginfo_t *a_info, void *a_nada);
char      eos_signal         (void);
char      eos_daemon         (void);

char      eos_open           (void);
char      eos_read           (void);
char      eos_parse          (void);
char      eos_list           (void);
char      eos_close          (void);
char      eos_report         (void);

char      eos_list_OLD       (void);
char      eos_read_OLD       (char*, int);

char      eos_launch         (void);
char      eos_wait           (void);
char      eos_done           (void);
char      eos_find           (int);
char      eos_run            (int);







FILE     *f = NULL;
int       running     = 0;
char      complete    = 'n';
char     *args[20];



char      mode_daemon    = 'y';
char      mode_init      = 'y';
char      mode_logger    = 's';




/*====================------------------------------------====================*/
/*===----                          utilities                           ----===*/
/*====================------------------------------------====================*/
static void      o___UTILITIES_______________o (void) {;}

char               /* PURPOSE : break string into an argv[] structure --------*/
str_parse          (char *a_cstring)
{
   /*---(defenses)-----------------------*/
   if (a_cstring == NULL) return -1;
   /*---(locals)-------------------------*/
   int i, j, k;
   int len = strlen(a_cstring);
   int pos = 0;
   /*---(clear parsed array)-------------*/
   for (i = 0; i < 20; ++i) args[i] = NULL;
   /*---(assign executable)--------------*/
   args[0] = a_cstring;
   /*---(arguments)----------------------*/
   i    = 1;
   pos  = 0;
   /*> yLOG_value  ("len",      len);                                                 <*/
   while (pos < len) {
      /*---(get though argument)---------*/
      for (k = pos; k <= len; ++k) {
         if (  a_cstring[k] != '\0' &&
               a_cstring[k] != ' ' )     continue;
         pos = k;
         break;
      }
      if (pos + 1 >= len)              break;
      /*---(get though whitespace)-------*/
      for (k = pos; k < len; ++k) {
         if (a_cstring[k] == ' ') {
            a_cstring[k]  =  '\0';
            continue;
         }
         pos = k;
         break;
      }
      if (pos + 1 >= len)              break;
      /*---(assign next argument)--------*/
      /*> yLOG_value  ("pos",      pos);                                              <*/
      args[i] = a_cstring + pos;
      /*---(prepare for next loop)-------*/
      ++i;
      if (i >= 20) break;
   }
   /*---(display)--------------------------*/
   /*> yLOG_info   ("start",    a_cstring);                                           <* 
    *> for (i = 0; i < 20; ++i) {                                                     <* 
    *>    if (args[i] == NULL)             break;                                     <* 
    *>    yLOG_bullet (i,    args[i]);                                                <* 
    *> }                                                                              <*/
   /*---(complete)-------------------------*/
   return 0;
}

char*              /* PURPOSE : clean whitespace from both sides of a string -*/
str_trim           (char *a_cstring)
{
   /*---(defenses)-------------------------*/
   if (a_cstring == NULL) return NULL;
   /*---(locals)---------------------------*/
   int i, j;
   int len = 0;
   /*---(leading whitespace)---------------*/
   len = strlen(a_cstring);
   for (i = 0; i <= len; ++i) {
      if (a_cstring[0] != ' ') break;
      for (j = 0; j <= len; ++j)
         a_cstring[j] = a_cstring[j + 1];
      --len;
   }
   /*---(trailing whitespace)--------------*/
   len = strlen(a_cstring);
   for (i = len - 1; i >= 0; --i) {
      if (a_cstring[i] != ' ') break;
      a_cstring[i] = '\0';
   }
   /*---(complete)-------------------------*/
   return a_cstring;
}



/*====================------------------------------------====================*/
/*===----                           drivers                            ----===*/
/*====================------------------------------------====================*/
static void      o___PROGRAM_________________o (void) {;}

int
prog_usage         (void)
{
   printf ("\n");
   printf ("eos        : clean, light, consistent, reliable system initialization\n");
   printf ("\n");
   printf ("usage      : eos [URGENTS] [OPTIONS]\n");
   printf ("\n");
   printf ("OPTIONS...\n");
   printf ("   -d,--daemon  (*)\n");
   printf ("             execute in normal, daemon mode\n");
   printf ("   -f,--foreground\n");
   printf ("             execute in non-daemon or foreground mode\n");
   printf ("   -1,--init  (*)\n");
   printf ("             must execute as pid 1\n");
   printf ("   -a,--anypid\n");
   printf ("             allow execution as any pid, not just 1\n");
   printf ("   -h,--help\n");
   printf ("             display program usage/help\n");
   printf ("\n");
   printf ("URGENTS...\n");
   printf ("   @q        quiet execution with no logs\n");
   printf ("   @l,@ls    route logging to system logs  (*)\n");
   printf ("   @lp       route logging to personal logs\n");
   printf ("   @lo       route logging to stdout\n");
   printf ("\n");
   printf ("namesake   : goddess of daybreak, i.e., homer's rosy-fingered dawn\n");
   printf ("\n");
   exit (0);
}

char       /*----: handle pre-argument processing initialization -------------*/
prog_init          (void)
{
   mode_daemon    = 'y';
   mode_init      = 'y';
   mode_logger    = 's';
   return 0;
}

char       /*----: process the command line arguments (urgents) --------------*/
prog_urgent        (int argc, char *argv[])
{
   /*---(locals)-------------------------*/
   int       i         = 0;            /* loop iterator -- arguments          */
   char     *a         = NULL;         /* current argument                    */
   /*---(process)------------------------*/
   for (i = 1; i < argc; ++i) {
      a = argv[i];
      if (a[0] != '@')       continue;
      if      (strncmp(a, "@q"          , 12) == 0)     mode_logger = 'q';
      else if (strncmp(a, "@l"          , 12) == 0)     mode_logger = 's';
      else if (strncmp(a, "@lo"         , 12) == 0)     mode_logger = 'o';
      else if (strncmp(a, "@lp"         , 12) == 0)     mode_logger = 'p';
      else if (strncmp(a, "@ls"         , 12) == 0)     mode_logger = 's';
   }
   /*---(complete)-----------------------*/
   return 0;
}

char       /*----: process the command line arguments (normal) ---------------*/
prog_args          (int argc, char *argv[])
{
   /*---(locals)-------------------------*/
   int       i         = 0;            /* loop iterator -- arguments          */
   char     *a         = NULL;         /* current argument                    */
   /*---(process)------------------------*/
   for (i = 1; i < argc; ++i) {
      a = argv[i];
      if (a[0] == '@')       continue;
      if      (strncmp(a, "-d"          , 12) == 0)     mode_daemon      = 'y';
      else if (strncmp(a, "--daemon"    , 12) == 0)     mode_daemon      = 'y';
      else if (strncmp(a, "-f"          , 12) == 0)     mode_daemon      = '-';
      else if (strncmp(a, "--foreground", 12) == 0)     mode_daemon      = '-';
      else if (strncmp(a, "-1"          , 12) == 0)     mode_init        = 'y';
      else if (strncmp(a, "--init"      , 12) == 0)     mode_init        = 'y';
      else if (strncmp(a, "-a"          , 12) == 0)     mode_init        = '-';
      else if (strncmp(a, "--anypid"    , 12) == 0)     mode_init        = '-';
      else if (strncmp(a, "-h"          , 12) == 0)     prog_usage ();
      else if (strncmp(a, "--help"      , 12) == 0)     prog_usage ();
   }
   /*---(complete)-----------------------*/
   return 0;
}

char       /*----: prepare the program for execution -------------------------*/
prog_begin         (void)
{
   /*---(locals)-------*-----------------*/
   int       rc        = 0;            /* generic return code                 */
   char      xtmpfs    = 'n';
   /*---(defense : uid)------------------*/
   if (getuid() != 0) {                      /* root only                     */
      printf("eos can only be run by superuser\n");
      exit (ERR_NOT_ROOT);         /* boot is always superuser, so no worries  */
   }
   /*---(defenses : pid)-----------------*/
   if (mode_init == 'y' && getpid() != 1) {  /* process number 1 only            */
      printf("eos can only run as process one (pid == 1)\n");
      exit (ERR_NOT_PID_ONE);     /* boot is always pid 1, so no worries      */
   }
   /*---(check on log filesystem)--------*/
   f = fopen ("/proc/mounts", "r");
   if (f == NULL) {
      printf("eos can not access /proc/mounts\n");
      exit (-3);
   }
   while (!feof (f)) {
      fgets (recd, 450, f);
      if (strncmp (recd, "varlog ", 7) == 0) {
         xtmpfs = 'y';
         break;
      }
   }
   fclose (f);
   /*---(setup log filesystem)-----------*/
   if (xtmpfs == 'n') {
      rc = mount("varlog", "/var/log/yLOG", "tmpfs", MS_NOSUID | MS_NODEV | MS_NOEXEC | MS_NOATIME, "size=50m");
      if (rc != 0) {
         printf("can not mount tmpfs for logging (rc=%d)\n", rc);
         exit (ERR_NO_TMPFS);
      }
   }
   /*---(initialize logging)-------------*/
   switch (mode_logger) {
   case  'q' : my.logger = yLOG_begin("eos", yLOG_STDOUT  , yLOG_QUIET);  break;
   case  'o' : my.logger = yLOG_begin("eos", yLOG_STDOUT  , yLOG_NOISE);  break;
   case  'p' : my.logger = yLOG_begin("eos", yLOG_PERSONAL, yLOG_NOISE);  break;
   case  's' : my.logger = yLOG_begin("eos", yLOG_SYSTEM  , yLOG_NOISE);  break;
   default   : my.logger = yLOG_begin("eos", yLOG_SYSTEM  , yLOG_NOISE);  break;
   }
   if (my.logger < 1) {
      printf("eos : can not start logger, FATAL\n");
      exit (ERR_NO_LOGGER);
   }
   /*---(log header)---------------------*/
   yLOG_info  ("purpose",  "clean, light, consistent system initialization");
   yLOG_info  ("namesake", "goddess of daybreak (rosy-fingered dawn)");
   yLOG_info  ("ver_num",  EOS_VER_NUM);
   yLOG_info  ("ver_txt",  EOS_VER_TXT);
   yLOG_info  ("cli args", "none");
   /*---(init)----------------------------------*/
   yLOG_enter (__FUNCTION__);
   /*---(process initd.conf)--------------------*/
   yLOG_info  ("UID",      "properly logged in as superuser");
   if (mode_init == 'y')  yLOG_info  ("PID",      "running as process id 1");
   else                   yLOG_info  ("PID",      "running in anypid mode");
   if (xtmpfs    == 'y')  yLOG_info  ("tmpfs",    "yLOG tmpfs already mounted");
   else                   yLOG_info  ("tmpfs",    "mounted the yLOG tmpfs");
   yLOG_info  ("logger",   "heatherly custom logger up and running");
   /*---(startup dlst library)------------------*/
   yLOG_info  ("yDLST"    ,"starting");
   yDLST_begin ();
   /*---(complete)------------------------------*/
   yLOG_exit  (__FUNCTION__);
   return 0;
}

char       /*----: wrapup the programs execution -----------------------------*/
prog_end           (void)
{
   yLOG_enter (__FUNCTION__);
   yLOG_info  ("yDLST"    ,"shutting down DLST facility");
   yDLST_end ();
   yLOG_info  ("logger",   "shutting down logger");
   yLOG_exit  (__FUNCTION__);
   yLOG_end   ();
   return 0;
}

int        /*----: entry point -----------------------------------------------*/
main               (int argc, char *argv[])
{
   /*---(locals)-------------------------*/
   char      rc        = 0;
   /*---(start)--------------------------*/
   if (rc == 0)  rc = prog_init   ();
   if (rc == 0)  rc = prog_urgent (argc, argv);
   if (rc == 0)  rc = prog_args   (argc, argv);
   if (rc == 0)  rc = prog_begin  ();
   if (rc == 0)  rc = eos_daemon  ();
   if (rc == 0)  rc = eos_signal  ();
   if (rc != 0) {
      prog_end    ();
      exit (-1);
   }
   /*---(process eos.conf)---------------*/
   if (rc == 0)  rc = eos_open     ();
   while (rc == 0) {
      rc = eos_read     ();
      if (rc != 0)           break;
      if (rc == 0)  rc = eos_parse    ();
      /*> eos_launch   ();                                                            <*/
   }
   eos_parse  ();
   rc = 0;
   eos_list     ();
   if (f != NULL)  eos_close    ();

   prog_end    ();
   exit (0);


   /*---(wait for jobs to complete)------*/
   while (1) {
      sleep(1);
      if (complete == 'n') {
         eos_done     ();
         if (running  <= 0  ) {
            complete = 'y';
            yLOG_note  ("ALL JOBS COMPLETE");
            eos_report   ();
            break;
         }
      } else break;
   }
   /*> while (1) {                                                                    <* 
    *>    sleep(5);                                                                   <* 
    *> }                                                                              <*/
   /*---(complete)-----------------------*/
   yLOG_end   ();
   execvp("/sbin/kharon", NULL);
   /*> execvp("/bin/login", NULL);                                                    <*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      daemons and signals                     ----===*/
/*====================------------------------------------====================*/
static void      o___DAEMON__________________o (void) {;}

char               /* PURPOSE : daemonize the program ------------------------*/
eos_daemon         (void)
{
   yLOG_enter (__FUNCTION__);
   /*---(locals)--------------------------------*/
   int       i    = 0;                       /* loop iterator                 */
   int       fd   = 0;                       /* file descriptor               */
   /*---(check for foreground mode)-------------*/
   if (mode_daemon != 'y') {
      yLOG_info  ("mode",   "user requested foreground mode running");
      yLOG_exit  (__FUNCTION__);
      return 0;
   }
   yLOG_info  ("mode",   "moving into daemon mode");
   /*---(fix the umask)-------------------------*/
   umask(0077);
   yLOG_info  ("umask",  "tighten the default file permissions");
   /*---(close off all descriptors)-------------*/
   for (i = 0; i < 256; ++i) {
      if (i == my.logger) continue;
      close(i);
   }
   yLOG_info  ("fds",    "closed all inherited file descriptors");
   /*---(tie std fds to the bitbucket)----------*/
   fd = open("/dev/null", O_RDWR);
   if (fd < 0) {
      yLOG_info  ("fds",    "creation of safe fd FAILED");
   }
   dup2(fd, 0);
   dup2(fd, 1);
   dup2(fd, 2);
   yLOG_info  ("std fds",   "redirect stdin, stdout, and stderr to /dev/null");
   /*---(complete)------------------------------*/
   yLOG_exit  (__FUNCTION__);
   return 0;
}

char       /*----: setup signal handling -------------------------------------*/
eos_signal         (void)
{
   /*---(design notes)-------------------*/
   /* options are...
    *    signal()        -- race conditions when they come fast (depricated)
    *    sigaction()     -- preferred method and standard in c
    *    signalfd()      -- linux specific that treats it like a file
    *
    */
   yLOG_enter (__FUNCTION__);
   /*---(set up structure)---------------*/
   struct sigaction sa;
   /*---(start with ignores)-------------*/
   yLOG_info  ("terminal", "ignore STOP, CONT, TIN, and TOUT");
   sa.sa_handler   = SIG_IGN;
   sa.sa_sigaction = NULL;
   sa.sa_flags     = SA_RESTART;
   /*> sigemptyset(&sa.sa_mask);                                                      <*/
   sigfillset(&sa.sa_mask);
   sa.sa_restorer  = NULL;
   sigaction(SIGINT ,  &sa      , NULL);       /* keyboard interrupt          */
   sigaction(SIGQUIT,  &sa      , NULL);       /* keyboard quit               */
   sigaction(SIGTERM,  &sa      , NULL);       /* terminate                   */
   sigaction(SIGCONT,  &sa      , NULL);       /* continue                    */
   sigaction(SIGTSTP,  &sa      , NULL);       /* keyboard stop               */
   sigaction(SIGTTIN,  &sa      , NULL);       /* tty input for background    */
   sigaction(SIGTTOU,  &sa      , NULL);       /* tty output for background   */
   /*---(then to handles)----------------*/
   sa.sa_handler   = NULL;
   sa.sa_sigaction = eos_comm;
   /*> sa.sa_flags     = SA_RESTART | SA_NOCLDSTOP | SA_NODEFER | SA_SIGINFO;         <*/
   /*> sa.sa_flags     = SA_RESTART | SA_NOCLDSTOP | SA_SIGINFO;                      <*/
   sa.sa_flags     = SA_RESTART | SA_SIGINFO;
   /*> sigemptyset(&sa.sa_mask);                                                      <*/
   sigfillset(&sa.sa_mask);
   sa.sa_restorer  = NULL;
   yLOG_info  ("children", "pay attention to children");
   sigaction(SIGCHLD,  &sa      , NULL);       /* watch for children          */
   yLOG_info  ("program",  "look for HUP");
   sigaction(SIGHUP ,  &sa      , NULL);       /* hangup means refresh        */
   yLOG_info  ("baddies",  "handle SEGV and TERM");
   sigaction(SIGTERM,  &sa      , NULL);       /* catch a kill                */
   sigaction(SIGSEGV,  &sa      , NULL);       /* catch a segfault            */
   /*---(complete)-----------------------*/
   yLOG_exit  (__FUNCTION__);
   return 0;
}

void       /*----: handle signals --------------------------------------------*/
eos_comm           (int a_signal, siginfo_t *a_info, void *a_nada)
{
   /*---(locals)-------------------------*/
   int       status    = 0;
   struct rusage r_use;
   int       rpid = 0;
   sigset_t  new_mask, old_mask;
   /*> sigfillset(&new_mask);                                                         <* 
    *> sigprocmask(SIG_BLOCK, &new_mask, &old_mask);                                  <*/
   switch (a_signal) {
   case  SIGCHLD:
      yLOG_senter("sigchld");
      yLOG_spoint((void *) a_info);
      yLOG_svalue("pid"     , (int) a_info->si_pid);
      yLOG_svalue("rc"      , (int) a_info->si_status);
      yLOG_snote ("wait");
      wait4(a_info->si_pid, &status, WNOHANG, &r_use);
      yLOG_svalue("utime"   , (int) ((r_use.ru_utime.tv_sec * 1000) + (r_use.ru_utime.tv_usec / 1000)));
      yLOG_svalue("stime"   , (int) ((r_use.ru_stime.tv_sec * 1000) + (r_use.ru_stime.tv_usec / 1000)));
      yLOG_svalue("switches", (int) r_use.ru_nvcsw + r_use.ru_nivcsw);
      yLOG_snote ("done");
      yLOG_sexit ();
      rpid   = eos_find (a_info->si_pid);
      if (rpid >= 0 && rpid < nprocs) {
         procs[rpid].status = 'c';
         procs[rpid].end    = yLOG_time();;
         procs[rpid].swaps  = (int) (r_use.ru_nvcsw + r_use.ru_nivcsw);
         procs[rpid].u_time = (int) ((r_use.ru_utime.tv_sec * 1000) + (r_use.ru_utime.tv_usec / 1000));
         procs[rpid].s_time = (int) ((r_use.ru_stime.tv_sec * 1000) + (r_use.ru_stime.tv_usec / 1000));
         if (strchr("ac", procs[rpid].type) != 0)  --running;
      }
      break;
   case  SIGHUP:
      yLOG_info  ("SIGNAL", "SIGHUP MEANS REFRESH");
      break;
   case  SIGTERM:
      yLOG_info  ("SIGNAL", "SIGTERM means terminate daemon (but eos can't)");
      break;
   case  SIGSEGV:
      yLOG_info  ("SIGNAL", "SIGSEGV means daemon blew up (but eos can't)");
      break;
   }
   /*> sigprocmask(SIG_SETMASK, &old_mask, NULL);                                     <*/
   /*---(complete)------------------------------*/
   return;
}



/*====================------------------------------------====================*/
/*===----                         configuration                        ----===*/
/*====================------------------------------------====================*/
static void      o___CONFIG__________________o (void) {;};

char               /* PURPOSE : open up the configuration file ---------------*/
eos_open           (void)
{
   yLOG_enter (__FUNCTION__);
   yLOG_info  ("file",    CONF);
   /*---(open)---------------------------*/
   f = fopen(CONF, "r");
   yLOG_point ("file*",   f);
   /*---(test)---------------------------*/
   if (f == NULL) {
      printf("eos : can not get initd.conf file, FATAL\n");
      yLOG_info  ("FAILED",  "can not open the initd.conf file");
      return -1;
   }
   /*---(status)-------------------------*/
   yLOG_info  ("status",  "file has been successfully opened");
   yLOG_exit  (__FUNCTION__);
   /*---(complete)-----------------------*/
   return 0;
}

char
eos_read           (void)
{
   int       len   = 0;
   int       rc;
   while (1) {
      fgets(recd, MAX_RECD - 5, f);
      if (feof(f))    return -1;
      len = strlen(recd) - 1;
      recd[len] = '\0';
      if (recd[0] == '#' )   continue;
      ++nrecd;
      break;
   }
   return 0;
}

tPROC*     /*----: create a new proc opject ----------------------------------*/
eos_new            (char *a_name, char a_type, char *a_desc, char *a_user, char *a_cmd)
{
   /*---(locals)-------------------------*/
   tPROC    *xproc     = NULL;
   int       i         = 0;
   /*---(create)-------------------------*/
   for (i = 0; i < 3; ++i) {
      xproc = (tPROC *) malloc(sizeof(tPROC));
      if (xproc != NULL)     break;
   }
   if (xproc == NULL)        return NULL;
   /*---(fill out)--------------------*/
   xproc->recd    = -1;
   strncpy (xproc->name, a_name, 50);
   xproc->type    = a_type;
   strncpy (xproc->desc, a_desc, 50);
   strncpy (xproc->user, a_user, 50);
   xproc->uid     = -1;
   strncpy (xproc->run , a_cmd, 200);
   /*---(complete)--------------------*/
   return xproc;
}

tPROC*     /*----: create a new milestone entry ------------------------------*/
eos_milestone      (char *a_name)
{
   /*---(locals)-------------------------*/
   tPROC    *xproc     = NULL;
   int       i         = 0;
   /*---(create)-------------------------*/
   for (i = 0; i < 3; ++i) {
      xproc = (tPROC *) malloc(sizeof(tPROC));
      if (xproc != NULL)     break;
   }
   if (xproc == NULL)        return NULL;
   /*---(fill out)--------------------*/
   xproc->recd    = -1;
   strncpy (xproc->name, a_name, 50);
   xproc->type    = 'm';
   strncpy (xproc->desc, a_name, 50);
   xproc->user[0] = '\0';
   xproc->uid     = -1;
   xproc->run[0]  = '\0';
   /*---(complete)--------------------*/
   return xproc;
}

char
eos_parse          (void)
{
   /*---(locals)-------*-----------------*/
   char      temp [MAX_RECD];
   int       len       = 0;
   int       rc;
   char     *p         = NULL;
   char     *q         = "\x1F";
   static    int       xgroup    = -1;
   static    char      xgname [200] = "";
   static    char      xmsg   [200] = "";
   static    int       link1     = -1;
   static    int       link2     = -1;
   char      name [200];
   int       xitem     = -1;
   tPROC    *xproc     = NULL;
   char     *xdesc     = NULL;
   char     *xuser     = NULL;
   char     *xcmd      = NULL;
   /*---(backup)-------------------------*/
   strncpy (temp, recd, MAX_RECD);
   if (xgroup >= 0 && recd[0] == '\0' && strcmp (xgname, "") != 0) {
      sprintf (xmsg, "%s_end", xgname);
      xproc = eos_milestone (xmsg);
      rc = yDLST_new    ( xgroup, NULL, strdup(xmsg), xproc);
      strcpy (xgname, "");
      return 0;
   }
   /*---(parse out ref)------------------*/
   p = strtok (temp, q);
   if (p == NULL) return 0;
   str_trim (p);
   strncpy (name, p, 150);
   p = strtok (NULL, q);
   if (p == NULL) return 0;
   str_trim (p);
   len = strlen (p);
   if (strncmp ("GROUP", p, 5) == 0) {
      strcpy (xgname, name);
      p = strtok (NULL, q);
      if (p == NULL) return 0;
      str_trim (p);
      rc = yDLST_create (&xgroup, name, strdup (name));
      sprintf (xmsg, "%s_beg", xgname);
      xproc = eos_milestone (xmsg);
      rc = yDLST_new    ( xgroup, NULL, strdup(xmsg), xproc);
   } else if (strncmp ("LLINK", name, 5) == 0) {
      p = strtok (NULL, q);
      if (p == NULL) return 0;
      str_trim (p);
      sprintf (xmsg, "%s_end", p);
      link1 = yDLST_search (xmsg);
      printf ("tie (%4d) %s  ", link1, xmsg);
      p = strtok (NULL, q);
      if (p == NULL) return 0;
      str_trim (p);
      sprintf (xmsg, "%s_beg", p);
      link2 = yDLST_search (xmsg);
      printf ("to (%4d) %s\n", link2, xmsg);
      if (link1 >= 0 && link2 >= 0) {
         yDLST_tie (link1, link2, NULL);
      }
   } else {
      xdesc = strtok (NULL, q);
      if (xdesc == NULL) return 0;
      str_trim (xdesc);
      xuser = strtok (NULL, q);
      if (xuser == NULL) return 0;
      str_trim (xuser);
      xcmd = strtok (NULL, q);
      if (xcmd == NULL) return 0;
      str_trim (xcmd);
      xproc = eos_new (name, p[0], xdesc, xuser, xcmd);
      strcpy (xproc->name, name);
      if (xgroup >= 0) rc = yDLST_new    ( xgroup, &xitem, strdup (name), xproc);
   }
   /*---(complete)-----------------------*/
   return 0;
}

char
eos_list           (void)
{
   yLOG_enter (__FUNCTION__);
   char     *xname     = NULL;
   tPROC    *xdata     = NULL;
   char     *xsave     = NULL;
   xname = yDLST_lists (HEAD);
   while (xname != NULL) {
      xsave = xname;
      yLOG_enter (xsave);
      xdata = (tPROC*) yDLST_list (CURR, HEAD);
      while (xdata != NULL) {
         yLOG_info  ("link",    xdata->name);
         xdata = yDLST_list (CURR, NEXT);
      }
      yLOG_exit  (xsave);
      xname = yDLST_lists (NEXT);
   }
   /*---(complete)-----------------------*/
   yLOG_exit  (__FUNCTION__);
   return 0;
}

char
eos_list_OLD (void)
{
   yLOG_enter (__FUNCTION__);
   /*---(locals)--------------------------------*/
   char      buffer[LINE];
   char      backup[LINE];
   int       len   = 0;
   int       recds = 0;
   int       rc;
   char     *xname, *xdesc, *xtype, *xcount, *xuser, *xexpect, *xrecover, *xcmd;
   char      ztype;
   char      zcount;
   int       zuid;
   char     *p = NULL;
   char     *q = "\x1F";
   tPASSWD  *pass;
   /*---(run input lines)-----------------------*/
   nprocs = 0;
   while (1) {
      /*---(get the line)-----------------------*/
      fgets(buffer, LINE - 5, f);
      if (feof(f))    break;
      len = strlen(buffer) - 1;
      buffer[len] = '\0';
      ++recds;
      /*---(defense)----------------------------*/
      if (buffer[0] == '#')            continue;
      if (len       <  20 )            continue;
      /*---(read)-------------------------------*/
      yLOG_break();
      yLOG_value ("recd#"   , recds);
      switch (buffer[13]) {
      case 'c' : 
      case 'a' : 
      case 's' : 
      case 'd' : 
         rc = eos_read_OLD (buffer, recds);
         if (rc == 0) ++nprocs;
         break;
      }
   }
   /*---(complete)------------------------------*/
   yLOG_exit  (__FUNCTION__);
   return  0;
}

char               /* PURPOSE : read in a process record ---------------------*/
eos_read_OLD       (char *a_buffer, int a_nrecd)
{
   yLOG_enter (__FUNCTION__);
   /*---(locals)--------------------------------*/
   char      backup[LINE];
   int       len   = 0;
   int       rc;
   char     *xname, *xdesc, *xtype, *xcount, *xuser, *xexpect, *xrecover, *xcmd;
   char      ztype;
   char      zcount;
   int       zuid;
   char     *p = NULL;
   char     *q = "\x1F";
   tPASSWD  *pass;
   /*---(read)-------------------------------*/
   strncpy(backup, a_buffer, LINE);
   /*---(check name)-------------------------*/
   xname    = str_trim(strtok(backup, q));
   if (xname == NULL) {
      yLOG_warn  ("name",  "can not be null");
      yLOG_exit  (__FUNCTION__);
      return -2;
   }
   len = strlen(xname);
   if (len < 1 || len >= NAME - 1) {
      yLOG_warn  ("name",  "not the proper length (1 - 10)");
      yLOG_exit  (__FUNCTION__);
      return -2;
   }
   yLOG_info  ("name"    , xname);
   /*---(check type)-------------------------*/
   xtype    = str_trim(strtok(NULL  , q));
   if (xtype == NULL) {
      yLOG_warn  ("type",  "can not be null");
      yLOG_exit  (__FUNCTION__);
      return -1;
   }
   if      (strcmp(xtype,   "config")    == 0)  ztype    = 'c';
   else if (strcmp(xtype,   "action")    == 0)  ztype    = 'a';
   else if (strcmp(xtype,   "serial")    == 0)  ztype    = 's';
   else if (strcmp(xtype,   "daemon")    == 0)  ztype    = 'd';
   else {
      yLOG_warn  ("type",  "not a valid type (casd)");
      yLOG_exit  (__FUNCTION__);
      return -1;
   }
   yLOG_char  ("type"    , ztype);
   /*---(check desc)-------------------------*/
   xdesc    = str_trim(strtok(NULL  , q));
   if (xdesc == NULL) {
      yLOG_warn  ("desc",  "can not be null");
      yLOG_exit  (__FUNCTION__);
      return -3;
   }
   len = strlen(xdesc);
   if (len < 1 || len >= DESC - 1) {
      yLOG_warn  ("desc",  "not the proper length (1 - 50)");
      yLOG_exit  (__FUNCTION__);
      return -3;
   }
   yLOG_info  ("desc"    , xdesc);
   /*---(check user)-------------------------*/
   xuser    = str_trim(strtok(NULL  , q));
   if (xuser == NULL) {
      yLOG_warn  ("user",  "can not be null");
      yLOG_exit  (__FUNCTION__);
      return -4;
   }
   pass = getpwnam(xuser);                     /* get password entry       */
   if (pass == NULL) {
      yLOG_warn  ("user",  "user name not a valid account on the system");
      yLOG_exit  (__FUNCTION__);
      return -4;
   }
   zuid       = pass->pw_uid;
   yLOG_value ("UID"     , zuid);
   /*---(check command)----------------------*/
   xcmd     = str_trim(strtok(NULL  , q));
   if (xdesc == NULL) {
      yLOG_warn  ("cmd",   "can not be null");
      yLOG_exit  (__FUNCTION__);
      return -5;
   }
   len = strlen(xcmd);
   if (len < 1 || len >= CMD  - 1) {
      yLOG_warn  ("cmd",   "not the proper length (1 - 500)");
      yLOG_exit  (__FUNCTION__);
      return -5;
   }
   yLOG_info  ("cmd"     , xcmd);
   /*---(parse for testing)---------------------*/
   char    backup2[CMD];
   strncpy(backup2, xcmd, CMD);
   str_parse (backup2);
   /*---(save)----------------------------------*/
   procs[nprocs].recd  = a_nrecd;
   procs[nprocs].type  = ztype;
   strcpy(procs[nprocs].name  , xname);
   strcpy(procs[nprocs].desc  , xdesc);
   procs[nprocs].uid   = zuid;
   strcpy(procs[nprocs].user  , xuser);
   strcpy(procs[nprocs].run   , xcmd);
   procs[nprocs].status  = '-';
   procs[nprocs].rpid    = 0;
   procs[nprocs].start   = 0;
   procs[nprocs].end     = 0;
   /*---(complete)------------------------------*/
   yLOG_exit  (__FUNCTION__);
   return  0;
}

char               /* PURPOSE : open up the configuration file ---------------*/
eos_close          (void)
{
   int       rc        = 0;            /* generic return code                 */
   yLOG_enter (__FUNCTION__);
   yLOG_info  ("file",    CONF);
   yLOG_value ("nprocs#"   , nprocs);
   /*---(open)---------------------------*/
   rc = fclose(f);
   if (rc == 0) {
      yLOG_info  ("status",  "file has been successfully closed");
   } else {
      yLOG_info  ("FAILURE",  "file can't be closed");
   }
   f = NULL;
   /*---(complete)-----------------------*/
   yLOG_exit  (__FUNCTION__);
   return 0;
}

char
eos_report         (void)
{
   yLOG_enter (__FUNCTION__);
   int       rc   = 0;
   int       i    = 0;                       /* loop iterator                 */
   int       status    = 0;
   struct rusage r_use;
   FILE     *output = NULL;
   yLOG_info  ("file"    , PERFORM);
   output = fopen(PERFORM, "a");
   yLOG_point ("file*"   , output);
   fprintf(output, "ref | ---name--- | -----------------------desc----------------------- | ---user--- | -rpid- | --start--- | ---end---- | --u_time-- | --s_time-- | swap | ---dur---- |\n");
   yLOG_note  ("writing lines...");
   for (i = 0; i < nprocs; ++i) {
      fprintf(output, "%3d | %-10s | %-50s | %-10s | %6d | %10lld | %10lld | %10ld | %10ld | %4d | %10lld |\n", i,
            procs[i].name   , procs[i].desc   , procs[i].user  ,
            procs[i].rpid   , procs[i].start  , procs[i].end   ,
            procs[i].u_time , procs[i].s_time , procs[i].swaps ,
            procs[i].end - procs[i].start);
   }
   fprintf(output, "ref | ---name--- | -----------------------desc----------------------- | ---user--- | -rpid- | --start--- | ---end---- | --u_time-- | --s_time-- | swap | ---dur---- |\n");
   fflush (output);
   yLOG_info  ("flushing", "done");
   fclose (output);
   output = NULL;
   yLOG_info  ("closing" , "done");
   yLOG_exit  (__FUNCTION__);
   return  0;
}




/*====================------------------------------------====================*/
/*===----                           processes                          ----===*/
/*====================------------------------------------====================*/
static void      o___PROCESSES_______________o (void) {;}

char
eos_launch         (void)
{
   yLOG_enter (__FUNCTION__);
   int       i    = 0;                       /* loop iterator                 */
   for (i = 0; i < nprocs; ++i) {
      printf("#%02d :: %-10s :: %s\n", i, procs[i].name, procs[i].desc);
      eos_run(i);
   }
   yLOG_exit  (__FUNCTION__);
   return  0;
}

char
eos_find           (int a_rpid)
{
   int       i    = 0;                       /* loop iterator                 */
   for (i = 0; i < nprocs; ++i) {
      if (procs[i].rpid == a_rpid) return i;
   }
   return  -1;
}

char
eos_run    (int a_proc)
{
   yLOG_senter ("RUN");
   /*---(locals)--------------------------------*/
   int       rc        = 0;                       /* simple return code       */
   int       rpid      = 0;                       /* child pid for execution  */
   tPASSWD  *pass;
   FILE     *output = NULL;
   long      now;                                 /* present datetime         */
   tTIME    *curr_time = NULL;
   char      msg[200];
   char      envp[10][200];
   sigset_t  new_mask, old_mask;
   char      backup[CMD]    = "";          /* copy of the command for parsing */
   char     *p[20];                        /* pointers to cli arguments       */
   int       i         = 0;                /* loop iterator -- arguement      */
   /*---(prepare)------------------------*/
   procs[a_proc].rpid   = 0;
   /*---(fork off to execute)-------------------*/
   snprintf(msg, 200, "%-16.16s,%3d", procs[a_proc].name, procs[a_proc].recd);
   yLOG_snote(msg);
   yLOG_snote("block");
   sigfillset(&new_mask);
   sigprocmask(SIG_BLOCK, &new_mask, &old_mask);
   yLOG_snote("fork");
   rpid = fork();
   if (rpid < 0) {                        /* error                            */
      yLOG_snote ("FAILURE");
      yLOG_sexit ();
      return -1;
   }
   if (rpid > 0) {
      yLOG_svalue ("pid", rpid);
      /*---(link into proc dll)-----------------*/
      procs[a_proc].rpid       = rpid;
      /*> proclist_add (a_line);                                                      <*/
      yLOG_snote  ("unblock");
      sigprocmask(SIG_SETMASK, &old_mask, NULL);
      /*---(log and exit)-----------------------*/
      yLOG_snote  ("SUCCESS");
      yLOG_sexit  ();
      procs[a_proc].status = 'r';
      procs[a_proc].start  = yLOG_time();;
      if (strchr("ac", procs[a_proc].type) != 0)  ++running;
      return 0;              /* parent moves on to next task     */
   }
   sigprocmask(SIG_SETMASK, &old_mask, NULL);
   output = fopen(STUFF, "a");
   fprintf(output, "\n\n");
   /*---(display header for debugging)----------*/
   fprintf(output, "=== execution logging ==================================================begin===\n");
   /*---(get the date)-----------------------*/
   now = time(NULL);
   curr_time = localtime(&now);
   strftime(msg, 50, "%Ss %Mm %Hh %dd %mm  %ww", curr_time);
   fprintf(output, "start     : %s\n",   msg);
   /*---(get user information)------------------*/
   pass = getpwnam(procs[a_proc].user);
   if (pass == NULL) {
      exit (-2);
   }
   /*---(set execution environment)-------------*/
   rc = setenv("USER",                 pass->pw_name, 1);
   snprintf(envp[0], 200, "USER=%s",   pass->pw_name);
   fprintf(output, "USER set  : %s\n", pass->pw_name);
   if (rc <  0) {
      exit (-3);
   }
   rc = setenv("HOME",                 pass->pw_dir,  1);
   snprintf(envp[1], 200, "HOME=%s",   pass->pw_dir);
   fprintf(output, "HOME set  : %s\n", pass->pw_dir);
   if (rc <  0) {
      exit (-3);
   }
   rc = setenv("SHELL",                SHELL, 1);
   snprintf(envp[2], 200, "SHELL=%s",  SHELL);
   fprintf(output, "SHELL set : %s\n", SHELL);
   if (rc <  0) {
      exit (-3);
   }
   rc = setenv("PATH",                 PATH, 1);
   snprintf(envp[3], 200, "PATH=%s",   PATH);
   fprintf(output, "PATH set  : %s\n", PATH);
   if (rc <  0) {
      exit (-3);
   }
   /*---(set permissions)-----------------------*/
   rc = initgroups(procs[a_proc].user, pass->pw_gid);
   if (rc <  0) {
      exit (-3);
   }
   rc = setregid(pass->pw_gid,         pass->pw_gid);
   snprintf(envp[4], 200, "GID=%d",    pass->pw_gid);
   fprintf(output, "GID set   : %d\n", pass->pw_gid);
   if (rc <  0) {
      return (-3);
   }
   rc = setreuid(pass->pw_uid, pass->pw_uid);
   snprintf(envp[5], 200, "UID=%d",    pass->pw_uid);
   fprintf(output, "UID set   : %d\n", pass->pw_uid);
   if (rc <  0) {
      return (-3);
   }
   /*---(set current dir)-----------------------*/
   rc = chdir(pass->pw_dir);
   fprintf(output, "chdir     : %s\n", pass->pw_dir);
   if (rc <  0) {
      return (-3);
   }
   /*---(try direct execution)------------------*/
   fprintf(output, "execvp    : %.50s\n", procs[a_proc].run);
   fprintf(output, "==========================================================================end===\n");
   fflush (output);
   fclose (output);
   char    backup2[CMD];
   strncpy(backup2, procs[a_proc].run, CMD);
   str_parse (backup2);
   rc = execvp(*args, args);
   /*---(try shell execution)-------------------*/
   envp[6][0] = '\0';
   output = fopen(STUFF, "a");
   fprintf(output, "FAILED execvp, fallback...\n");
   fprintf(output, "execl     : %.50s\n", procs[a_proc].run);
   fprintf(output, "==========================================================================end===\n");
   fflush (output);
   fclose (output);
   rc = execl(SHELL, SHELL, "-c", procs[a_proc].run, NULL, NULL);
   /*---(this should never come back)-----------*/
   output = fopen(STUFF, "a");
   fprintf(output, "FAILED execl, just won't run\n");
   fprintf(output, "==========================================================================end===\n");
   fflush (output);
   fclose (output);
   _exit (-3);    /* must use _exit to get out properly                       */
}

char
eos_done           (void)
{
   int       rc   = 0;
   int       i    = 0;                       /* loop iterator                 */
   int       status    = 0;
   struct rusage r_use;
   for (i = 0; i < nprocs; ++i) {
      if (procs[i].status == 'c') continue;
      rc = wait4(procs[i].rpid, &status, WNOHANG, &r_use);
      if (rc ==  0) continue;
      yLOG_senter("zombie");
      yLOG_svalue("pid"     , (int) procs[i].rpid);
      yLOG_svalue("pid2"    , (int) rc);
      yLOG_svalue("utime"   , (int) ((r_use.ru_utime.tv_sec * 1000) + (r_use.ru_utime.tv_usec / 1000)));
      yLOG_svalue("stime"   , (int) ((r_use.ru_stime.tv_sec * 1000) + (r_use.ru_stime.tv_usec / 1000)));
      yLOG_svalue("switches", (int) r_use.ru_nvcsw + r_use.ru_nivcsw);
      yLOG_snote ("done");
      yLOG_sexit ();
      procs[i].status = 'c';
      procs[i].end    = yLOG_time();
      if (strchr("ac", procs[i].type) != 0)  --running;
   }
   return  0;
}



/*============================[[ end-of-code ]]===============================*/
