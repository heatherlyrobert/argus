#include "argus.h"

typedef struct utmp tUTMP;

int         nutmpx      = 0;      /* utmp record count                        */
int         nwtmpx      = 0;      /* wtmp record count                        */
int         nbtmpx      = 0;      /* btmp record count                        */
int         nsulog      = 0;      /* sulog record count                       */
int         nhestia     = 0;      /* hestia record count                      */
int         nlastlogx   = 0;      /* lastlog record count                     */
int         nstty       = 0;      /* securetty record count                   */
int         npasswd     = 0;      /* passwd record count                      */
int         nshadow     = 0;      /* shadow record count                      */
int         ngroup      = 0;      /* group record count                       */
int         ngshadow    = 0;      /* gshadow record count                     */

int         hsession    = 0;      /* head utmp record in current session      */

int         sh_full     = 0;
int         sh_zero     = 0;
int         sh_other    = 0;

int         ac_long     = 0;
int         ac_odd      = 0;

int         ac_real     = 0;
int         ac_not      = 0;
int         ac_daemon   = 0;
int         ac_open     = 0;

struct  cDEBUG {
   char        files;
   char        users;
   char        groups;
   char        matrix;
   char        utmp;
   char        wtmp;
   char        btmp;
   char        current;
   char        lastlog;
} debug;


typedef struct cGROUPS tGROUPS;
typedef struct cUSER  tUSER;


struct  cGROUPS {
   char        name        [ 32];
   int         len;
   char        pass;
   int         gid;
   char        list        [500];
   int         count;
} groups [200];

struct  cUSER {
   char        name        [ 32];
   int         len;
   char        pass;
   int         uid;
   int         gid;
   char        desc        [100];
   char        home        [100];
   char        shell       [100];
   char        active;
   char        ac_type;
   char        sh_type;
   int         ties        [ 20];
   int         nties;
} users [200];


#define cleanse(x) xcleanse(x, sizeof(x))
void
xcleanse           (char *s, int len)
{
   for ( ; *s && len-- > 0; s++)
      if (!isprint(*s))
         *s = '?';
   return;
}

char *
timetostr          (const time_t time)
{
   static char s[29];    /* [Sun Sep 01 00:00:00 1998 PST] */
   if (time != 0)
      strftime(s, 29, "%a %b %d %T %Y %Z", localtime(&time));
   else
      s[0] = '\0';
   return s;
}


void
print_utline       (int  a_count, struct utmp ut)
{
   char *addr_string, *time_string;
   struct in_addr in;
   in.s_addr = ut.ut_addr;
   addr_string = inet_ntoa(in);
   time_string = timetostr(ut.ut_time);
   cleanse(ut.ut_id);
   cleanse(ut.ut_user);
   cleanse(ut.ut_line);
   cleanse(ut.ut_host);
   printf(" %04d    %1d  ", a_count, ut.ut_type);
   switch (ut.ut_type) {
   case  EMPTY         : printf ("          ");          break;
   case  RUN_LVL       : printf ("runlevel  ");          break;
   case  BOOT_TIME     : printf ("boot      ");          break;
   case  OLD_TIME      : printf ("old       ");          break;
   case  NEW_TIME      : printf ("new       ");          break;
   case  INIT_PROCESS  : printf ("init_proc ");          break;
   case  LOGIN_PROCESS : printf ("login     ");          break;
   case  USER_PROCESS  : printf ("user      ");          break;
   case  DEAD_PROCESS  : printf ("dead      ");          break;
   case  ACCOUNTING    : printf ("accounting");          break;
   }
   if (ut.ut_pid     >     0)  printf("       %6d"   , ut.ut_session);
   else                        printf("            -" );
   printf("      %-4.4s     "  , (ut.ut_id[0] == '\0') ? "-" : ut.ut_id);
   if (ut.ut_session >     0)  printf("%6ld"   , ut.ut_session);
   else                        printf("     -" );
   printf("         %-24.24s    %-20.20s    %-30.30s    %-15.15s        %-28.28s\n",
         (ut.ut_user[0] == '\0') ? "-" : ut.ut_user,
         (ut.ut_line[0] == '\0') ? "-" : ut.ut_line,
         (ut.ut_host[0] == '\0') ? "-" : ut.ut_host,
         addr_string, time_string);
   return;
}

