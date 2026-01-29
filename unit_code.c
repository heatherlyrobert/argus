#include    "argus.h"
#include    <yEXEC_solo.h>
#include    <yEXEC_uver.h>



static char u_home  [LEN_FULL]  = "";



char         /*-> set up programgents/debugging ------[ light  [uz.320.011.05]*/ /*-[00.0000.00#.#]-*/ /*-[--.---.---.--]-*/
PROG__unit_quiet        (void)
{
   char        x_argc      = 1;
   char       *x_args [1]  = { "argus" };
   getcwd (u_home, LEN_PATH);
   PROG_debugging (x_argc, x_args);
   PROG_startup   (x_argc, x_args);
   return 0;
}

char         /*-> set up programgents/debugging ------[ light  [uz.320.011.05]*/ /*-[00.0000.00#.!]-*/ /*-[--.---.---.--]-*/
PROG__unit_loud         (void)
{
   char        x_argc      = 3;
   char       *x_args [3]  = { "argus_unit", "@@kitchen", "@@ystr" };
   getcwd (u_home, LEN_PATH);
   PROG_debugging (x_argc, x_args);
   PROG_startup   (x_argc, x_args);
   return 0;
}

char         /*-> set up program urgents/debugging ---[ light  [uz.210.001.01]*/ /*-[00.0000.00#.!]-*/ /*-[--.---.---.--]-*/
PROG__unit_end          (void)
{
   chdir (u_home);
   PROG_shutdown  ();
   return 0;
}



/*====================------------------------------------====================*/
/*===----                      unit-testing                            ----===*/
/*====================------------------------------------====================*/
static void  o___STRING__________o () { return; }

char
PROG_string             (char a_type, char a_string [LEN_FULL])
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =   -5;
   char        rc          =    0;
   char        x_argc      =    0;
   char       *x_argv      [LEN_TERSE];
   char        x_disp      [LEN_FULL]  = "";
   /*---(header)-------------------------*/
   DEBUG_PROG  yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_PROG    yLOG_point   ("a_string"     , a_string);
   --rce;  if (a_string == NULL) {
      DEBUG_PROG  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_PROG    yLOG_info    ("a_string"     , a_string);
   /*---(parse)--------------------------*/
   rc = yexec_uparse (a_string, &x_argc, x_argv, x_disp);
   DEBUG_PROG  yLOG_value   ("config"    , rc);
   --rce;  if (rc <  0) {
      DEBUG_PROG  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_PROG    yLOG_info    ("x_disp"       , x_disp);
   /*---(execute)------------------------*/
   DEBUG_PROG  yLOG_char    ("a_type"    , a_type);
   --rce;  switch (a_type) {
   case 'd' :
      /*---(args-only)-------------------*/
      rc = PROG_debugging  (x_argc, x_argv);
      DEBUG_PROG  yLOG_value     ("debugging" , rc);
      break;
   case 'i' :
      /*---(args-only)-------------------*/
      rc = PROG__init      (x_argc, x_argv);
      DEBUG_PROG  yLOG_value     ("init"      , rc);
      break;
   case 'a' :
      /*---(args-only)-------------------*/
      rc = PROG__args      (x_argc, x_argv);
      DEBUG_PROG  yLOG_value     ("args"      , rc);
      break;
   case 's' :
      /*---(startup-only)----------------*/
      rc = PROG_startup    (x_argc, x_argv);
      DEBUG_ARGS  yLOG_value     ("startup"   , rc);
      break;
   case 'm' :
      /*---(main/pseudo)-----------------*/
      rc = PROG_pseudo     (x_argc, x_argv);
      DEBUG_PROG  yLOG_value     ("main"      , rc);
      break;
   default  :
      /*---(unknown)---------------------*/
      DEBUG_PROG  yLOG_note    ("a_type is unknown/illegal");
      DEBUG_PROG  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check trouble)------------------*/
   if (rc <  0) {
      DEBUG_PROG  yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   /*---(complete)-----------------------*/
   DEBUG_PROG  yLOG_exit  (__FUNCTION__);
   return 1;
}



/*====================------------------------------------====================*/
/*===----                      unit-testing                            ----===*/
/*====================------------------------------------====================*/
static void  o___ACCESSOR________o () { return; }


char*        /*-[ unit test accessor ]---------------------[us2---·A-7·6--·B21-]¬[----·T-B1H---·---·----]¬[---·------]*/
PROG__unit              (char *a_question, int i)
{
   /*---(locals)-----------+-----------+-*/
   char        s           [LEN_TITLE] = " 0[]";
   char        t           [LEN_TITLE] = " 0[]";
   /*---(defense)------------------------*/
   snprintf (unit_answer, LEN_RECD, "PROG unit        : question unknown");
   /*---(simple)-------------------------*/
   if (strcmp (a_question, "reporting")       == 0) {
      snprintf (unit_answer, LEN_RECD, "PROG reporting   : file %c, user %c, grup %c, matx %c, utmp %c, wtmp %c, btmp %c, curr %c, last %c",
            SHOW_FILES     , SHOW_USERS     , SHOW_GROUPS    , SHOW_MATRIX    , SHOW_UTMP      , SHOW_WTMP      , SHOW_BTMP      , SHOW_CURRENT   , SHOW_LASTLOG   );
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}
