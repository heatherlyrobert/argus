/*============================[[ beg-of-code ]]===============================*/

/*===[[ START HDOC ]]=========================================================*/
/*===[[ HEADER ]]=============================================================#

 *   focus         : (SA) system_admin
 *   niche         : (bo) bootup
 *   application   : eos (rosy-fingered dawn, greek goddess of daybreak)
 *   purpose       : consistent, reliable, and fast system initialization
 *
 *   base_system   : gnu/linux
 *   lang_name     : gnu/ansi-c (because it's right, just, best, and universal)
 *   dependencies  : yLOG only
 *   size          : tiny (less than 1,000 slocL)
 * 
 *   author        : the_heatherlys
 *   created       : 2010-10
 *   priorities    : direct, simple, brief, vigorous, and lucid (h.w. fowler)
 *   end goal      : loosely coupled, strict interface, maintainable, portable
 * 
 */
/*===[[ PURPOSE ]]=============================================================*

 *   traditional init has many distinct elements, each of which with a separate
 *   focus.  we would like to see if it can be taken down to pieces and have a
 *   number of very focused programs perform them separately to avoid the
 *   complexity that has arisen from the integration.
 *
 *   eos will be the system initialization program with the very focused mission
 *   of preparing a stable, secure, baseline system on which other programs can
 *   elaborate.
 *
 *
 *
 *
 *   we don't want to just use the system, we want to *own* it; so that means
 *   we have to fundmentally understand the critical services which only tearing
 *   down and rebuilding can really teach -- even if its more basic in the end.
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


#include      "init.h"



char      debug__flow = 'n';
#define   DEBUG_FLOW     if (debug__flow == 'y') 

FILE     *f = NULL;
int       running     = 0;
char      complete    = 'n';

char      eos_init           (void);
void      eos_comm           (int a_sig, siginfo_t *a_info, void *a_nada);
char      eos_signal         (void);
char      eos_daemon         (void);

char      eos_open           (void);
char      eos_list           (void);
char      eos_read           (char*, int);
char      eos_close          (void);
char      eos_report         (void);

char      eos_launch         (void);
char      eos_wait           (void);
char      eos_done           (void);
char      eos_find           (int);
char      eos_run            (int);


/*====================------------------------------------====================*/
/*===----                         housekeeping                         ----===*/
/*====================------------------------------------====================*/



int
main (int argc, char *argv[])
{
   /*---(check for debugging)-------------------*/
   if (strcmp(argv[0], "eosv") == 0) debug__flow = 'y';
   DEBUG_FLOW  printf("eos by the_heatherlys (%s)\n", VER_NUM);
   /*---(start)---------------------------------*/
   eos_init     ();
   eos_daemon   ();
   eos_signal   ();
   /*---(process initd.conf)--------------------*/
   eos_open     ();
   if (f != NULL) {
      eos_list     ();
      eos_close    ();
      eos_launch   ();
   }
   while (1) {
      sleep(1);
      if (complete == 'n') {
         eos_done     ();
         if (running  <= 0  ) {
            complete = 'y';
            yLOG_note  ("ALL JOBS COMPLETE");
            eos_report   ();
         }
      }
   }
   yLOG_end   ();
   /*> execl(SHELL, SHELL, "-c", "/sbin/agetty 38400 tty1 linux", NULL, NULL);        <*/
   return 0;
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
   DEBUG_FLOW  printf("   - configuration file is open\n");
   yLOG_info  ("status",  "file has been successfully opened");
   yLOG_exit  (__FUNCTION__);
   /*---(complete)-----------------------*/
   return 0;
}