char
argus_security     (char *a_file, char a_gname [LEN_USER], int a_perms)
{
   /*---(locals)-----------+-----------+-*/
   tSTAT       s;
   tGRP       *t;
   int         rc = 0;
   /*---(check existance)----------------*/
   rc = lstat(a_file, &s);
   if  (rc < 0) {
      RPTG_FILES  printf ("FATAL, file not found\n");
      return -1;
   }
   RPTG_FILES  printf ("exist, ");
   /*---(check type)---------------------*/
   if  (!S_ISREG(s.st_mode))  {
      RPTG_FILES  printf ("FATAL, not a regular file\n");
      return -2;
   }
   RPTG_FILES  printf ("reg, ");
   /*---(check root owner)---------------*/
   if  (s.st_uid  != 0)  {
      RPTG_FILES  printf ("FATAL, owner not root\n");
      return -3;
   }
   RPTG_FILES  printf ("root:");
   /*---(check group)--------------------*/
   t = getgrnam (a_gname);
   if (t == NULL) {
      RPTG_FILES  printf ("FATAL, group <<%s>> does not exist\n", a_gname);
      return -6;
   }
   if  (s.st_gid  != t->gr_gid)  {
      RPTG_FILES  printf ("FATAL, group not <<%d>>\n", t->gr_gid);
      return -4;
   }
   /*---(check permissions)--------------*/
   RPTG_FILES  printf ("%s, ", a_gname);
   if  ((s.st_mode & 00777) != a_perms)  {
      RPTG_FILES  printf ("  FATAL, permissions not 0%o\n", a_perms);
      return -5;
   }
   RPTG_FILES  printf ("0%o, ", a_perms);
   /*---(complete)-----------------------*/
   return 0;
}

