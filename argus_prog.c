#include "argus.h"



/*===[[ GNU GENERAL PUBLIC LICENSE (GPL) ]]===================================*/
/*´´·········1·········2·········3·········4·········5·········6·········7·········8  */

#define  P_COPYRIGHT   \
   "copyright (c) 2012 robert.s.heatherly at balsashrike at gmail dot com"

#define  P_LICENSE     \
   "the only place you could have gotten this code is my github, my website,¦"   \
   "or illegal sharing. given that, you should be aware that this is GPL licensed."

#define  P_COPYLEFT    \
   "the GPL COPYLEFT REQUIREMENT means any modifications or derivative works¦"   \
   "must be released under the same GPL license, i.e, must be free and open."

#define  P_INCLUDE     \
   "the GPL DOCUMENTATION REQUIREMENT means that you must include the original¦" \
   "copyright notice and the full licence text with any resulting anything."

#define  P_AS_IS       \
   "the GPL NO WARRANTY CLAUSE means the software is provided without any¦"      \
   "warranty and the author cannot be held liable for damages."

#define  P_THEFT    \
   "if you knowingly violate the spirit of these ideas, i suspect you might "    \
   "find any number of freedom-minded hackers may take it quite personally ;)"

/*´´·········1·········2·········3·········4·········5·········6·········7·········8  */
/*===[[ GNU GENERAL PUBLIC LICENSE (GPL) ]]===================================*/



tMY         my;
char    g_print     [LEN_RECD]  = "";
char    unit_answer [LEN_RECD]  = "";



/*====================------------------------------------====================*/
/*===----                        program support                       ----===*/
/*====================------------------------------------====================*/
static void      o___SUPPORT____________o (void) {;}

char*
PROG_version            (void)
{
   char    t [20] = "";
#if    __TINYC__ > 0
   strcpy (t, "[tcc built  ]");
#elif  __GNUC__  > 0
   strcpy (t, "[gnu gcc    ]");
#elif  __CBANG__  > 0
   strcpy (t, "[cbang      ]");
#elif  __HEPH__  > 0
   strcpy (t, "[hephaestus ]");
#else
   strcpy (t, "[unknown    ]");
#endif
   snprintf (g_print, LEN_HUND, "%s   %s : %s", t, P_VERNUM, P_VERTXT);
   return g_print;
}

char
PROG_vershow            (void)
{
   fprintf (my.m_output, "%s\n", PROG_version ());
   exit (0);
}