char               /* PURPOSE : open up the configuration file ---------------*/
eos_close          (void)
{
   int       rc        = 0;            /* generic return code                 */
   yLOG_enter (__FUNCTION__);
   yLOG_info  ("file",    CONF);
   yLOG_value ("nprocs#"   , nprocs);
   DEBUG_FLOW  printf("found %02d procs\n", nprocs);
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




/*====================------------------------------------====================*/
/*===----                         housekeeping                         ----===*/
/*====================------------------------------------====================*/
static void      o___HOUSEKEEPING____________o (void) {;};

char               /* PURPOSE : get the logging up and going -----------------*/
eos_init           (void)
{
   /*---(locals)-------------------------*/
   int       rc        = 0;            /* generic return code                 */
   int       quiet     = 0;
   /*---(defense:security)---------------*/
   if (getuid() != 0) {                   /* root only                        */
      printf("eos can only be run by superuser\n");
      exit (-1);   /* boot is always superuser, so no worries  */
   }
   DEBUG_FLOW  printf("   - logged in as superuser\n");
   /*---(defense:process)----------------*/
   if (getpid() != 1) {                   /* process number 1 only            */
      printf("eos can only run as process one (pid == 1)\n");
      exit (-2);   /* boot is always pid 1, so no worries      */
   }
   DEBUG_FLOW  printf("   - process id is 1\n");
   /*---(setup log filesystem)-----------*/
   rc = mount("tmpfs", "/var/log/yLOG", "tmpfs", 0, "size=50m");
   if (rc != 0) quiet = 1;
   DEBUG_FLOW  printf("   - logging filesystem is mounted\n");
   /*---(setup logger)-------------------*/
   my.logger = yLOG_begin("eos", quiet);
   if (my.logger < 1) {
      printf("initd : can not start logger, FATAL\n");
      exit(-3);
   }
   DEBUG_FLOW  printf("   - logger started\n");
   /*---(log header)---------------------*/
   yLOG_info  ("purpose",  "consistent, reliable, and fast system initialization");
   yLOG_info  ("namesake", "goddess of daybreak (rosy-fingered dawn");
   yLOG_info  ("ver_num",  VER_NUM);
   yLOG_info  ("ver_txt",  VER_TXT);
   yLOG_info  ("cli args", "none");
   /*---(init)----------------------------------*/
   yLOG_enter (__FUNCTION__);
   /*---(process initd.conf)--------------------*/
   yLOG_info  ("UID",      "properly logged in as superuser");
   yLOG_info  ("PID",      "running as process id 1");
   yLOG_info  ("logger",   "created tmpfs for our custom logger");
   yLOG_value ("log_mode", quiet);
   /*---(complete)------------------------------*/
   yLOG_exit  (__FUNCTION__);
   return 0;
}

char               /* PURPOSE : daemonize the program ------------------------*/
eos_daemon         (void)
{
   yLOG_enter (__FUNCTION__);
   /*---(locals)--------------------------------*/
   int       i    = 0;                       /* loop iterator                 */
   int       fd   = 0;                       /* file descriptor               */
   /*---(fix the umask)-------------------------*/
   umask(0077);
   yLOG_info  ("umask",  "reset the default file permissions");
   DEBUG_FLOW  printf("   - umask tightened\n");
   /*---(close off all descriptors)-------------*/
   for (i = 0; i < 256; ++i) {
      if (i == my.logger) continue;
      close(i);
   }
   yLOG_info  ("fds",    "closed all inherited file descriptors");
   DEBUG_FLOW  printf("   - closed file descriptors\n");
   /*---(tie std fds to the bitbucket)----------*/
   fd = open("/dev/null", O_RDWR);
   if (fd < 0) {
      yLOG_info  ("fds",    "creation of safe fd FAILED");
   }
   dup2(fd, 0);
   dup2(fd, 1);
   dup2(fd, 2);
   yLOG_info  ("std fds",   "redirect stdin, stdout, and stderr to /dev/null");
   DEBUG_FLOW  printf("   - routed stdin, stdout, and stderr to /dev/null\n");
   /*---(complete)------------------------------*/
   yLOG_exit  (__FUNCTION__);
   return 0;
}

char               /* PURPOSE : setup signal handling ------------------------*/
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
   sigfillset(&sa.sa_mask);
   sa.sa_restorer  = NULL;
   /*---(start with ignores)-------------*/
   sa.sa_handler   = SIG_IGN;
   sa.sa_sigaction = NULL;
   sa.sa_flags     = SA_RESTART;
   yLOG_info  ("terminal", "ignore STOP, CONT, TIN, and TOUT");
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
   sa.sa_flags     = SA_RESTART | SA_NOCLDSTOP | SA_SIGINFO;
   yLOG_info  ("children", "pay attention to children");
   sigaction(SIGCHLD,  &sa      , NULL);       /* watch for children          */
   yLOG_info  ("program",  "look for HUP");
   sigaction(SIGHUP ,  &sa      , NULL);       /* hangup means refresh        */
   yLOG_info  ("program",  "handle SEGV and TERM");
   sigaction(SIGTERM,  &sa      , NULL);       /* catch a kill                */
   sigaction(SIGSEGV,  &sa      , NULL);       /* catch a segfault            */
   /*---(complete)-----------------------*/
   yLOG_exit  (__FUNCTION__);
   return 0;
}


void               /* PURPOSE : handle signals -------------------------------*/
eos_comm           (int a_signal, siginfo_t *a_info, void *a_nada)
{
   /*---(locals)-------------------------*/
   int       status    = 0;
   struct rusage r_use;
   int       rpid = 0;
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
      rpid = eos_find (a_info->si_pid);
      if (rpid >= 0 && rpid < nprocs) {
         procs[rpid].status = 'c';
         procs[rpid].end    = yLOG_time();;
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
   /*---(complete)------------------------------*/
   return;
}

char*              /* PURPOSE : clean whitespace from both sides of a string -*/
strtrim            (char *a_cstring)
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

char
eos_list     (void)
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
         rc = eos_read (buffer, recds);
         if (rc == 0) ++nprocs;
         break;
      }
   }
   /*---(complete)------------------------------*/
   yLOG_exit  (__FUNCTION__);
   return  0;
}