char             /* [------] load the user records ---------------------------*/
argus_users        (void)
{
   /*---(locals)-----------+-----------+-*/
   FILE       *fp          = NULL;
   char        recd        [1000];
   char       *p           = NULL;
   char       *q           = ":";
   int         count       = 0;
   int         count2      = 0;
   int         i           = 0;
   char        x_name      [32];
   char        x_pass      [200];
   char        found;
   /*---(read passwd)--------------------*/
   fp = fopen (PASSWD, "r");
   if (fp != NULL) {
      while (1) {
         /*---(get recd)-----------------*/
         fgets(recd, 1000, fp);
         if (feof (fp)) break;
         recd [strlen (recd) - 1] = '\0';
         /*---(check comments)-----------*/
         if (recd[0] == '#' )  continue;
         if (recd[0] == '\0')  continue;
         /*---(user name)----------------*/
         p    = strtok (recd  , q);
         if (p == NULL) continue;
         users[count].len  = strlen (p);
         strncpy (users[count].name , p, 30);
         /*---(password)-----------------*/
         p    = strtok (NULL  , q);
         if (p == NULL) continue;
         if      (strcmp (p, "x") == 0)   users[count].pass = 's';
         else if (strcmp (p, "" ) == 0)   users[count].pass = 'x';
         else                             users[count].pass = '-';
         /*---(uid/gid)------------------*/
         p    = strtok (NULL  , q);
         if (p == NULL) continue;
         users[count].uid  = atoi (p);
         p    = strtok (NULL  , q);
         if (p == NULL) continue;
         users[count].gid  = atoi (p);
         /*---(desc)---------------------*/
         p    = strtok (NULL  , q);
         if (p == NULL) continue;
         strncpy (users[count].desc , p, 90);
         /*---(home)---------------------*/
         p    = strtok (NULL  , q);
         if (p == NULL) continue;
         strncpy (users[count].home , p, 90);
         /*---(shell)--------------------*/
         p    = strtok (NULL  , q);
         if (p == NULL) {
            strncpy (users[count].shell, users[count].home , 90);
            strncpy (users[count].home , users[count].desc , 90);
            strncpy (users[count].desc , "-"               , 90);
         } else {
            strncpy (users[count].shell, p, 90);
         }
         /*---(shell type)---------------*/
         users[count].sh_type  = 's';
         if      (strcmp (users[count].shell, "/bin/sh"       ) == 0)       users[count].sh_type = '#';
         else if (strcmp (users[count].shell, "/bin/bash"     ) == 0)       users[count].sh_type = '#';
         else if (strcmp (users[count].shell, "/bin/dash"     ) == 0)       users[count].sh_type = '#';
         else if (strcmp (users[count].shell, "/bin/csh"      ) == 0)       users[count].sh_type = '#';
         else if (strcmp (users[count].shell, "/bin/tcsh"     ) == 0)       users[count].sh_type = '#';
         else if (strcmp (users[count].shell, "/bin/zsh"      ) == 0)       users[count].sh_type = '#';
         else if (strcmp (users[count].shell, "/dev/null"     ) == 0)       users[count].sh_type = '-';
         else if (strcmp (users[count].shell, "/bin/false"    ) == 0)       users[count].sh_type = '-';
         else if (strcmp (users[count].shell, "/sbin/nologin" ) == 0)       users[count].sh_type = '-';
         if (users[count].sh_type == '#')   ++sh_full;
         if (users[count].sh_type == '-')   ++sh_zero;
         if (users[count].sh_type == 's')   ++sh_other;
         /*---(type)---------------------*/
         users[count].ac_type = '-';
         users[count].active  = '?';
         users[count].nties   = 0;
         /*---(name check)---------------*/
         if (users[count].len > 8)   ++ac_long;
         found = 0;
         for (i = 0; i < users[count].len; ++i) {
            if (strchr ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_", users[count].name[i]) != 0) continue;
            ++found;
         }
         if (found > 0)  ++ac_odd;
         /*---(done)---------------------*/
         ++count;
      }
      fclose(fp);
   }
   /*---(open shadow)--------------------*/
   fp = fopen (SHADOW, "r");
   if (fp != NULL) {
      while (1) {
         /*---(get recd)-----------------*/
         fgets(recd, 1000, fp);
         if (feof (fp)) break;
         recd [strlen (recd) - 1] = '\0';
         /*---(check comments)-----------*/
         if (recd[0] == '#' )  continue;
         if (recd[0] == '\0')  continue;
         /*---(user name)----------------*/
         p    = strtok (recd  , q);
         if (p == NULL) continue;
         strncpy (x_name , p, 30);
         /*---(password)-----------------*/
         p    = strtok (NULL  , q);
         if (p == NULL) continue;
         strncpy (x_pass , p, 150);
         /*---(find user)----------------*/
         found = 0;
         for (i = 0; i < count; ++i) {
            if (strcmp (users[i].name, x_name) != 0)  continue;
            found = 1;
            users[i].active = '-';
            if      (x_pass [0] == '*' )     users[i].ac_type = '-';
            else if (x_pass [0] == '!' )     users[i].ac_type = 'd';
            else if (x_pass [0] == '\0')     users[i].ac_type = '?';
            else  {
               users[i].active  = 'y';
               users[i].ac_type = '#';
            }
            if (users[i].ac_type == '#')   ++ac_real;
            if (users[i].ac_type == '-')   ++ac_not;
            if (users[i].ac_type == 'd')   ++ac_daemon;
            if (users[i].ac_type == '?')   ++ac_open;
            break;
         }
         /*---(done----------------------*/
         ++count2;
      }
      fclose(fp);
   }
   /*---(output)-------------------------*/
   RPTG_USERS  printf ("\n");
   RPTG_USERS  printf ("##  -------------name---------------  len  p  r  t  --uid--  --gid--  -------------desc---------------  -------------home---------------  -------------shell--------------  sh  grp\n");
   for (i = 0; i < count; ++i) {
      RPTG_USERS  if (i % 5 == 0) printf ("\n");
      RPTG_USERS  printf ("%2d  %-32.32s  %3d  %c  %c  %c   %5d    %5d   %-32.32s  %-32.32s  %-32.32s  %c  %3d\n", i, users[i].name, users[i].len , users[i].pass, users[i].active , users[i].ac_type, users[i].uid , users[i].gid , users[i].desc, users[i].home, users[i].shell, users[i].sh_type, users[i].nties);
   }
   RPTG_USERS  printf ("\n");
   RPTG_USERS  printf ("##  -------------name---------------  len  p  r  t  --uid--  --gid--  -------------desc---------------  -------------home---------------  -------------shell--------------  sh  grp\n");
   RPTG_USERS  printf ("\n");
   RPTG_USERS  printf ("   users          %4d\n", count);
   RPTG_USERS  printf ("   names--------  ----   ----------------------------------\n");
   RPTG_USERS  printf ("      long        %4d    names over 8 chars                \n", ac_long);
   RPTG_USERS  printf ("      odd         %4d    includes punctuation              \n", ac_odd);
   RPTG_USERS  printf ("   accounts-----  ----   ----------------------------------\n");
   RPTG_USERS  printf ("      real     #  %4d    (functional)                      \n", ac_real);
   RPTG_USERS  printf ("      disabled -  %4d                                      \n", ac_not);
   RPTG_USERS  printf ("      daemon   d  %4d                                      \n", ac_daemon);
   RPTG_USERS  printf ("      open        %4d    (without password)                \n", ac_open);
   RPTG_USERS  printf ("      missing  ?  %4d                                      \n", count - ac_real - ac_not - ac_daemon - ac_open);
   RPTG_USERS  printf ("   shells-------  ----   ----------------------------------\n");
   RPTG_USERS  printf ("      full     #  %4d   (sh, bash, dash, csh, tcsh, zsh)\n"   , sh_full);
   RPTG_USERS  printf ("      zero     -  %4d   (false, null, nologin)\n"             , sh_zero);
   RPTG_USERS  printf ("      other    s  %4d   (halt, shutdown, application, ...)\n" , sh_other);
   RPTG_USERS  printf ("      missing     %4d                                     \n" , count - sh_full - sh_zero - sh_other);
   /*---(complete)-----------------------*/
   return 0;
}

char
argus_groups            (void)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_name      [LEN_USER]  = "";
   int         x_gid       =    0;
   char        x_quality   =  '-';
   char        x_active    =  '-';
   char        x_nuser     =    0;
   char        x_unames    [LEN_HUND]  = "";
   char        x_uids      [LEN_HUND]  = "";
   int         c           =    0;
   char        t           [LEN_LABEL] = "";
   char        s           [LEN_FULL]  = "";
   char        r           [LEN_FULL]  = "";
   /*---(header)-------------------------*/
   DEBUG_PROG   yLOG_enter   (__FUNCTION__);
   /*---(first)--------------------------*/
   rc = yENV_group_by_cursor (YDLST_HEAD, x_name, &x_gid, &x_quality, &x_active, &x_nuser, x_unames, x_uids);
   DEBUG_PROG   yLOG_value   ("cursor"    , rc);
   while (rc >= 0) {
      RPTG_GROUPS  SHOW_exim (RUN_GROUPS, c, my.m_exim, my.m_titles);
      RPTG_GROUPS  fprintf (my.m_output, "%2d  %5d  %2d %-12.12s  %c  %c  %3d  %2d %-50.50s  %2d %-40.40s  Ï\n", c, x_gid, strlen (x_name), x_name, x_quality, x_active, x_nuser, strlen (x_unames), x_unames, strlen (x_uids), x_uids);
      ++c;
      rc = yENV_group_by_cursor (YDLST_NEXT, x_name, &x_gid, &x_quality, &x_active, &x_nuser, x_unames, x_uids);
      DEBUG_PROG   yLOG_value   ("cursor"    , rc);
   }
   /*---(report suffix)------------------*/
   RPTG_GROUPS  SHOW_exim (RUN_GROUPS, -1, my.m_exim, my.m_titles);
   /*---(complete)-----------------------*/
   DEBUG_PROG   yLOG_exit    (__FUNCTION__);
   return 0;
}

