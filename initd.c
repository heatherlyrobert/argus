/*===========================[[ start-of-code ]]==============================*/
#include      "init.h"



char          debug__flow = 'y';



/*====================------------------------------------====================*/
/*===----                         housekeeping                         ----===*/
/*====================------------------------------------====================*/
static void      o___HOUSEKEEPING____________o (void) {;};

char               /* PURPOSE : get the logging up and going -----------------*/
initialize         (const char a_quiet)
{
   /*---(locals)-------------------------*/
   int       rc        = 0;            /* generic return code                 */
   /*---(defense:security)---------------*/
   if (getuid() != 0) {                   /* root only                        */
      printf("initd can only be run by superuser\n");
      exit (-1);   /* boot is always superuser, so no worries  */
   }
   /*---(defense:process)----------------*/
   if (getpid() != 1) {                   /* process number 1 only            */
      printf("initd can only run as process one (pid == 1)\n");
      exit (-2);   /* boot is always pid 1, so no worries      */
   }
   /*---(adjust verbosity)---------------*/
   if      (a_quiet == 0)  { my.quiet = 0; my.updates = 0; }
   else if (a_quiet == 1)  { my.quiet = 0; my.updates = 1; }
   else                    { my.quiet = 1; my.updates = 1; }
   /*---(setup log filesystem)-----------*/
   rc = mount("tmpfs", "/var/log/yLOG.current", "tmpfs", 0, "size=50m");
   if (rc != 0) my.quiet = 1;
   /*---(setup logger)-------------------*/
   my.logger = yLOG_begin("initd", my.quiet);
   if (my.logger < 1) {
      printf("initd : can not start logger, FATAL\n");
      exit(-3);
   }
   /*---(log header)---------------------*/
   yLOG_info  ("purpose",  "consistent, reliable system initialization");
   yLOG_info  ("ver_num",  VER_NUM);
   yLOG_info  ("ver_txt",  VER_TXT);
   yLOG_info  ("cli args", "none");
   /*---(init)----------------------------------*/
   yLOG_enter (__FUNCTION__);
   /*---(process initd.conf)--------------------*/
   yLOG_info  ("UID",     "properly logged in as superuser");
   yLOG_info  ("PID",     "running as process id 1");
   yLOG_info  ("logger",  "created tmpfs for our custom logger");
   yLOG_value ("log_mode",a_quiet);
   /*---(complete)------------------------------*/
   yLOG_exit  (__FUNCTION__);
   return 0;
}


void               /* PURPOSE : handle signals -------------------------------*/
communicate        (int a_signal, siginfo_t *a_info, void *a_nada)
{
   /*---(locals)-------------------------*/
   int       status    = 0;
   struct rusage r_use;
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
      break;
   case  SIGHUP:
      yLOG_info  ("SIGNAL", "SIGHUP MEANS REFRESH CRONTABS BEFORE NEXT RUN");
      break;
   case  SIGTERM:
      yLOG_info  ("SIGNAL", "SIGTERM means terminate daemon");
      /*> terminate("EXITING", 99);                                                   <*/
      break;
   case  SIGSEGV:
      yLOG_info  ("SIGNAL", "SIGSEGV means daemon blew up");
      /*> terminate("EXITING", 99);                                                   <*/
      break;
   }
   signals();    /* must reset signals after use         */
   /*---(complete)------------------------------*/
   return;
}

char               /* PURPOSE : setup signal handling ------------------------*/
signals            (void)
{
   /*---(design notes)-------------------*/
   /* options are...
    *    signal()        -- race conditions when they come fast (depricated)
    *    sigaction()     -- preferred method and standard in c
    *    signalfd()      -- linux specific that treats it like a file
    *
    */
   /*---(set up structure)---------------*/
   struct sigaction sa;
   sigemptyset(&sa.sa_mask);
   sa.sa_restorer  = NULL;
   /*---(start with ignores)-------------*/
   sa.sa_handler   = SIG_IGN;
   sa.sa_sigaction = NULL;
   sa.sa_flags     = SA_RESTART;
   sigaction(SIGTSTP,  &sa      , NULL);       /* watch for children          */
   sigaction(SIGTTOU,  &sa      , NULL);       /* watch for children          */
   sigaction(SIGTTIN,  &sa      , NULL);       /* watch for children          */
   /*---(then to handles)----------------*/
   sa.sa_handler   = NULL;
   sa.sa_sigaction = communicate;
   sa.sa_flags     = SA_RESTART | SA_NOCLDSTOP | SA_SIGINFO;
   sigaction(SIGCHLD,  &sa      , NULL);       /* watch for children          */
   sigaction(SIGHUP ,  &sa      , NULL);       /* hangup means refresh        */
   sigaction(SIGTERM,  &sa      , NULL);       /* catch a kill                */
   sigaction(SIGSEGV,  &sa      , NULL);       /* catch a segfault            */
   /*---(complete)-----------------------*/
   return 0;
}

char               /* PURPOSE : daemonize the program ------------------------*/
daemonize          (void)
{
   yLOG_enter (__FUNCTION__);
   /*---(locals)--------------------------------*/
   int       i    = 0;                       /* loop iterator                 */
   int       fd   = 0;                       /* file descriptor               */
   /*---(fix the umask)-------------------------*/
   umask(0077);
   yLOG_info  ("umask",  "reset the default file permissions");
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
      /*> terminate(__FUNCTION__, 2);                                                 <*/
   }
   dup2(fd, 0);
   dup2(fd, 1);
   dup2(fd, 2);
   yLOG_info  ("std fds",   "redirect stdin, stdout, and stderr to /dev/null");
   /*---(signals)-------------------------------*/
   yLOG_info  ("signals",  "setup signal handlers");
   signals();
   /*---(complete)------------------------------*/
   yLOG_exit  (__FUNCTION__);
   return 0;
}

char        /* PURPOSE : ensure single threading                       */
lock          (void)
{
   return 0;
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
inventory     (FILE *a_source)
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
      fgets(buffer, LINE - 5, a_source);
      if (feof(a_source))    break;
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
         rc = read_process(buffer, recds);
         if (rc == 0) ++nprocs;
         break;
      }
   }
   /*---(complete)------------------------------*/
   yLOG_exit  (__FUNCTION__);
   return  0;
}


char               /* PURPOSE : read in a process record ---------------------*/
read_process       (char *a_buffer, int a_nrecd)
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
   /*---(complete)------------------------------*/
   yLOG_exit  (__FUNCTION__);
   return  0;
}


char
run        (int a_proc)
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
   /*---(prepare)-------------------------------*/
   procs[a_proc].rpid   = 0;
   /*---(fork off to execute)-------------------*/
   snprintf(msg, 200, "%-16.16s,%3d", procs[a_proc].name, procs[a_proc].recd);
   yLOG_snote(msg);
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
      /*---(log and exit)-----------------------*/
      yLOG_snote  ("SUCCESS");
      yLOG_sexit  ();
      return 0;              /* parent moves on to next task     */
   }
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