char
PROG_usage              (void)
{
   fprintf (my.m_output, "\n");
   fprintf (my.m_output, "argus      : heatherly system access guardian\n");
   fprintf (my.m_output, "\n");
   fprintf (my.m_output, "namesake   : argos panoptes (all-seeing), giant and hera's watcher\n");
   fprintf (my.m_output, "\n");
   fprintf (my.m_output, "usage      : argus [URGENTS] [OPTIONS]\n");
   fprintf (my.m_output, "\n");
   fprintf (my.m_output, "OPTIONS...\n");
   fprintf (my.m_output, "   --help       program usage/help\n");
   fprintf (my.m_output, "   --files      input files analyses\n");
   fprintf (my.m_output, "   --users      user names and general analysis\n");
   fprintf (my.m_output, "   --groups     group names and general analysis\n");
   fprintf (my.m_output, "   --matrix     user to group matrix\n");
   fprintf (my.m_output, "   --utmp       utmp (current) entries\n");
   fprintf (my.m_output, "   --wtmp       wtmp (historical) entries\n");
   fprintf (my.m_output, "   --btmp       btmp (failure) entries\n");
   fprintf (my.m_output, "   --current    current wtmp entries\n");
   fprintf (my.m_output, "   --lastlog    lastlog entries\n");
   fprintf (my.m_output, "\n");
   fprintf (my.m_output, "URGENTS...\n");
   fprintf (my.m_output, "   @q        quiet execution with no logs\n");
   fprintf (my.m_output, "   @l,@ls    route logging to system logs  (*)\n");
   fprintf (my.m_output, "   @lp       route logging to personal logs\n");
   fprintf (my.m_output, "   @lo       route logging to stdout\n");
   fprintf (my.m_output, "\n");
   exit (0);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        debugging setup                       ----===*/
/*====================------------------------------------====================*/
static void      o___DEBUGGING__________o (void) {;}

char
PROG_debugging          (int a_argc, char *a_argv[])
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG  yLOG_enter   (__FUNCTION__);
   /*---(initialize)---------------------*/
   rc = yURG_logger  (a_argc, a_argv);
   DEBUG_PROG  yLOG_value   ("logger"    , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }

   rc = yURG_urgs    (a_argc, a_argv);
   DEBUG_PROG  yLOG_value   ("urgs"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_PROG  yLOG_exit  (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        program startup                       ----===*/
/*====================------------------------------------====================*/
static void      o___STARTUP____________o (void) {;}

char
PROG__init              (int a_argc, char *a_argv[])
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(default values)-----------------*/
   yURG_all_mute ();
   strcpy (my.m_cmdline, "");
   my.m_runas       = '·';
   my.m_exim        = 'y';
   my.m_titles      = 'y';
   strcpy (my.m_file   , "(stdout)");
   my.m_output      = NULL;
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char
PROG__args              (int a_argc, char *a_argv[])
{
   /*> printf ("   > prog_args\n");                                                   <*/
   /*---(locals)-------------------------*/
   int       i         = 0;            /* loop iterator -- arguments          */
   char     *a         = NULL;         /* current argument                    */
   int       x         = 0;            /* count of arguments                  */
   char      x_good    =  'y';         /* accepted argument                   */
   /*---(header)-------------------------*/
   DEBUG_PROG  yLOG_enter   (__FUNCTION__);
   /*---(process)------------------------*/
   for (i = 1; i < a_argc; ++i) {
      a = a_argv[i];
      if (a[0] == '@')       continue;
      ++x;
      x_good = 'y';
      if      (strncmp(a, "-h"          , 12) == 0)     PROG_usage ();
      else if (strncmp(a, "--help"      , 12) == 0)     PROG_usage ();
      else if (strncmp(a, "--version"   , 12) == 0)     PROG_vershow ();
      /*---(reporting options)-----------*/
      else if (strncmp(a, "--files"     , 12) == 0)     my.m_runas  = RUN_FILES;
      else if (strncmp(a, "--users"     , 12) == 0)     my.m_runas  = RUN_USERS;
      else if (strncmp(a, "--groups"    , 12) == 0)     my.m_runas  = RUN_GROUPS;
      else if (strncmp(a, "--matrix"    , 12) == 0)     my.m_runas  = RUN_MATRIX;
      else if (strncmp(a, "--utmp"      , 12) == 0)     my.m_runas  = RUN_UTMP;
      else if (strncmp(a, "--wtmp"      , 12) == 0)     my.m_runas  = RUN_WTMP;
      else if (strncmp(a, "--btmp"      , 12) == 0)     my.m_runas  = RUN_BTMP;
      else if (strncmp(a, "--current"   , 12) == 0)     my.m_runas  = RUN_CURRENT;
      else if (strncmp(a, "--lastlog"   , 12) == 0)     my.m_runas  = RUN_LASTLOG;
      /*---(output direction)------------*/
      else if (strncmp(a, "--exim"      , 12) == 0)     my.m_exim   = 'y';
      else if (strncmp(a, "--noexim"    , 12) == 0)     my.m_exim   = '·';
      else if (strncmp(a, "--titles"    , 12) == 0)     my.m_titles = 'y';
      else if (strncmp(a, "--notitles"  , 12) == 0)     my.m_titles = '·';
      else if (strncmp(a, "--clip"      , 12) == 0)     strcpy (my.m_file, YSTR_CLIP);
      else if (strncmp(a, "--unit"      , 12) == 0)     strcpy (my.m_file, ARGUS_TEMP);
      /*---(tbd)-------------------------*/
      else                                              x_good = '-';
      if (x_good == 'y') {
         strlcat (my.m_cmdline, " ", LEN_RECD);
         strlcat (my.m_cmdline, a  , LEN_RECD);
      }
   }
   /*---(summary)------------------------*/
   DEBUG_PROG  yLOG_char    ("r_files"   , SHOW_FILES);
   DEBUG_PROG  yLOG_char    ("r_users"   , SHOW_USERS);
   DEBUG_PROG  yLOG_char    ("r_groups"  , SHOW_GROUPS);
   DEBUG_PROG  yLOG_char    ("r_matrix"  , SHOW_MATRIX);
   DEBUG_PROG  yLOG_char    ("r_utmp"    , SHOW_UTMP);
   DEBUG_PROG  yLOG_char    ("r_wtmp"    , SHOW_WTMP);
   DEBUG_PROG  yLOG_char    ("r_btmp"    , SHOW_BTMP);
   DEBUG_PROG  yLOG_char    ("r_current" , SHOW_CURRENT);
   DEBUG_PROG  yLOG_char    ("r_lastlog" , SHOW_LASTLOG);
   DEBUG_PROG  yLOG_info    ("m_file"    , my.m_file);
   DEBUG_PROG  yLOG_info    ("m_cmdline" , my.m_cmdline);
   /*---(complete)-----------------------*/
   DEBUG_PROG  yLOG_exit    (__FUNCTION__);
   return 0;
}

char
PROG__begin             (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG  yLOG_enter   (__FUNCTION__);
   /*---(open-output)--------------------*/
   DEBUG_PROG  yLOG_info    ("m_file"    , my.m_file);
   if (strcmp (my.m_file, "(stdout)") == 0)  rc = yENV_open_stdout (&(my.m_output));
   else                                      rc = yENV_open        ("", my.m_file, 'w', &(my.m_output));
   DEBUG_PROG  yLOG_value   ("open"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_PROG  yLOG_point   ("m_output"  , my.m_output);
   /*---(complete)-----------------------*/
   DEBUG_PROG  yLOG_exit  (__FUNCTION__);
   return 0;
}

char
PROG_startup            (int a_argc, char *a_argv[])
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   yURG_stage_check (YURG_BEG);
   DEBUG_PROG  yLOG_enter   (__FUNCTION__);
   /*---(initialize)---------------------*/
   if (rc >= 0) {
      rc = PROG__init    (a_argc, a_argv);
      DEBUG_PROG  yLOG_value   ("init"      , rc);
   }
   /*---(arguments)----------------------*/
   if (rc >= 0) {
      rc = PROG__args    (a_argc, a_argv);
      DEBUG_PROG  yLOG_value   ("args"      , rc);
   }
   /*---(begin)--------------------------*/
   if (rc >= 0) {
      rc = PROG__begin   ();
      DEBUG_PROG  yLOG_value   ("args"      , rc);
   }
   /*---(complete)-----------------------*/
   DEBUG_PROG  yLOG_exit  (__FUNCTION__);
   yURG_stage_check (YURG_MID);
   return rc;
}



/*====================------------------------------------====================*/
/*===----                      action dispatching                      ----===*/
/*====================------------------------------------====================*/
static void      o___DRIVER_____________o (void) {;}

char
PROG_pseudo             (int a_argc, char *a_argv[])
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(initialize)---------------------*/
   rc = PROG_debugging (a_argc, a_argv);
   DEBUG_PROG    yLOG_value   ("urgents"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   rc = PROG_startup   (a_argc, a_argv);
   DEBUG_PROG    yLOG_value   ("startup"   , rc);
   --rce;  if (rc < 0) {
      rc = PROG_shutdown ();
      DEBUG_PROG    yLOG_value   ("shutdown"  , rc);
      DEBUG_PROG  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(dispatch)-----------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*> argus_count   ();                                                              <*/
   /*> argus_users   ();                                                              <*/
   argus_groups  ();
   /*> argus_matrix  ();                                                              <*/
   /*> argus_list    ();                                                              <*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   /*---(wrap-up)------------------------*/
   rc = PROG_shutdown ();
   DEBUG_PROG    yLOG_value   ("shutdown"  , rc);
   /*---(complete)-----------------------*/
   return rc;
}



/*====================------------------------------------====================*/
/*===----                        program shutdown                      ----===*/
/*====================------------------------------------====================*/
static void      o___SHUTDOWN___________o (void) {;}

char
PROG__end               (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(wrap-up)------------------------*/
   DEBUG_PROG  yLOG_info    ("m_file"    , my.m_file);
   if (strcmp (my.m_file, "(stdout)") != 0)  rc = yENV_close (&(my.m_output));
   DEBUG_PROG  yLOG_value   ("close"     , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_PROG  yLOG_point   ("m_output"  , my.m_output);
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   DEBUG_PROG   yLOGS_end    ();
   return 0;
}

char
PROG_shutdown          (void)
{
   yURG_stage_check (YURG_END);
   return PROG__end ();
}