char             /* [------] analyze groups ----------------------------------*/
argus_groups_OLD   (void)
{
   /*---(locals)-----------+-----------+-*/
   FILE       *fp          = NULL;
   char        recd        [1000];
   char       *p           = NULL;
   char       *r           = NULL;
   char       *q           = ":";
   int         count       = 0;
   int         i           = 0;
   char       *s           = NULL;
   char       *t           = NULL;
   /*---(read passwd)--------------------*/
   fp = fopen (GROUP, "r");
   if (fp != NULL) {
      while (1) {
         /*---(get recd)-----------------*/
         fgets(recd, 1000, fp);
         if (feof (fp)) break;
         recd [strlen (recd) - 1] = '\0';
         /*---(check comments)-----------*/
         if (recd[0] == '#' )  continue;
         if (recd[0] == '\0')  continue;
         /*---(init)---------------------*/
         groups[count].name[0]  = '\0';
         groups[count].len      = 0;
         groups[count].pass     = '-';
         groups[count].gid      = 666;
         groups[count].list[0]  = '\0';
         groups[count].count    = 0;
         /*---(user name)----------------*/
         p    = strtok_r (recd  , q, &s);
         if (p == NULL) continue;
         groups[count].len  = strlen (p);
         strncpy (groups[count].name , p, 30);
         /*---(password)-----------------*/
         p    = strtok_r (NULL  , q, &s);
         if (p == NULL) continue;
         if      (strcmp (p, "x") == 0)   groups[count].pass = 's';
         else                             groups[count].pass = '-';
         /*---(gid)----------------------*/
         p    = strtok_r (NULL  , q, &s);
         if (p == NULL) continue;
         groups[count].gid  = atoi (p);
         /*---(list)---------------------*/
         strncpy (groups[count].list , "", 490);
         groups [count].count = 0;
         p    = strtok_r (NULL  , q, &s);
         if (p != NULL) {
            strncpy (groups[count].list , p, 490);
            r    = strtok_r (p     , ",", &t);
            while (r != NULL) {
               ++groups[count].count;
               for (i = 0; i < npasswd; ++i) {
                  if (strcmp (r, users [i].name) != 0)  continue;
                  users [i].ties [users[i].nties++] = count;
                  break;
               }
               r    = strtok_r (NULL  , ",", &t);
            }
         }
         /*---(done)---------------------*/
         ++count;
      }
   }
   /*---(output)-------------------------*/
   RPTG_GROUPS  printf ("##  -------------name---------------  len  p  --gid--  -----------------------------list-------------------------------  cnt\n");
   for (i = 0; i < count; ++i) {
      RPTG_GROUPS  if (i % 5 == 0) printf ("\n");
      RPTG_GROUPS  printf ("%2d  %-32.32s  %3d  %c   %5d   %-64.64s  %3d\n", i, groups[i].name, groups[i].len , groups[i].pass , groups[i].gid , groups[i].list, groups[i].count);
   }
   RPTG_GROUPS  printf ("\n");
   RPTG_GROUPS  printf ("##  -------------name---------------  len  p  --gid--  -----------------------------list-------------------------------  cnt\n");
   /*---(complete)-----------------------*/
   return 0;
}