char               /* PURPOSE : read in a process record ---------------------*/
eos_read           (char *a_buffer, int a_nrecd)
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
   xname    = strtrim(strtok(backup, q));
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
   xtype    = strtrim(strtok(NULL  , q));
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
   xdesc    = strtrim(strtok(NULL  , q));
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
   xuser    = strtrim(strtok(NULL  , q));
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
   xcmd     = strtrim(strtok(NULL  , q));
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

char
eos_launch         (void)
{
   yLOG_enter (__FUNCTION__);
   int       i    = 0;                       /* loop iterator                 */
   for (i = 0; i < nprocs; ++i) {
      DEBUG_FLOW  printf("#%02d :: %-10s :: %s\n", i, procs[i].name, procs[i].desc);
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

char
eos_report         (void)
{
   yLOG_enter (__FUNCTION__);
   int       rc   = 0;
   int       i    = 0;                       /* loop iterator                 */
   int       status    = 0;
   struct rusage r_use;
   for (i = 0; i < nprocs; ++i) {
      yLOG_senter("line");
      yLOG_sinfo ("name"    , procs[i].name);
      yLOG_svalue("pid"     , (int) procs[i].rpid);
      yLOG_svalue("start"   , (int) procs[i].start);
      yLOG_svalue("end"     , (int) procs[i].end);
      yLOG_snote ("done");
      yLOG_sexit ();
   }
   yLOG_exit  (__FUNCTION__);
   return  0;
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
   /*---(prepare)-------------------------------*/
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
   /*> output = fopen(STUFF, "a");                                                    <*/
   /*---(display header for debugging)----------*/
   /*> fprintf(output, "=== crond : the heatherly cron system ==================================begin===\n");   <*/
   /*---(get the date)-----------------------*/
   now = time(NULL);
   curr_time = localtime(&now);
   strftime(msg, 50, "%Ss %Mm %Hh %dd %mm  %ww", curr_time);
   /*> fprintf(output, "start     : %s\n",   msg);                                    <*/
   /*---(get user information)------------------*/
   pass = getpwnam(procs[a_proc].user);
   if (pass == NULL) {
      exit (-2);
   }
   /*---(set execution environment)-------------*/
   rc = setenv("USER",                 pass->pw_name, 1);
   snprintf(envp[0], 200, "USER=%s",   pass->pw_name);
   /*> fprintf(output, "USER set  : %s\n", pass->pw_name);                            <*/
   if (rc <  0) {
      exit (-3);
   }
   rc = setenv("HOME",                 pass->pw_dir,  1);
   snprintf(envp[1], 200, "HOME=%s",   pass->pw_dir);
   /*> fprintf(output, "HOME set  : %s\n", pass->pw_dir);                             <*/
   if (rc <  0) {
      exit (-3);
   }
   rc = setenv("SHELL",                SHELL, 1);
   snprintf(envp[2], 200, "SHELL=%s",  SHELL);
   /*> fprintf(output, "SHELL set : %s\n", SHELL);                                    <*/
   if (rc <  0) {
      exit (-3);
   }
   rc = setenv("PATH",                 PATH, 1);
   snprintf(envp[3], 200, "PATH=%s",   PATH);
   /*> fprintf(output, "PATH set  : %s\n", PATH);                                     <*/
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
   /*> fprintf(output, "GID set   : %d\n", pass->pw_gid);                             <*/
   if (rc <  0) {
      return (-3);
   }
   rc = setreuid(pass->pw_uid, pass->pw_uid);
   snprintf(envp[5], 200, "UID=%d",    pass->pw_uid);
   /*> fprintf(output, "UID set   : %d\n", pass->pw_uid);                             <+/   <*/
   if (rc <  0) {
      return (-3);
   }
   /*---(set current dir)-----------------------*/
   rc = chdir(pass->pw_dir);
   /*> fprintf(output, "chdir     : %s\n", pass->pw_dir);                             <*/
   if (rc <  0) {
      return (-3);
   }
   /*---(close log)-----------------------------*/
   /*> fprintf(output, "execl     : %.50s\n", procs[a_proc].run);                     <*/
   /*> fprintf(output, "==========================================================================end===\n");   <*/
   /*> fflush (output);                                                               <*/
   /*> fclose (output);                                                               <*/
   /*> system("set > /home/dotsuu/z_gehye/environ.txt");                              <*/
   /*---(execute)-------------------------------*/
   envp[6][0] = '\0';
   /*> _exit (0);                                                                     <*/
   rc = execl(SHELL, SHELL, "-c", procs[a_proc].run, NULL, NULL);
   /*> rc = execle(SHELL, SHELL, "-c", a_line->cmd, NULL, envp);                      <*/
   /*---(this should never come back)-----------*/
   _exit (-3);    /* must use _exit to get out properly                       */
}