char             /* [------] show the user/group matrix ----------------------*/
argus_matrix       (void)
{
   /*---(locals)-----------+-----------+-*/
   FILE       *fp          = NULL;
   char        recd        [1000];
   char       *p           = NULL;
   char       *q           = ":";
   int         count       = 0;
   int         i           = 0;
   int         j           = 0;
   int         k           = 0;
   char        f           = 0;
   char        grp_label   [20] = "--group--";
   /*---(report title)-------------------*/
   RPTG_MATRIX  printf ("##   report      åuser/group assignment matrixæ\n");
   RPTG_MATRIX  printf ("\n");
   /*---(group numbers arcoss top)-------*/
   RPTG_MATRIX  {
      for (i = 2; i >= 0; --i) {
         printf ("                              ");
         for (k = 0; k < ngroup; ++k) {
            if (k % 3 == 0) printf ("  ");
            if (i == 2) {
               if (k < 10)  printf ("  ");
               else         printf (" %d", k / 10);
            }
            else if (i == 1) printf (" %d", k % 10);
            else {
               if (groups[k].len > 10)  printf (" +");
               else                     printf (" -");
            }
         }
         printf ("\n");
      }
   }
   /*---(group name header)--------------*/
   RPTG_MATRIX  {
      for (i = 9; i >= 0; --i) {
         if (i == 0) printf ("##  ---users---  -uid-  -gid- ");
         else        printf ("                            %c ", grp_label [9 - i]);
         for (k = 0; k < ngroup; ++k) {
            if (k % 3 == 0) printf ("  ");
            if (groups[k].len <= i) {
               printf (" ·");
               continue;
            }
            printf (" %c", groups[k].name[groups[k].len - 1 - i]);
         }
         if (i == 0)  printf ("   cnt  sh  ac  !!\n");
         else         printf ("   %c\n", grp_label [9 - i]);
      }
   }
   /*---(cycle users)--------------------*/
   RPTG_MATRIX  {
      for (i = 0; i < npasswd; ++i) {
         if (i % 3 == 0) printf ("\n");
         printf ("%2d  %-10.10s%c  %5d  %5d ", i,
               users[i].name, (users[i].len > 10) ? '+' : ' ', users[i].uid , users[i].gid);
         for (k = 0; k < ngroup; ++k) {
            if (k % 3 == 0) printf ("  ");
            f = 0;
            for (j = 0; j < users[i].nties; ++j) {
               if (k != users[i].ties[j]) continue;
               ++f;
            }
            if (f > 0) {
               printf (" @");
            } else {
               if (groups[k].gid == users[i].gid) {
                  printf (" #");
                  ++groups[k].count;
                  ++users[i].nties;
                  continue;
               } else {
                  printf (" -");
               }
            }
         }
         printf ("    %2d   %c   %c  %s\n", users[i].nties, users[i].sh_type,
               users[i].ac_type,
               (users[i].active == 'y') ? "++" : "");
      }
   }
   /*---(group count footer)-------------*/
   RPTG_MATRIX  {
      for (i = 0; i <= 2; ++i) {
         if (i == 2)  printf ("                      totals  ");
         else         printf ("                              ");
         for (k = 0; k < ngroup; ++k) {
            if (k % 3 == 0) printf ("  ");
            if (i == 1) {
               if (groups[k].count < 10)  printf ("  ");
               else                       printf (" %d", groups[k].count / 10);
            }
            else if (i == 2) printf (" %d", groups[k].count % 10);
            else             printf ("  ");
         }
         printf ("\n");
      }
   }
   RPTG_MATRIX  printf ("\n");
   /*---(complete)-----------------------*/
   return 0;
}

char             /* [------] count the records in user records ---------------*/
argus_count        (void)
{
   /*---(locals)-----------+-----------+-*/
   FILE       *fp          = NULL;
   tUTMP       ut;
   char        recd        [1000];
   int         all_file    = 0;
   int         bad_file    = 0;
   /*---(header)-------------------------*/
   RPTG_FILES  printf ("\n");
   RPTG_FILES  printf ("verifying files are in proper order...\n");
   /*---(passwd)-------------------------*/
   RPTG_FILES  printf ("   - passwd    , users          [%-40.40s]   ", PASSWD);
   ++all_file;
   if (argus_security (PASSWD, "root", 0644) == 0) {
      fp = fopen (PASSWD, "r");
      if (fp != NULL) {
         fgets(recd, 1000, fp);
         while (!feof (fp)) {
            if (strchr("# \0", recd[0] == 0))  ++npasswd;
            fgets(recd, 1000, fp);
         }
         fclose(fp);
         RPTG_FILES  printf ("found %6d records\n", npasswd);
      } else {
         RPTG_FILES  printf ("NOT FOUND  ------\n");
         ++bad_file;
      }
   } else {
      ++bad_file;
   }
   /*---(shadow)-------------------------*/
   RPTG_FILES  printf ("   - shadow    , passwords      [%-40.40s]   ", SHADOW);
   ++all_file;
   if (argus_security (SHADOW, "root", 0600) == 0) {
      fp = fopen (SHADOW, "r");
      if (fp != NULL) {
         fgets(recd, 1000, fp);
         while (!feof (fp)) {
            if (strchr("# \0", recd[0] == 0))  ++nshadow;
            fgets(recd, 1000, fp);
         }
         fclose(fp);
         RPTG_FILES  printf ("found %6d records\n", nshadow);
      } else {
         RPTG_FILES  printf ("NOT FOUND  ------\n");
         ++bad_file;
      }
   } else {
      ++bad_file;
   }
   /*---(group)--------------------------*/
   RPTG_FILES  printf ("   - group     , groups         [%-40.40s]   ", GROUP);
   ++all_file;
   if (argus_security (GROUP, "root", 0644) == 0) {
      fp = fopen (GROUP, "r");
      if (fp != NULL) {
         fgets(recd, 1000, fp);
         while (!feof (fp)) {
            if (strchr("# \0", recd[0] == 0))  ++ngroup;
            fgets(recd, 1000, fp);
         }
         fclose(fp);
         RPTG_FILES  printf ("found %6d records\n", ngroup);
      } else {
         RPTG_FILES  printf ("NOT FOUND  ------\n");
         ++bad_file;
      }
   } else {
      ++bad_file;
   }
   /*---(gshadow)------------------------*/
   RPTG_FILES  printf ("   - gshadow   , group pass     [%-40.40s]   ", GSHADOW);
   ++all_file;
   if (argus_security (GSHADOW, "root", 0600) == 0) {
      fp = fopen (GSHADOW, "r");
      if (fp != NULL) {
         fgets(recd, 1000, fp);
         while (!feof (fp)) {
            if (strchr("# \0", recd[0] == 0))  ++ngshadow;
            fgets(recd, 1000, fp);
         }
         fclose(fp);
         RPTG_FILES  printf ("found %6d records\n", ngshadow);
      } else {
         RPTG_FILES  printf ("NOT FOUND  ------\n");
         ++bad_file;
      }
   } else {
      ++bad_file;
   }
   /*---(utmpx)--------------------------*/
   RPTG_FILES  printf ("   - utmpx     , current        [%-40.40s]   ", UTMP);
   ++all_file;
   if (argus_security (UTMP, "root", 0644) == 0) {
      fp = fopen (UTMP, "r");
      if (fp != NULL) {
         while (fread(&ut, sizeof ut, 1, fp) == 1) {
            ++nutmpx;
         }
         fclose(fp);
         RPTG_FILES  printf ("found %6d records\n", nutmpx);
      } else {
         RPTG_FILES  printf ("NOT FOUND\n");
         ++bad_file;
      }
   } else {
      ++bad_file;
   }
   /*---(wtmpx)--------------------------*/
   RPTG_FILES  printf ("   - wtmpx     , historical     [%-40.40s]   ", WTMP);
   ++all_file;
   if (argus_security (WTMP, "root", 0644) == 0) {
      fp = fopen (WTMP, "r");
      if (fp != NULL) {
         while (fread(&ut, sizeof ut, 1, fp) == 1) {
            if (ut.ut_type == BOOT_TIME) hsession = nwtmpx;
            ++nwtmpx;
         }
         fclose(fp);
         RPTG_FILES  printf ("found %6d records, hsession = %6d\n", nwtmpx, hsession);
      } else {
         RPTG_FILES  printf ("NOT FOUND\n");
         ++bad_file;
      }
   } else {
      ++bad_file;
   }
   /*---(btmpx)--------------------------*/
   RPTG_FILES  printf ("   - btmpx     , failures       [%-40.40s]   ", BTMP);
   ++all_file;
   if (argus_security (BTMP, "root", 0600) == 0) {
      fp = fopen (BTMP, "r");
      if (fp != NULL) {
         while (fread(&ut, sizeof ut, 1, fp) == 1)   ++nbtmpx;
         fclose(fp);
         RPTG_FILES  printf ("found %6d records\n", nbtmpx);
      } else {
         RPTG_FILES  printf ("NOT FOUND  ------\n");
         ++bad_file;
      }
   } else {
      ++bad_file;
   }
   /*---(hestia)-------------------------*/
   RPTG_FILES  printf ("   - hestia    , getty          [%-40.40s]   ", HESTIA);
   ++all_file;
   if (argus_security (HESTIA, "root", 0600) == 0) {
      fp = fopen (HESTIA, "r");
      if (fp != NULL) {
         fgets(recd, 1000, fp);
         while (!feof (fp)) {
            if (strchr("# \0", recd[0] == 0))  ++nhestia;
            fgets(recd, 1000, fp);
         }
         fclose(fp);
         RPTG_FILES  printf ("found %6d records\n", nhestia);
      } else {
         RPTG_FILES  printf ("NOT FOUND  ------\n");
         ++bad_file;
      }
   } else {
      ++bad_file;
   }
   /*---(sulog)--------------------------*/
   RPTG_FILES  printf ("   - sulog     , switches       [%-40.40s]   ", SULOG);
   ++all_file;
   if (argus_security (SULOG, "root", 0644) == 0) {
      fp = fopen (SULOG, "r");
      if (fp != NULL) {
         fgets(recd, 1000, fp);
         while (!feof (fp)) {
            if (strchr("# \0", recd[0] == 0))  ++nsulog;
            fgets(recd, 1000, fp);
         }
         fclose(fp);
         RPTG_FILES  printf ("found %6d records\n", nsulog);
      } else {
         RPTG_FILES  printf ("NOT FOUND  ------\n");
         ++bad_file;
      }
   } else {
      ++bad_file;
   }
   /*---(securetty)----------------------*/
   RPTG_FILES  printf ("   - securetty , root ttys      [%-40.40s]   ", TTY);
   ++all_file;
   if (argus_security (TTY, "root", 0600) == 0) {
      fp = fopen (TTY, "r");
      if (fp != NULL) {
         fgets(recd, 1000, fp);
         while (!feof (fp)) {
            if (strchr("# \0", recd[0] == 0))  ++nstty;
            fgets(recd, 1000, fp);
         }
         fclose(fp);
         RPTG_FILES  printf ("found %6d records\n", nstty);
      } else {
         RPTG_FILES  printf ("NOT FOUND  ------\n");
         ++bad_file;
      }
   } else {
      ++bad_file;
   }
   /*---(lastlog)------------------------*/
   RPTG_FILES  printf ("   - lastlog   , last use       [%-40.40s]   ", LASTLOG);
   ++all_file;
   if (argus_security (LASTLOG, "root", 0644) == 0) {
      fp = fopen (LASTLOG, "r");
      if (fp != NULL) {
         fgets(recd, 1000, fp);
         while (!feof (fp)) {
            if (strchr("# \0", recd[0] == 0))  ++nlastlogx;
            fgets(recd, 1000, fp);
         }
         fclose(fp);
         RPTG_FILES  printf ("found %6d records\n", nlastlogx);
      } else {
         RPTG_FILES  printf ("NOT FOUND  ------\n");
         ++bad_file;
      }
   } else {
      ++bad_file;
   }
   /*---(summary)------------------------*/
   RPTG_FILES  {
      printf ("\n");
      printf ("file count  = %3d\n", all_file);
      printf ("error files = %3d\n", bad_file);
      printf ("\n");
      printf ("NOTES:\n");
      printf ("   1) ultimately all files should be root:root and minimum 0644 (some 0600)\n");
      printf ("   2) several files are deprecated, so hackers might forget to cover their tracks\n");
   }
   /*---(complete)-----------------------*/
   return 0;
}

char
argus_list         (void)
{
   FILE       *fp          = NULL;
   struct utmp ut;
   int    count  = 0;
   int    shown  = 0;
   int    lshown = 0;
   printf ("\n");
   RPTG_UTMP {
      fp = fopen(UTMP, "r");
      printf ("report format, current access records (--utmp)\n");
   } else  RPTG_CURRENT {
      fp = fopen(WTMP, "r");
      printf ("report format, current session historical access records (--current)\n");
   } else  RPTG_WTMP {
      fp = fopen(WTMP, "r");
      printf ("report format, historical access records (--wtmp)\n");
   } else  RPTG_BTMP {
      fp = fopen(BTMP, "r");
      printf ("report format, failed access records (--btmp)\n");
   } else {
      return 0;
   }
   if (fp == NULL) {
      perror("Unable to open file");
      exit(1);
   }
   printf ("\n");
   printf ("ref_no  -----type-----      ---pid---  ---id---  ---sid---      -----------user-----------  ---------line---------  --------------host--------------  -------addr------      -------------time-------------\n");
   printf ("\n");
   while (fread(&ut, sizeof ut, 1, fp) == 1) {
      lshown = shown;
      RPTG_UTMP {
         if (count >= hsession) {
            print_utline(count, ut);
            ++shown;
         }
      } else {
         print_utline(count, ut);
         ++shown;
      }
      if ((shown != lshown) && (shown % 5 == 0)) printf ("\n");
      ++count;
   }
   printf ("\n");
   printf ("ref_no  -----type-----      ---pid---  ---id---  ---sid---      -----------user-----------  ---------line---------  --------------host--------------  -------addr------      -------------time-------------\n");
   printf ("\n");
   printf ("showed %6d of %6d records\n", shown, count);
   fclose(fp);
   return;
}



/*============================[[    end-code    ]]============================*/
